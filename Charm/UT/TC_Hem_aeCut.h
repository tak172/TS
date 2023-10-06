#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/EsrGuide.h"
#include "../Hem/HemHelpful.h"
#include "../Hem/HappenLayer.h"

class TC_Hem_aeCut : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeCut );
    CPPUNIT_TEST( Middle );
    CPPUNIT_TEST( WithStart );
    CPPUNIT_TEST( WithEnd );
    CPPUNIT_TEST( Whole );
    CPPUNIT_TEST( TooShort );
    CPPUNIT_TEST( SaveNumber );
    CPPUNIT_TEST( Case3674 );
    CPPUNIT_TEST( StoppingBegin_Death_3889 );
    CPPUNIT_TEST( Ticket3912_split );
    CPPUNIT_TEST( Ticket3912_toEnd );
    CPPUNIT_TEST( CutPathBeginningUntilDeparture );
    CPPUNIT_TEST( CutPathBeginningUntilTransition );
    CPPUNIT_TEST( WorkaroundCutPathStartingDeparture );
    CPPUNIT_TEST( CutPathTailStationSpan );
    CPPUNIT_TEST( StoppingBegin_End_WrongWay_3889 );
    CPPUNIT_TEST( RemoveEndOfPath );
    CPPUNIT_TEST( RemoveBeginOfPath );
	CPPUNIT_TEST_SUITE_END();

public:
    void tearDown() override;

private:
	void Middle(); // разрыв в середине нити
    void WithStart(); // удаление первой половины нити
    void WithEnd(); // удаление второй половины нити
    void Whole(); // удаление второй половины нити
    void TooShort(); // удаление очень короткой нити Form-Death
    void SaveNumber(); // сохранение номера поезда после разрыва
    void Case3674(); // реальная ошибка #3674
    void StoppingBegin_Death_3889(); // разрыв неполнойя стоянки на перегоне
    void StoppingBegin_End_WrongWay_3889(); //разрыв полной стоянки на перегоне, на неправильном пути
    void Ticket3912_split(); // разрыв стоянки на перегоне
    void Ticket3912_toEnd(); // удаление от начала стоянки на перегоне до конца нити
    void CutPathBeginningUntilDeparture();
    void CutPathBeginningUntilTransition();
    void WorkaroundCutPathStartingDeparture(); // "выделение" начала, если нить начинается с отправления
    void CutPathTailStationSpan();          // разрыв хвоста нити от станционного события до SpanMove
    void RemoveEndOfPath();                 // удаление хвоста нити
    void RemoveBeginOfPath();               // удаление начала нити

    void collectUndo( HappenLayer& hl );
    void implementUndo( HappenLayer& hl );

    static const time_t T=3600*(240+23);
    attic::a_document            undoer;
    EsrGuide esrGuide;

    std::shared_ptr<const SpotEvent> getSpotPtr(HappenLayer& happenLayer, const SpotEvent& spot);
    std::shared_ptr<Hem::HappenPath> getPathPtr(HappenLayer& happenLayer, const SpotEvent& spot);
};
