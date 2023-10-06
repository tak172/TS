#include "stdafx.h"
#include "HemEventRegistry.h"
#include "../Guess/Msg.h"
#include "../Hem/HemEvent.h"

struct HemEventDescr
{
    HemEventDescr( const std::string& xname, const std::wstring& uname ) : xmlName( xname ), userName( uname ) {}
    HemEventDescr() : xmlName(), userName() {}
    std::string xmlName; //атрибут xml-файла
    std::wstring userName; //пользовательское название
};

ALLOCATE_SINGLETON(HemEventRegistry);

HemEventRegistry::HemEventRegistry()
{
    registry[ HCode::FORM ] = HemEventDescr( form_xAttr, trx_noop( L"Тип события", L"Возникновение" ) );
    registry[ HCode::EXPLICIT_FORM ] = HemEventDescr( explicit_form_xAttr, trx_noop( L"Тип события", L"Формирование" ) );
    registry[ HCode::DISFORM ] = HemEventDescr( disform_xAttr, trx_noop( L"Тип события", L"Расформирование" ) );
    registry[ HCode::DEATH ] = HemEventDescr( death_xAttr, trx_noop( L"Тип события", L"Завершение" ) );
    registry[ HCode::ARRIVAL ] = HemEventDescr( arrival_xAttr, trx_noop( L"Тип события", L"Прибытие" ) );
    registry[ HCode::DEPARTURE ] = HemEventDescr( departure_xAttr, trx_noop( L"Тип события", L"Отправление" ) );
    registry[ HCode::TRANSITION ] = HemEventDescr( transition_xAttr, trx_noop( L"Тип события", L"Проследование" ) );
    registry[ HCode::SPAN_MOVE ] = HemEventDescr( span_move_xAttr, trx_noop( L"Тип события", L"Движение по перегону" ) );
    registry[ HCode::INFO_CHANGING ] = HemEventDescr( info_changing_xAttr, trx_noop( L"Тип события", L"Изменение информации" ) );
    registry[ HCode::ENTRANCE_STOPPING ] = HemEventDescr( entra_stopping_xAttr, trx_noop( L"Тип события", L"Стоянка перед входным" ) );
    registry[ HCode::SPAN_STOPPING_BEGIN ] = HemEventDescr( span_stopping_begin_xAttr, trx_noop( L"Тип события", L"Начало стоянки на перегоне" ) );
    registry[ HCode::SPAN_STOPPING_END ] = HemEventDescr( span_stopping_end_xAttr, trx_noop( L"Тип события", L"Конец стоянки на перегоне" ) );
    registry[ HCode::WRONG_SPANWAY ] = HemEventDescr( wrongway_xAttr, trx_noop( L"Тип события", L"Движение по неправильному пути перегона" ) );
    registry[ HCode::TOKEN_SPANWAY ] = HemEventDescr( tokenway_xAttr, trx_noop( L"Тип события", L"Сведения о пути перегона" ) );
    registry[ HCode::POCKET_ENTRY ] = HemEventDescr( pockent_xAttr, trx_noop( L"Тип события", L"Вход в \"карман\"" ) );
    registry[ HCode::POCKET_EXIT ] = HemEventDescr( pockex_xAttr, trx_noop( L"Тип события", L"Выход из \"кармана\"" ) );
    registry[ HCode::STATION_ENTRY ] = HemEventDescr( statent_xAttr, trx_noop( L"Тип события", L"Вход на станции" ) );
    registry[ HCode::BANNED_ENTRY ] = HemEventDescr( bannedent_xAttr, trx_noop( L"Тип события", L"Вход на станцию под запрещающий сигнал" ) );
    registry[ HCode::STATION_EXIT ] = HemEventDescr( statex_xAttr, trx_noop( L"Тип события", L"Выход со станции" ) );
    registry[ HCode::THROW ] = HemEventDescr( throw_xAttr, trx_noop( L"Тип события", L"Бросание" ) );
    registry[ HCode::RAISE ] = HemEventDescr( raise_xAttr, trx_noop( L"Тип события", L"Подъем" ) );
    registry[ HCode::WAY_RED ] = HemEventDescr( way_occupy_xAttr, trx_noop( L"Тип события", L"Занятие ПО пути" ) );
    registry[ HCode::WAY_BLACK ] = HemEventDescr( way_empty_xAttr, trx_noop( L"Тип события", L"Освобождение ПО пути" ) );
    registry[ HCode::CUTOFF ] = HemEventDescr( cutoff_xAttr, trx_noop( L"Тип события", L"Потеря связи со станцией" ) );
    registry[ HCode::CUTIN ] = HemEventDescr( cutin_xAttr, trx_noop( L"Тип события", L"Возобновление связи со станцией" ) );
    registry[ HCode::ASOUP_UNKNOWN ] = HemEventDescr( asoupUnknown_xAttr, trx_noop( L"Тип события", L"Сообщение АСОУП (неизв.)" ) );
	CreateSequenceTable();
}

HemEventRegistry::~HemEventRegistry()
{
}

void HemEventRegistry::CreateSequenceTable()
{
    auto at = [this]( HCode one, HCode two )->Allowably&
    {
        auto it = seqtable.insert( std::make_pair( SequenceKey( one, two ), ALW_NONE ) );
        ASSERT( true == it.second ); // 
        return it.first->second;
    };

	//событий расформирования нет.
	//завершение
    at( HCode::DEATH, HCode::FORM         ) = ALW_ALL;
    at( HCode::DEATH, HCode::ARRIVAL      ) = ALW_ALL; //ALW_USERCHART ?
    at( HCode::DEATH, HCode::DEPARTURE    ) = ALW_ALL; //ALW_USERCHART ?
    at( HCode::DEATH, HCode::TRANSITION   ) = ALW_ALL; //ALW_USERCHART ?
    at( HCode::DEATH, HCode::SPAN_MOVE    ) = ALW_ALL; //ALW_USERCHART ?
    at( HCode::DEATH, HCode::INFO_CHANGING) = ALW_ALL; //ALW_USERCHART ?
    at( HCode::DEATH, HCode::WRONG_SPANWAY ) = ALW_ALL; //ALW_USERCHART ? ????
    at( HCode::DEATH, HCode::TOKEN_SPANWAY ) = ALW_ALL; //ALW_USERCHART ? ????
    at( HCode::DEATH, HCode::POCKET_EXIT  ) = ALW_ALL; //ALW_USERCHART ?
    at( HCode::DEATH, HCode::STATION_ENTRY ) = ALW_ALL; //ALW_USERCHART ?
    at( HCode::DEATH, HCode::BANNED_ENTRY ) = ALW_ALL; //ALW_USERCHART ?
    at( HCode::DEATH, HCode::STATION_EXIT ) = ALW_ALL; //ALW_USERCHART ?
	//появление
	at( HCode::FORM, HCode::EXPLICIT_FORM		) = ALW_ALL;
    at( HCode::FORM, HCode::DISFORM				) = ALW_ALL;
    at( HCode::FORM, HCode::DEATH				) = ALW_ALL;
    at( HCode::FORM, HCode::ARRIVAL				) = ALW_ALL;
    at( HCode::FORM, HCode::DEPARTURE			) = ALW_ALL;
    at( HCode::FORM, HCode::TRANSITION			) = ALW_ALL;
    at( HCode::FORM, HCode::INFO_CHANGING		) = ALW_ALL;
	at( HCode::FORM, HCode::SPAN_STOPPING_BEGIN	) = ALW_ALL;
	at( HCode::FORM, HCode::SPAN_STOPPING_END	) = ALW_ALL; //если поезд встал на перегоне, а затем исчез и восстановился
    at( HCode::FORM, HCode::SPAN_MOVE			) = ALW_ALL;
    at( HCode::FORM, HCode::POCKET_ENTRY		) = ALW_ALL;
    at( HCode::FORM, HCode::ENTRANCE_STOPPING	) = ALW_ALL;
    at( HCode::FORM, HCode::STATION_ENTRY ) = ALW_ALL;
    at( HCode::FORM, HCode::BANNED_ENTRY ) = ALW_ALL;
    at( HCode::FORM, HCode::STATION_EXIT		) = ALW_ALL;
    at( HCode::FORM, HCode::WRONG_SPANWAY ) = ALW_ALL; // т.к. поезд может появиться на перегоне
    at( HCode::FORM, HCode::TOKEN_SPANWAY ) = ALW_ALL; // т.к. поезд может появиться на перегоне
    //формирование
	at( HCode::EXPLICIT_FORM, HCode::DISFORM ) = ALW_ALL;
	at( HCode::EXPLICIT_FORM, HCode::DEATH ) = ALW_ALL;
	at( HCode::EXPLICIT_FORM, HCode::DEPARTURE ) = ALW_ALL;
	at( HCode::EXPLICIT_FORM, HCode::INFO_CHANGING ) = ALW_ALL;
    at( HCode::EXPLICIT_FORM, HCode::WRONG_SPANWAY ) = ALW_ALL;
    at( HCode::EXPLICIT_FORM, HCode::TOKEN_SPANWAY ) = ALW_ALL;
    at( HCode::EXPLICIT_FORM, HCode::STATION_EXIT ) = ALW_ALL;
	at( HCode::EXPLICIT_FORM, HCode::THROW ) = ALW_ALL;
	//прибытие
	at( HCode::ARRIVAL, HCode::EXPLICIT_FORM       ) = ALW_ALL;
    at( HCode::ARRIVAL, HCode::DISFORM       ) = ALW_ALL;
    at( HCode::ARRIVAL, HCode::DEATH         ) = ALW_ALL;
    at( HCode::ARRIVAL, HCode::DEPARTURE     ) = ALW_ALL;
    at( HCode::ARRIVAL, HCode::INFO_CHANGING ) = ALW_ALL;
	at( HCode::ARRIVAL, HCode::THROW ) = ALW_ALL;
	//отправление
    at( HCode::DEPARTURE, HCode::DISFORM       ) = ALW_ALL;
    at( HCode::DEPARTURE, HCode::DEATH         ) = ALW_ALL;
    at( HCode::DEPARTURE, HCode::ARRIVAL       ) = ALW_ALL;
    at( HCode::DEPARTURE, HCode::TRANSITION    ) = ALW_ALL;
    at( HCode::DEPARTURE, HCode::INFO_CHANGING ) = ALW_ALL;
    at( HCode::DEPARTURE, HCode::STATION_EXIT  ) = ALW_ALL;
    at( HCode::DEPARTURE, HCode::SPAN_MOVE         ) = ALW_USERCHART;
    at( HCode::DEPARTURE, HCode::STATION_ENTRY ) = ALW_USERCHART;
    at( HCode::DEPARTURE, HCode::BANNED_ENTRY ) = ALW_USERCHART;
    at( HCode::DEPARTURE, HCode::ENTRANCE_STOPPING ) = ALW_USERCHART;
    at( HCode::DEPARTURE, HCode::SPAN_STOPPING_BEGIN)= ALW_USERCHART;
    at( HCode::DEPARTURE, HCode::WRONG_SPANWAY ) = ALW_USERCHART;
    at( HCode::DEPARTURE, HCode::TOKEN_SPANWAY ) = ALW_USERCHART;
    //проследование
    at (HCode::TRANSITION, HCode::DISFORM       ) = ALW_ALL;
    at( HCode::TRANSITION, HCode::DEATH         ) = ALW_ALL;
    at( HCode::TRANSITION, HCode::ARRIVAL       ) = ALW_ALL;
    at( HCode::TRANSITION, HCode::TRANSITION    ) = ALW_ALL;
    at( HCode::TRANSITION, HCode::INFO_CHANGING ) = ALW_ALL;
    at( HCode::TRANSITION, HCode::STATION_EXIT  ) = ALW_ALL;
    at( HCode::TRANSITION, HCode::SPAN_MOVE         ) = ALW_USERCHART;
    at( HCode::TRANSITION, HCode::ENTRANCE_STOPPING ) = ALW_USERCHART;
    at( HCode::TRANSITION, HCode::STATION_ENTRY ) = ALW_USERCHART;
    at( HCode::TRANSITION, HCode::BANNED_ENTRY ) = ALW_USERCHART;
    at( HCode::TRANSITION, HCode::SPAN_STOPPING_BEGIN)= ALW_USERCHART;
    at( HCode::TRANSITION, HCode::WRONG_SPANWAY ) = ALW_USERCHART;
    at( HCode::TRANSITION, HCode::TOKEN_SPANWAY ) = ALW_USERCHART;
    //движение по перегону
    at( HCode::SPAN_MOVE, HCode::DISFORM			) = ALW_ALL;
    at( HCode::SPAN_MOVE, HCode::DEATH				) = ALW_ALL;
    at( HCode::SPAN_MOVE, HCode::ARRIVAL			) = ALW_ALL; //ALW_USERCHART ?
    at( HCode::SPAN_MOVE, HCode::TRANSITION			) = ALW_ALL; //ALW_USERCHART ?
    at( HCode::SPAN_MOVE, HCode::SPAN_MOVE			) = ALW_ALL;
    at( HCode::SPAN_MOVE, HCode::INFO_CHANGING		) = ALW_ALL;
	at( HCode::SPAN_MOVE, HCode::SPAN_STOPPING_BEGIN) = ALW_ALL;
    at( HCode::SPAN_MOVE, HCode::POCKET_ENTRY		) = ALW_ALL;
    at( HCode::SPAN_MOVE, HCode::ENTRANCE_STOPPING	) = ALW_ALL;
    at( HCode::SPAN_MOVE, HCode::STATION_ENTRY ) = ALW_ALL;
    at( HCode::SPAN_MOVE, HCode::BANNED_ENTRY ) = ALW_ALL;
    //изменение базовой информации
	at( HCode::INFO_CHANGING, HCode::EXPLICIT_FORM		) = ALW_ALL;
    at( HCode::INFO_CHANGING, HCode::DEATH				) = ALW_ALL;
    at( HCode::INFO_CHANGING, HCode::ARRIVAL			) = ALW_ALL;
    at( HCode::INFO_CHANGING, HCode::DEPARTURE			) = ALW_ALL;
    at( HCode::INFO_CHANGING, HCode::TRANSITION			) = ALW_ALL;
    at( HCode::INFO_CHANGING, HCode::SPAN_MOVE			) = ALW_ALL;
    at( HCode::INFO_CHANGING, HCode::INFO_CHANGING		) = ALW_ALL;
	at( HCode::INFO_CHANGING, HCode::SPAN_STOPPING_BEGIN) = ALW_ALL;
	at( HCode::INFO_CHANGING, HCode::SPAN_STOPPING_END	) = ALW_ALL;
    at( HCode::INFO_CHANGING, HCode::POCKET_ENTRY		) = ALW_ALL;
    at( HCode::INFO_CHANGING, HCode::POCKET_EXIT		) = ALW_ALL;
    at( HCode::INFO_CHANGING, HCode::ENTRANCE_STOPPING	) = ALW_ALL;
    at( HCode::INFO_CHANGING, HCode::STATION_ENTRY ) = ALW_ALL;
    at( HCode::INFO_CHANGING, HCode::BANNED_ENTRY ) = ALW_ALL;
    at( HCode::INFO_CHANGING, HCode::STATION_EXIT		) = ALW_ALL;
	at( HCode::INFO_CHANGING, HCode::THROW				) = ALW_ALL;
	//вход в "карман"
    at( HCode::POCKET_ENTRY, HCode::DEATH				) = ALW_ALL;
    at( HCode::POCKET_ENTRY, HCode::INFO_CHANGING		) = ALW_ALL; //изменение информации для поезда в "кармане" возможно при уступке номера/индекса
	at( HCode::POCKET_ENTRY, HCode::SPAN_STOPPING_BEGIN ) = ALW_ALL;
    at( HCode::POCKET_ENTRY, HCode::POCKET_EXIT			) = ALW_ALL;
	//выход из "кармана"
    at( HCode::POCKET_EXIT, HCode::DEATH				) = ALW_ALL;
    at( HCode::POCKET_EXIT, HCode::POCKET_ENTRY			) = ALW_ALL;
    at( HCode::POCKET_EXIT, HCode::SPAN_MOVE			) = ALW_ALL;
    at( HCode::POCKET_EXIT, HCode::INFO_CHANGING		) = ALW_ALL;
	at( HCode::POCKET_EXIT, HCode::SPAN_STOPPING_BEGIN	) = ALW_ALL;
    at( HCode::POCKET_EXIT, HCode::ENTRANCE_STOPPING	) = ALW_ALL;
	//вход на станцию
    at( HCode::STATION_ENTRY, HCode::DEATH         ) = ALW_ALL;
    at( HCode::STATION_ENTRY, HCode::ARRIVAL       ) = ALW_ALL;
    at( HCode::STATION_ENTRY, HCode::DEPARTURE     ) = ALW_ALL;
    at( HCode::STATION_ENTRY, HCode::TRANSITION    ) = ALW_ALL;
    at( HCode::STATION_ENTRY, HCode::INFO_CHANGING ) = ALW_ALL;
    at( HCode::STATION_ENTRY, HCode::STATION_EXIT  ) = ALW_ALL;
    at( HCode::STATION_ENTRY, HCode::DISFORM       ) = ALW_ALL;
    //вход под запрещающий на станцию
    at( HCode::BANNED_ENTRY, HCode::DEATH ) = ALW_ALL;
    at( HCode::BANNED_ENTRY, HCode::ARRIVAL ) = ALW_ALL;
    at( HCode::BANNED_ENTRY, HCode::DEPARTURE ) = ALW_ALL;
    at( HCode::BANNED_ENTRY, HCode::TRANSITION ) = ALW_ALL;
    at( HCode::BANNED_ENTRY, HCode::INFO_CHANGING ) = ALW_ALL;
    at( HCode::BANNED_ENTRY, HCode::STATION_EXIT ) = ALW_ALL;
    at( HCode::BANNED_ENTRY, HCode::DISFORM ) = ALW_ALL;
    //выход со станции
    at( HCode::STATION_EXIT, HCode::DEATH               ) = ALW_ALL;
    at( HCode::STATION_EXIT, HCode::INFO_CHANGING       ) = ALW_ALL;
    at( HCode::STATION_EXIT, HCode::SPAN_MOVE           ) = ALW_ALL;
    at( HCode::STATION_EXIT, HCode::WRONG_SPANWAY ) = ALW_ALL;
    at( HCode::STATION_EXIT, HCode::TOKEN_SPANWAY ) = ALW_ALL;
    at( HCode::STATION_EXIT, HCode::STATION_ENTRY ) = ALW_ALL;
    at( HCode::STATION_EXIT, HCode::BANNED_ENTRY ) = ALW_ALL;
    at( HCode::STATION_EXIT, HCode::SPAN_STOPPING_BEGIN ) = ALW_ALL;
    at( HCode::STATION_EXIT, HCode::ENTRANCE_STOPPING ) = ALW_USERCHART;
    at( HCode::STATION_EXIT, HCode::ARRIVAL           ) = ALW_USERCHART;
    at( HCode::STATION_EXIT, HCode::TRANSITION        ) = ALW_USERCHART;
	//стоянка перед входным
    at( HCode::ENTRANCE_STOPPING, HCode::DISFORM			) = ALW_ALL;
    at( HCode::ENTRANCE_STOPPING, HCode::DEATH				) = ALW_ALL;
    at( HCode::ENTRANCE_STOPPING, HCode::ARRIVAL			) = ALW_ALL;
    at( HCode::ENTRANCE_STOPPING, HCode::DEPARTURE			) = ALW_ALL;
    at( HCode::ENTRANCE_STOPPING, HCode::TRANSITION			) = ALW_ALL;
    at( HCode::ENTRANCE_STOPPING, HCode::INFO_CHANGING		) = ALW_ALL;
    at( HCode::ENTRANCE_STOPPING, HCode::STATION_ENTRY ) = ALW_ALL;
    at( HCode::ENTRANCE_STOPPING, HCode::BANNED_ENTRY ) = ALW_ALL;
    at( HCode::ENTRANCE_STOPPING, HCode::SPAN_MOVE			) = ALW_ALL;
	//начало стоянки на перегоне
	at( HCode::SPAN_STOPPING_BEGIN, HCode::DISFORM				) = ALW_ALL;
	at( HCode::SPAN_STOPPING_BEGIN, HCode::DEATH				) = ALW_ALL;
	at( HCode::SPAN_STOPPING_BEGIN, HCode::INFO_CHANGING		) = ALW_ALL;
	at( HCode::SPAN_STOPPING_BEGIN, HCode::SPAN_STOPPING_END	) = ALW_ALL;
	//конец стоянки на перегоне
	at( HCode::SPAN_STOPPING_END, HCode::DISFORM				) = ALW_ALL;
	at( HCode::SPAN_STOPPING_END, HCode::DEATH					) = ALW_ALL;
	at( HCode::SPAN_STOPPING_END, HCode::ARRIVAL				) = ALW_USERCHART;
	at( HCode::SPAN_STOPPING_END, HCode::TRANSITION				) = ALW_USERCHART;
	at( HCode::SPAN_STOPPING_END, HCode::SPAN_MOVE				) = ALW_ALL;
	at( HCode::SPAN_STOPPING_END, HCode::INFO_CHANGING			) = ALW_ALL;
	at( HCode::SPAN_STOPPING_END, HCode::POCKET_ENTRY			) = ALW_ALL;
	at( HCode::SPAN_STOPPING_END, HCode::POCKET_EXIT			) = ALW_ALL; //стоянка на перегоне возможна как вне кармана, так и внутри него
    at( HCode::SPAN_STOPPING_END, HCode::STATION_ENTRY ) = ALW_ALL;
    at( HCode::SPAN_STOPPING_END, HCode::BANNED_ENTRY ) = ALW_ALL;
    at( HCode::SPAN_STOPPING_END, HCode::SPAN_STOPPING_BEGIN	) = ALW_USERCHART; // допускается подряд несколько стоянок на перегоне
    at( HCode::SPAN_STOPPING_END, HCode::ENTRANCE_STOPPING	    ) = ALW_USERCHART; // допускается подряд стоянка на перегоне и стоянка перед входным
	//выход на неправильный перегон
    at( HCode::WRONG_SPANWAY, HCode::DISFORM           ) = ALW_ALL;
    at( HCode::WRONG_SPANWAY, HCode::DEATH             ) = ALW_ALL;
    at( HCode::WRONG_SPANWAY, HCode::SPAN_MOVE         ) = ALW_ALL;
    at( HCode::WRONG_SPANWAY, HCode::INFO_CHANGING     ) = ALW_ALL;
    at( HCode::WRONG_SPANWAY, HCode::ENTRANCE_STOPPING ) = ALW_ALL;
    at( HCode::WRONG_SPANWAY, HCode::ARRIVAL           ) = ALW_USERCHART;
    at( HCode::WRONG_SPANWAY, HCode::TRANSITION		   ) = ALW_USERCHART;
    at( HCode::WRONG_SPANWAY, HCode::SPAN_STOPPING_BEGIN ) = ALW_USERCHART;
    at( HCode::WRONG_SPANWAY, HCode::STATION_ENTRY ) = ALW_USERCHART;
    at( HCode::WRONG_SPANWAY, HCode::BANNED_ENTRY ) = ALW_USERCHART;
    //выход на неправильный перегон
    at( HCode::TOKEN_SPANWAY, HCode::DISFORM ) = ALW_ALL;
    at( HCode::TOKEN_SPANWAY, HCode::DEATH ) = ALW_ALL;
    at( HCode::TOKEN_SPANWAY, HCode::SPAN_MOVE ) = ALW_ALL;
    at( HCode::TOKEN_SPANWAY, HCode::INFO_CHANGING ) = ALW_ALL;
    at( HCode::TOKEN_SPANWAY, HCode::ENTRANCE_STOPPING ) = ALW_ALL;
    at( HCode::TOKEN_SPANWAY, HCode::ARRIVAL ) = ALW_USERCHART;
    at( HCode::TOKEN_SPANWAY, HCode::TRANSITION ) = ALW_USERCHART;
    at( HCode::TOKEN_SPANWAY, HCode::SPAN_STOPPING_BEGIN ) = ALW_USERCHART;
    at( HCode::TOKEN_SPANWAY, HCode::STATION_ENTRY ) = ALW_USERCHART;
    at( HCode::TOKEN_SPANWAY, HCode::BANNED_ENTRY ) = ALW_USERCHART;
    // бросание
	at( HCode::THROW, HCode::DISFORM ) = ALW_ALL;
	at( HCode::THROW, HCode::DEATH ) = ALW_ALL;
	at( HCode::THROW, HCode::RAISE ) = ALW_ALL;
	//поднятие
	at( HCode::RAISE, HCode::DISFORM ) = ALW_ALL;
	at( HCode::RAISE, HCode::DEATH ) = ALW_ALL;
	at( HCode::RAISE, HCode::DEPARTURE ) = ALW_ALL;
	at( HCode::RAISE, HCode::INFO_CHANGING ) = ALW_ALL;
    at( HCode::RAISE, HCode::WRONG_SPANWAY ) = ALW_ALL;
    at( HCode::RAISE, HCode::TOKEN_SPANWAY ) = ALW_ALL;
    at( HCode::RAISE, HCode::STATION_EXIT ) = ALW_ALL;
	at( HCode::RAISE, HCode::THROW ) = ALW_ALL;
	// занятие ПО-пути - потом можно только освободить
    at( HCode::WAY_RED, HCode::WAY_BLACK ) = ALW_ALL;
    // освобождение ПО-пути - потом ничего нельзя
}

bool HemEventRegistry::CanBeStarted( HCode new_code ) const
{
    switch ( new_code )
    {
    case HCode::DEATH:
    case HCode::DISFORM:
    case HCode::SPAN_STOPPING_END:
    case HCode::WAY_BLACK:
    case HCode::INFO_CHANGING:
        return false;
    default:
        return true;
    }
}

bool HemEventRegistry::CanBeFollowed( HCode last_code, HCode new_code ) const
{
	auto cit = seqtable.find( SequenceKey(last_code,new_code) );
    bool res = seqtable.end() != cit && cit->second != ALW_NONE; // \todo проверить конкретные разрешения
    return res;
}

bool HemEventRegistry::CodeIsValid( HCode eCode ) const
{
	return registry.find( eCode ) != registry.end();
}

std::shared_ptr<HCode> HemEventRegistry::GetEventCode( const std::string& xml_name ) const
{
	std::shared_ptr<HCode> hemCodePtr;
	auto cit = find_if( registry.begin(), registry.end(), [&xml_name]( const std::pair <HCode, HemEventDescr> & cdPair ){
		return xml_name == cdPair.second.xmlName;
	} );
	if ( cit != registry.end() )
		hemCodePtr = std::make_shared<HCode>( cit->first );
	return hemCodePtr;
}

bool HemEventRegistry::GetEventCodeEx( const char* xml_name, HCode& code ) const
{
    auto x2c = [this](){
        std::map< std::string, HCode > xml2code;
        for( auto r : registry )
            xml2code.insert( std::make_pair(r.second.xmlName,r.first) );
        return xml2code;
    };
    static const auto xml_to_code = x2c();
    auto cit = xml_to_code.find( xml_name );
    bool found = ( cit != xml_to_code.end() );
    if ( found )
        code = cit->second;
    return found;
}

const std::string& HemEventRegistry::GetXMLName( HCode eCode ) const
{
	auto cit = registry.find( eCode );
	if ( cit != registry.end() )
		return cit->second.xmlName;

    static const std::string xmlNameNotFound = "";
	return xmlNameNotFound;
}

std::wstring HemEventRegistry::getUserName( HCode eCode ) const
{
    auto cit = registry.find( eCode );
    return ( cit != registry.end() ) ?  cit->second.userName : L"";
}

bool HemEventRegistry::MustBeOnSpan( HCode code )
{
    return ( 
        code == HCode::ENTRANCE_STOPPING ||
        code == HCode::POCKET_ENTRY ||
        code == HCode::POCKET_EXIT ||
        code == HCode::SPAN_MOVE ||
        code == HCode::SPAN_STOPPING_BEGIN ||
        code == HCode::SPAN_STOPPING_END ||
        code == HCode::WRONG_SPANWAY ||
        code == HCode::TOKEN_SPANWAY
        );
}

bool HemEventRegistry::MustBeOnStation( HCode code )
{
    // проверка на события, которые должны быть только на станции
    return ( 
        code == HCode::ARRIVAL ||
        code == HCode::DEPARTURE ||
        code == HCode::TRANSITION ||
        code == HCode::EXPLICIT_FORM ||
        code == HCode::WAY_BLACK ||
        code == HCode::WAY_RED ||
        code == HCode::STATION_ENTRY ||
        code == HCode::BANNED_ENTRY ||
        code == HCode::STATION_EXIT
        );
}

bool HemEventRegistry::IsSpecialClosedCode( HCode code )
{
    return code == HCode::DEATH || code == HCode::DISFORM;
};

bool HemEventRegistry::isDepartureCode( const HCode code)
{
	return HCode::TRANSITION == code || HCode::DEPARTURE == code;
}

bool HemEventRegistry::isArrivalCode( const HCode code)
{
	return HCode::TRANSITION == code || HCode::ARRIVAL == code;
}

bool HemEventRegistry::isFormCode( const HCode code )
{
	return HCode::FORM == code || HCode::EXPLICIT_FORM == code;
}
