#include "stdafx.h"

#include "TC_Scoreboard.h"
#include "../Tower/RollingStock.h"
#include "../Tower/Scoreboard.h"
#include "../Tower/ExplainMgr.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Scoreboard );

/*
    bef - дальний подъезд к горке
    pre - непосредственный подход к горочному светофору
    top - вершина горки (сразу за горочным светофором)
    next - следующий участок после top
    aft  - и далее по горке

    Однострочное описание отцепов и их положения:

    вертикаль (|) - границы участков
    буквами - обозначены вагоны (Car)
    :цифры> - голова отцепа и расстояние до стыка (Train) при движении вправо
    <цифры: - голова отцепа и расстояние до стыка (Train) при движении влево

    "bef|    pre    |    top   |    next    | aft"
    "---+-----------+==========+------------+----"
    "   | edcb:24> a|:32>      |            |    "
    "   |  edcb:5>  |a:11>     |            |    "
    "   |    edcb:0>|a:3>      |            |    " 
    "   | edc:14>   |b:32> a:0>|            |    " 
    "   | edc:14>   |b:18>     | a:40>      |    " 
    "   |         ed|c:31>     | b:40> a:11>|    " 
*/

using namespace RollingStock;

const EsrKit esr(7000);
const BadgeE IU[] = {
    BadgeE( L"bef", esr),
    BadgeE( L"pre", esr),
    BadgeE( L"top", esr),
    BadgeE( L"nex", esr),
    BadgeE( L"aft", esr)
    };

const int TOP_WIDTH = 31;
const DB_ADDR carDBA( 33000 );
const CarNumber  carNUM( 44440000 );

/*
Класс Config для наглядного задания расположения отцепов.
    буквы abc..z обозначают вагоны
    <> - направление движения и положение головы
    :99> или <99: - расстояние от головы до стыка
    например:  ba:27> два вагона в 27 метров от стыка справа
    вся область разбита на участки символами I. Слева направо это " pre I top I next "
*/
class Config
{
public:
    Config( Model& _model );
    bool build( boost::string_ref descr );
    bool empty() const;
    Train    get_train();
    ListCars get_listcars();
private:
    Model& m_model;
    std::deque<Train> m_trains;
    std::deque<ListCars> m_listcars;

    static const char I = '|'; // ограничитель участков
    static const char L = '<'; // направление влево
    static const char R = '>'; // направление вправо
    static const char S = ':'; // разделитель вагонов и расстояния до стыка
    struct Row
    {
        std::string letter; // буквы вагонов отцепа
        int  dist;          // расстояние
        char direction;     // направление L/R
        size_t head_section;// номер iu начиная с 1
    };
    static std::vector<Row> parse( const boost::string_ref area );
    void collect( const boost::string_ref area, const std::vector<Row>& rows );
    void tune_train();
    DB_ADDR make_train_dba() const;
};

Config::Config( Model& _model )
    : m_model( _model )
{
}

bool Config::build( boost::string_ref area )
{
    m_trains.clear();
    m_listcars.clear();
    // выделение отцепов
    std::vector<Row> rows = parse( area );
    // построение поездов
    collect( area, rows );
    // уточнение адресов поездов
    tune_train();
    // 
    return !m_trains.empty() && m_trains.size() == m_listcars.size();
}


bool Config::empty() const
{
    return m_trains.empty() && m_listcars.empty();
}

Train Config::get_train()
{
    Train res;
    if ( !m_trains.empty() ) {
        res = m_trains.front();
        m_trains.pop_front();
    } else {
        CPPUNIT_FAIL( "No more trains!" );
    }
    return res;
}


ListCars Config::get_listcars()
{
    ListCars res;
    if ( !m_listcars.empty() ) {
        res = m_listcars.front();
        m_listcars.pop_front();
    } else {
        CPPUNIT_FAIL( "No more listcars!" );
    }
    return res;
}

std::vector<Config::Row> Config::parse( const boost::string_ref area )
{
    std::vector<Row> res;
    size_t iu_pos = 0;
    for( size_t i = 0; i < area.size(); /*none*/ ) {
        if ( I == area[i] ) {
            ++iu_pos;
            ++i;
        } else if ( area[i] != L && !std::isalpha( area[i] ) ) {
            // отцеп начинается с '<' либо с буквы вагона
            ++i;
        } else if ( std::isalpha( area[ i ] ) ) {
            // распознавание правого отцепа: вагоны, затем расстояние, затем признак головы
            Row row;
            row.letter.clear();
            row.direction = R;

            while( i<area.size() && ( std::isalpha( area[i] ) || I == area[i] ) ) {
                if ( std::isalpha( area[i] ) )
                    row.letter.push_back( area[i] );
                else if ( I == area[i] )
                    ++iu_pos;
                ++i;
            }
            if ( i>=area.size() || S != area[i] )
                CPPUNIT_FAIL( "некорректный отцеп" );
            ++i;
            row.dist = 0;
            while( i<area.size() && std::isdigit( area[i] ) ) {
                row.dist = row.dist * 10 + (area[i] - '0');
                ++i;
            }
            if ( i>=area.size() || R != area[i] )
                CPPUNIT_FAIL( "некорректный отцеп без >" );
            row.head_section = iu_pos;
            res.push_back( row );
            ++i;
        } else if ( L == area[ i ] ) {
            // распознавание левого отцепа: признак головы, затем расстояние, затем вагоны
            CPPUNIT_FAIL( "не реализовано т.к. пока не потребовалось" );
        } else {
            CPPUNIT_FAIL( "недостижимая точка" );
        }
    }
    // должно быть описание всех участков
    if ( iu_pos+1 != size_array(IU) )
        CPPUNIT_FAIL( "Неверное количество участков в разбиении исходной строки" );
    return res;
}

void Config::collect( const boost::string_ref area, const std::vector<Row>& rows )
{
    Train train;
    train.speed( 5 );

    ListCars lc;

    Car car;
    car.length( 20 );
    car.gross_weight( 11 );
    car.axis( 4 );

    for( const Row& r : rows ) {
        lc.dba_train = DB_ADDR();
        lc.dba_car.clear();

        for( auto lett : r.letter ) {
            int n = (lett-'a')+1;
            car.dba( DB_ADDR( carDBA + n ) );
            car.number( CarNumber(carNUM + n) );
            car.destination( std::to_wstring(n)+L"п_2" );

            lc.dba_car.push_back( car.dba() );
            m_model.put( car );
        }
        train.dba( DB_ADDR() );
        train.dist( r.dist );
        if ( R == r.direction ) {
            CPPUNIT_ASSERT( r.head_section < size_array(IU) );
            CPPUNIT_ASSERT( r.head_section+1 < size_array(IU) );
            train.head( IU[ r.head_section ] );
            train.ahead( IU[ r.head_section+1 ] );
            // вагоны хранятся от головы - перевернуть список
            std::reverse( lc.dba_car.begin(), lc.dba_car.end() );
        } else if ( L == r.direction ) {
            CPPUNIT_ASSERT( r.head_section < size_array(IU) );
            CPPUNIT_ASSERT( r.head_section-1 < size_array(IU) );
            train.head( IU[ r.head_section ] );
            train.ahead( IU[ r.head_section-1 ] );
        } else {
            CPPUNIT_FAIL("incorrect direction");
        }

        m_trains.push_back( train );
        m_listcars.push_back( lc );
    }
}

void Config::tune_train()
{
    // восстановить адреса поездов по модели (совпадение состава)
    for( size_t i=0; m_listcars.size() > i; ++i ) {
        // имеем только набор вагонов - надо найти такой же поезд
        Train& train = m_trains[i];
        ListCars& lc = m_listcars[i];

        DB_ADDR temp_dba_train;
        if ( m_model.in_train( lc.dba_car.front(), &temp_dba_train) ) {
            // проверить полное совпадение состава
            auto cars = m_model.get_cars( temp_dba_train );
            auto exist = [&cars]( DB_ADDR dba ) {
                return cars.end() != std::find(cars.begin(), cars.end(), dba );
            };
            bool all = std::all_of( lc.dba_car.begin(), lc.dba_car.end(), exist );
            if ( all && cars.size() == lc.dba_car.size() ) {
                train.dba( temp_dba_train );
                lc.dba_train = temp_dba_train;
            }
        }
    }
    // найти соседние поезда без адресов - это расцеп
    {
        auto zz = [](const Train& one, const Train& two) {
            return 0 == one.dba() && 0 == two.dba();
        };

        auto it = std::adjacent_find( m_trains.begin(), m_trains.end(), zz );
        if ( m_trains.end() != it ) {
            // найден расцеп - сначала старый поезд, а потом новый
            size_t ix = std::distance( m_trains.begin(), it );
            Train& train1 = m_trains[  ix];
            ListCars& lc1 = m_listcars[ix];
            Train& train2 = m_trains[  ix+1];
            ListCars& lc2 = m_listcars[ix+1];
            // вписать адрес старого поезда
            DB_ADDR temp_dba_train;
            CPPUNIT_ASSERT( m_model.in_train( lc1.dba_car.back(), &temp_dba_train) );
            train1.dba( temp_dba_train );
            lc1.dba_train = temp_dba_train;
            // создать адрес нового поезда
            DB_ADDR dba_create = make_train_dba();
            train2.dba( dba_create );
            lc2.dba_train = dba_create;
        }
    }

    // найти одинокий поезд без адреса - это новый поезд
    {
        auto zero = []( const Train& one ) {
            return 0 == one.dba();
        };
        auto kt = std::find_if( m_trains.begin(), m_trains.end(), zero );
        if ( m_trains.end() != kt ) {
            // найден новый поезд без расцепа - инициализация
            size_t ix = std::distance( m_trains.begin(), kt );
            Train& trainN = m_trains[  ix];
            ListCars& lcN = m_listcars[ix];
            // создать адрес нового поезда
            DB_ADDR dba_create = make_train_dba();
            trainN.dba( dba_create );
            lcN.dba_train = dba_create;
        }
    }
}


DB_ADDR Config::make_train_dba() const
{
    DB_ADDR dba_create( 55001 ); // адрес первого поезда
    Train temp;
    while( m_model.info_train( dba_create, &temp ) )
        dba_create = DB_ADDR(dba_create + 1);
    return dba_create;
}

TC_Scoreboard::~TC_Scoreboard()
{}

void TC_Scoreboard::promotion()
{
    size_t idx;
    Car    car;

    Config cfg( *m_model );
    // начальная расстановка
    cfg.build( "|edcba:20>  |     |      |" );
    apply_all( cfg );
    CPPUNIT_ASSERT( 5 == m_blank->element_count() );
    CPPUNIT_ASSERT( !m_blank->pos_measure( &idx, &car ) );
    CPPUNIT_ASSERT( m_blank->pos_remains( &idx, &car ) );
    CPPUNIT_ASSERT( 0 == idx );
    CPPUNIT_ASSERT( car.dba() == carDBA+1 );
    // первый расцеп
    cfg.build( "|edcb:40>a:20> |     |    |  " );
    apply_all( cfg );
    CPPUNIT_ASSERT( 5 == m_blank->element_count() );
    CPPUNIT_ASSERT( !m_blank->pos_measure( &idx, &car ) );
    CPPUNIT_ASSERT( m_blank->pos_remains( &idx, &car ) );
    CPPUNIT_ASSERT( 0 == idx );
    CPPUNIT_ASSERT( car.dba() == carDBA+1 );
    // НЕ начало измерений
    cfg.build( "|edcb:20> | a:31>    |     | " );
    apply_all( cfg );
    CPPUNIT_ASSERT( 5 == m_blank->element_count() );
    CPPUNIT_ASSERT( !m_blank->pos_measure( &idx, &car ) );
    CPPUNIT_ASSERT( car.dba() == carDBA+1 );
    // начало измерений - доехал до 1м измерительного участка
    cfg.build( "|edcb:20> | a:30>    |     | " );
    apply_all( cfg );
    CPPUNIT_ASSERT( 5 == m_blank->element_count() );
    CPPUNIT_ASSERT( m_blank->pos_measure( &idx, &car ) );
    CPPUNIT_ASSERT( 0 == idx );
    CPPUNIT_ASSERT( car.dba() == carDBA+1 );

    CPPUNIT_ASSERT( m_blank->pos_remains( &idx, &car ) );
    CPPUNIT_ASSERT( 1 == idx );
    CPPUNIT_ASSERT( car.dba() == carDBA+2 );
}


void TC_Scoreboard::apply_all( Config &cfg )
{
    while( !cfg.empty() ) {
        ListCars lc = cfg.get_listcars();
        m_model->put( lc );
        Train trCurr = cfg.get_train();
        m_model->put( trCurr );
        m_blank->apply( trCurr, *m_explainMgr );
    }
}

void TC_Scoreboard::setUp()
{
    Scoreboard::Blank::Outside pred = []( const BadgeE& /*bdg*/) {
        return true;
    };

    m_model.reset( new Model() );
    m_chaininfo.reset( new ChainInfo(*m_model) );
    m_blank.reset( new Scoreboard::Blank(*m_model, pred, *m_chaininfo) );
    m_blank->init_entrance( IU[1], IU[2], TOP_WIDTH, 2 );
    m_explainMgr.reset( new ExplainManager );
}

void TC_Scoreboard::tearDown()
{
    m_explainMgr.reset();
    m_blank.reset();
    m_model.reset();
    m_chaininfo.reset();
}
