#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/EsrGuide.h"
#include "../Fund/GidUraler.h"
#include "../Fund/Lexicon.h"
/* 
 * Проверка работы класса
 */

class TC_Foreign : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_Foreign );
    CPPUNIT_TEST( detectSamara );
    CPPUNIT_TEST( detectNovosib );
    CPPUNIT_TEST( detectNordRW );
    CPPUNIT_TEST( detectBisk );
    CPPUNIT_TEST( detectNordRW_GU );
    CPPUNIT_TEST( biskWarn );
    CPPUNIT_TEST( biskExpireWarn );
    CPPUNIT_TEST( biskWarn_SingleWaySpan );
    CPPUNIT_TEST( biskWarn_SingleWayFullSpan );
    CPPUNIT_TEST( biskWarn_VerbalOddSpanWay );
    CPPUNIT_TEST( biskWarn_VerbalEvenSpanWay );
    CPPUNIT_TEST( biskWarn_checkTmpSk );
    CPPUNIT_TEST( biskWarn_Tornakalns_Olaine_nepara );
    CPPUNIT_TEST( biskWarn_Tornakalns_Olaine_para );
    CPPUNIT_TEST( biskWarn_Saulkrasti_Lilaste );
    CPPUNIT_TEST( biskWarn_Tornakalns_Olaine_putB );
    CPPUNIT_TEST( biskWarn_Tornakalns_Olaine_ticket7185 );
    CPPUNIT_TEST( biskWarn_Tornakalns_switch );
    CPPUNIT_TEST( readNordRW_GU_text );
    CPPUNIT_TEST( readNordRW_Zip );
    CPPUNIT_TEST( orderNordRW_Zip );
    CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp() override;
    virtual void tearDown() override;

protected:
    void detectSamara();
    void detectNovosib();
    void detectNordRW();
    void detectBisk();
    void detectNordRW_GU();
    void biskWarn();
    void biskExpireWarn();
    void biskWarn_SingleWaySpan();
    void biskWarn_SingleWayFullSpan();
    void biskWarn_VerbalOddSpanWay();
    void biskWarn_VerbalEvenSpanWay();
    void biskWarn_checkTmpSk();
    void biskWarn_Tornakalns_Olaine_nepara(); // пред. нечетный путь около Олайне
    void biskWarn_Tornakalns_Olaine_para(); // пред. четный путь около Торнякалнса
    void biskWarn_Saulkrasti_Lilaste(); // пред. Saulkrasti и Lilaste (блок-пост Incupe)
    void biskWarn_Tornakalns_Olaine_putB(); // путь Б Торнякалнс-Олайне
    void biskWarn_Tornakalns_Olaine_ticket7185(); // предупреждение оказалось на самом блок-посту KIG-1629 id=5829
    void biskWarn_Tornakalns_switch();  // стрелка Торнякалнс
    void readNordRW_GU_text(); // разбор текстового расписания Гид-Урала
    void readNordRW_Zip(); // разбор ZIP файлов от ГИд-Урала
    void orderNordRW_Zip(); // восстановление при нарушении порядка ZIP файлов от ГИд-Урала

private:
    static const time_t wallClock = time_t(1234567);
    GIDURALER gUraler;
    EsrGuide esrGuide;
    Lexicon lex;
};
