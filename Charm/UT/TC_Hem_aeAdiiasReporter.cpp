#include "stdafx.h"
#include <regex>
#include "TC_Hem_aeAdiiasReporter.h"
#include "UtHemHelper.h"
#include "../Hem/Appliers/aeAdiiasReporter.h"
#include "../Hem/UserChart.h"
#include "../helpful/LocoCharacteristics.h"
#include "../helpful/StrToTime.h"

#define USE_CRR_REPORTING   0
#define USE_DIESEL_TRAIN    1
#define USE_ELECTRIC_TRAIN  1

namespace bt = boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeAdiiasReporter );

void TC_Hem_aeAdiiasReporter::setUp()
{
    m_esrGuide.reset( new EsrGuide );
    m_carrier.reset( new Carrier );
    m_happenLayer.reset( new UtLayer<HappenLayer> );
    m_asoupLayer.reset( new UtLayer<AsoupLayer> );
    setUp_EsrGuide();
    setUp_Carrier();
}

void TC_Hem_aeAdiiasReporter::setUp_Carrier()
{
    std::wstring text = 
        L"<Carrier>"
        L"<Company carrier_code='01' name='ЛДЗ ( УПРАВЛЕНИЕ ГРУЗОВЫХ ПЕРЕВОЗОК )' nameLv='LDZ ( KRAVU PARVADAJUMU PARVALDE )' />"
        L"<Company carrier_code='01' name='ЛДЗ ( УПРАВЛЕНИЕ ГРУЗОВЫХ ПЕРЕВОЗОК )' nameLv='LDZ ( KRAVU PARVADAJUMU PARVALDE )' />"
        L"<Company carrier_code='02' name='БАЛТИЯС ЭКСПРЕСИС АО' nameLv='BALTIJAS EKSPRESIS AS' />"
        L"<Company carrier_code='03' name='БАЛТИЯС ТРАНЗИТА СЕРВИСС АО' nameLv='BALTIJAS TRANZITA SERVISS AS' />"
        L"<Locomotive carrier_code='02' ser='680' number='295' railway='09' />"
        L"<Locomotive carrier_code='02' ser='680' number='297' railway='09' />"
        L"<Locomotive carrier_code='02' ser='680' number='318' railway='09' />"
        L"</Carrier>";
    attic::a_document doc;
    doc.load_utf8( ToUtf8(text) );
    m_carrier->deserialize( doc.document_element() );
}

void TC_Hem_aeAdiiasReporter::setUp_EsrGuide()
{
    std::wstring text = 
        L"<EsrGuide>"
        L"<EsrGuide kit='06610' name='Sebeža' picketing1_val='306~200' picketing1_comm='Рига-Зилупе' picketing2_val='616~800' picketing2_comm='Себеж' />"
        L"<EsrGuide kit='08620' name='Valga' picketing1_val='166~100' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='09000' name=' Skirotava-A' picketing1_val='8~54' picketing1_comm='Рига Пассажирская' picketing2_val='8~54' picketing2_comm='Рига-Зилупе' picketing3_val='8~54' picketing3_comm='Шкиротава-А'/>"
        L"<EsrGuide kit='09006' name=' Šķirotava-J' picketing1_val='4~500' picketing1_comm='Земитани-Шкиротава-Я' picketing2_val='5~711' picketing2_comm='Рига Пассажирская' picketing3_val='5~711' picketing3_comm='Рига-Зилупе' picketing4_val='5~711' picketing4_comm='Шкиротава-Я' />"
        L"<EsrGuide kit='09008' name=' Šķirotava-C' picketing1_val='6~610' picketing1_comm='Рига-Зилупе' />"
        L"<EsrGuide kit='09180' name='Jelgava' picketing1_val='164~100' picketing1_comm='Вентспилс' picketing2_val='42~954' picketing2_comm='Литва' picketing3_val='42~954' picketing3_comm='Мейтене' picketing4_val='42~954' picketing4_comm='Реньге' picketing5_val='42~954' picketing5_comm='Рига'/>"
        L"<EsrGuide kit='09181' name='Jelgava 2' picketing1_val='166~500' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09190' name='Daugava' picketing1_val='291~' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09191' name='Selpils' picketing1_val='284~200' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09193' name='Sece' picketing1_val='268~700' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09200' name='Daudzeva' picketing1_val='260~900' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09202' name='Menta' picketing1_val='251~300' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09210' name='Taurkalne' picketing1_val='240~400' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09211' name='Lacplesis' picketing1_val='231~700' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09220' name='Vecumnieki' picketing1_val='215~600' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09230' name='Misa' picketing1_val='206~500' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09240' name='Iecava' picketing1_val='195~800' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09241' name='Zalite' picketing1_val='186~400' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09251' name='Garoza' picketing1_val='177~900' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09351' name='Ilmāja' picketing1_val='187~280' picketing1_comm='Лиепая' />"
        L"<EsrGuide kit='09355' name='Tore' picketing1_val='206~733' picketing1_comm='Лиепая' />"
        L"<EsrGuide kit='09360' name='Koknese' picketing1_val='94~100' picketing1_comm='Рига-Зилупе'/>"
        L"<EsrGuide kit='09370' name='Aizkraukle' picketing1_val='82~500' picketing1_comm='Рига-Зилупе'/>"
        L"<EsrGuide kit='09380' name='Skriveri' picketing1_val='72~700' picketing1_comm='Рига-Зилупе'/>"
        L"<EsrGuide kit='09400' name='Lielvarde' picketing1_val='51~400' picketing1_comm='Рига-Зилупе'/>"
        L"<EsrGuide kit='09410' name='Ogre' picketing1_val='34~400' picketing1_comm='Рига-Зилупе'/>"
        L"<EsrGuide kit='09420' name='Salaspils' picketing1_val='18~300' picketing1_comm='Рига-Зилупе'/>"
        L"<EsrGuide kit='09510' name='Līgatne' picketing1_val='64~100' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='09511' name='Sigulda' picketing1_val='53~100' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='09520' name='Inčukalns' picketing1_val='40~600' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='09530' name='Vangaži' picketing1_val='34~200' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='09540' name='Krievupe' picketing1_val='29~200' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='09750' name='Tukums-II' picketing1_val='108~300' picketing1_comm='Вентспилс' picketing2_val='64~400' picketing2_comm='Рига-Тукумс'/>"
        L"<EsrGuide kit='09751' name='Zvare' picketing1_val='97~900' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09764' name='Livberze' picketing1_val='144~800' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09772' name='Slampe' picketing1_val='125~700' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09780' name='Kandava' picketing1_val='84~400' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09790' name='Sabile' picketing1_val='72~700' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09800' name='Stende' picketing1_val='65~300' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09801' name='Lici' picketing1_val='57~200' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09802' name='Spare' picketing1_val='46~200' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09803' name='Usma' picketing1_val='36~900' picketing1_comm='Вентспилс'/>"
        L"<EsrGuide kit='09860' name='Liepāja' picketing1_val='229~700' picketing1_comm='Лиепая'/>"
        L"<EsrGuide kit='11001' name='Daugavpils D' picketing1_val='392~500' picketing1_comm='Вентспилс' picketing2_val='530~' picketing2_comm='Россия' picketing3_val='530~' picketing3_comm='Турмантас' />"
        L"<EsrGuide kit='11002' name='Daugavpils P(pāra)' picketing1_val='390~386' picketing1_comm='Вентспилс' picketing2_val='0~2' picketing2_comm='Дау-ск' picketing3_val='531~992' picketing3_comm='Россия' picketing4_val='531~992' picketing4_comm='Турмантас' />"
        L"<EsrGuide kit='11010' name='Daugavpils-Šķ.' picketing1_val='0~1' picketing1_comm='3-Д-СК' picketing2_val='3~' picketing2_comm='387км' picketing3_val='390~' picketing3_comm='Вентспилс' picketing4_val='5~' picketing4_comm='Дау-ск' picketing5_val='201~400' picketing5_comm='Рокишки' picketing6_val='528~400' picketing6_comm='Россия' />"
        L"<EsrGuide kit='11071' name='I.p.3.km' picketing1_val='4~200' picketing1_comm='3-Д-СК' picketing2_val='0~1' picketing2_comm='3км' picketing3_val='200~161' picketing3_comm='Рокишки' picketing4_val='532~639' picketing4_comm='Турмантас'/>"
        L"<EsrGuide kit='11074' name='C.p.191.km' picketing1_val='0~100' picketing1_comm='1км' picketing2_val='190~646' picketing2_comm='Рокишки' />"
        L"<EsrGuide kit='11075' name='C.p.192.km' picketing1_val='0~100' picketing1_comm='1км-192' picketing2_val='191~616' picketing2_comm='Рокишки' />"
        L"<EsrGuide kit='11076' name='C.p.5.km' picketing1_val='2~' picketing1_comm='3км' picketing2_val='5~' picketing2_comm='Грива-5' picketing3_val='197~905' picketing3_comm='Рокишки' />"
        L"<EsrGuide kit='11081' name='Lugaži' picketing1_val='164~100' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='11082' name='Saule' picketing1_val='154~900' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='11090' name='Strenči' picketing1_val='141~100' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='11091' name='Brenguļi' picketing1_val='129~500' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='11100' name='Valmiera' picketing1_val='120~800' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='11101' name='Bāle' picketing1_val='113~800' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='11110' name='Lode' picketing1_val='105~300' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='11111' name='Jāņamuiža' picketing1_val='98~400' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='11120' name='Cēsis' picketing1_val='93~' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='11121' name='Āraiši' picketing1_val='83~300' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='11130' name='Ieriķi' picketing1_val='73~800' picketing1_comm='Земитани-Валга' />"
        L"<EsrGuide kit='11290' name='Zilupe' picketing1_val='279~700' picketing1_comm='Рига-Зилупе' />"
        L"<EsrGuide kit='11291' name='Nerza' picketing1_val='268~300' picketing1_comm='Рига-Зилупе' />"
        L"<EsrGuide kit='11292' name='Istalsna' picketing1_val='257~800' picketing1_comm='Рига-Зилупе' />"
        L"<EsrGuide kit='11300' name='Ludza' picketing1_val='248~300' picketing1_comm='Рига-Зилупе' />"
        L"<EsrGuide kit='11301' name='Cirma' picketing1_val='236~400' picketing1_comm='Рига-Зилупе' />"
        L"<EsrGuide kit='11302' name='Taudejāņi' picketing1_val='231~300' picketing1_comm='Рига-Зилупе' />"
        L"<EsrGuide kit='11310' name='Rezekne II' picketing1_val='224~300' picketing1_comm='Рига-Зилупе' picketing2_val='444~' picketing2_comm='Россия'/>"
        L"<EsrGuide kit='11311' name='Sakstagals' picketing1_val='211~900' picketing1_comm='Рига-Зилупе'/>"
        L"<EsrGuide kit='11312' name='Rēzekne IIA' picketing1_val='225~300' picketing1_comm='Рига-Зилупе' picketing2_val='443~' picketing2_comm='Россия' />"
        L"<EsrGuide kit='11321' name='C.p.223km' picketing1_val='223~' picketing1_comm='Рига-Зилупе' picketing2_val='443~900' picketing2_comm='Россия' />"
        L"<EsrGuide kit='11380' name='Vilani' picketing1_val='198~200' picketing1_comm='Рига-Зилупе'/>"
        L"<EsrGuide kit='11381' name='Varaklani' picketing1_val='187~800' picketing1_comm='Рига-Зилупе'/>"
        L"<EsrGuide kit='11390' name='Stirniene' picketing1_val='179~600' picketing1_comm='Рига-Зилупе'/>"
        L"<EsrGuide kit='11391' name='Atasiene' picketing1_val='164~100' picketing1_comm='Рига-Зилупе'/>"
        L"<EsrGuide kit='11401' name='Mezare' picketing1_val='154~400' picketing1_comm='Рига-Зилупе'/>"
        L"<EsrGuide kit='11412' name='Kukas' picketing1_val='142~400' picketing1_comm='Рига-Зилупе'/>"
        L"<EsrGuide kit='11419' name='Asote' picketing1_val='311~' picketing1_comm='Вентспилс' />"
        L"<EsrGuide kit='11420' name='Krustpils' picketing1_val='301~900' picketing1_comm='Вентспилс' picketing2_val='129~400' picketing2_comm='Рига-Зилупе'/>"
        L"<EsrGuide kit='11422' name='Trepe' picketing1_val='318~700' picketing1_comm='Вентспилс' />"
        L"<EsrGuide kit='11430' name='Līvāni' picketing1_val='330~800' picketing1_comm='Вентспилс' />"
        L"<EsrGuide kit='11431' name='Jersika' picketing1_val='341~800' picketing1_comm='Вентспилс' />"
        L"<EsrGuide kit='11432' name='Sergunta' picketing1_val='351~200' picketing1_comm='Вентспилс' />"
        L"<EsrGuide kit='11442' name='Vabole' picketing1_val='370~600' picketing1_comm='Вентспилс' />"
        L"<EsrGuide kit='11443' name='Līksna' picketing1_val='375~500' picketing1_comm='Вентспилс' />"
        L"<EsrGuide kit='11445' name='C.p.383km' picketing1_val='10~100' picketing1_comm='1км' picketing2_val='382~891' picketing2_comm='Вентспилс' />"
        L"<EsrGuide kit='11446' name='Nīcgale' picketing1_val='358~700' picketing1_comm='Вентспилс' />"
        L"<EsrGuide kit='11451' name='C.p.387km' picketing1_val='0~1' picketing1_comm='387км' picketing2_val='387~415' picketing2_comm='Вентспилс' />"
        L"<EsrGuide kit='11460' name='Ilukste' picketing1_val='179~800' picketing1_comm='Рокишки' />"
        L"<EsrGuide kit='11470' name='Eglaine' picketing1_val='173~400' picketing1_comm='Рокишки' />"
        L"<EsrGuide kit='11760' name='Plavinas' picketing1_val='0~1' picketing1_comm='Плявиняс' picketing2_val='112~300' picketing2_comm='Рига-Зилупе'/>"
        L"<SpanGuide kit='06610:11290' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='304~' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='280~200' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='08620:11081' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord />"
        L"    <rwcoord picketing1_val='164~600' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='09000:09420' orientation='even'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='9~797' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"  <Way wayNum='2'>"
        L"    <rwcoord picketing1_val='9~818' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='09355:09860' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='207~513' picketing1_comm='Лиепая' />"
        L"    <rwcoord picketing1_val='228~700' picketing1_comm='Лиепая' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='09360:09370' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='93~500' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='83~' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"  <Way wayNum='2'>"
        L"    <rwcoord picketing1_val='93~500' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='83~' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='09360:11760' orientation='even'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='94~600' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='111~500' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"  <Way wayNum='2'>"
        L"    <rwcoord picketing1_val='94~600' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='111~500' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Aizkraukle - Skrīveri -->"
        L"<SpanGuide kit='09370:09380' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='82~' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='73~500' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"  <Way wayNum='2'>"
        L"    <rwcoord picketing1_val='82~' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='73~500' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Skrīveri - Lielvārde -->"
        L"<SpanGuide kit='09380:09400' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='72~' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='52~' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"  <Way wayNum='2'>"
        L"    <rwcoord picketing1_val='72~' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='52~' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='09400:09410' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='51~' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='35~' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"  <Way wayNum='2'>"
        L"    <rwcoord picketing1_val='51~' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='35~' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='09410:09420' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='33~500' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='19~' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"  <Way wayNum='2'>"
        L"    <rwcoord picketing1_val='33~500' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='19~' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='09510:09511' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='63~600' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='53~600' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Līgatne - Ieriķi -->"
        L"<SpanGuide kit='09510:11130' orientation='even'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='64~600' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='73~300' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Sigulda - Inčukalns -->"
        L"<SpanGuide kit='09511:09520' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='52~600' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='41~100' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"  <Way wayNum='2'>"
        L"    <rwcoord picketing1_val='52~600' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='41~100' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Inčukalns - Vangaži -->"
        L"<SpanGuide kit='09520:09530' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='40~100' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='34~700' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"  <Way wayNum='2'>"
        L"    <rwcoord picketing1_val='40~700' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='34~700' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Vangaži - Krievupe -->"
        L"<SpanGuide kit='09530:09540' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='33~700' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='29~700' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='09750:09751' orientation='odd'>"
        L"<Way wayNum='1'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='98~858' picketing1_comm='Вентспилс' />"
        L"</Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11290:11291' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='278~850' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='269~537' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11291:11292' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='267~812' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='259~113' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11292:11300' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='257~467' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='249~264' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11300:11301' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='247~409' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='237~139' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Cirma - Taudejāņi -->"
        L"<SpanGuide kit='11301:11302' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='235~398' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='231~792' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Taudejāņi - Rēzekne IIA -->"
        L"<SpanGuide kit='11302:11312' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='230~552' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='227~718' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11002:11451' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='389~476' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='387~840' picketing1_comm='Вентспилс' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11010:11071' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='1~190' picketing1_comm='3-Д-СК' />"
        L"    <rwcoord picketing1_val='3~500' picketing1_comm='3-Д-СК' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11010:11451' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='388~856' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='387~836' picketing1_comm='Вентспилс' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11071:11076' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='0~3' picketing1_comm='3км' />"
        L"    <rwcoord picketing1_val='1~500' picketing1_comm='3км' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11074:11075' orientation='even'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='190~159' picketing1_comm='Рокишки' />"
        L"    <rwcoord picketing1_val='191~500' picketing1_comm='Рокишки' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон C.p.191.km - Ilukste -->"
        L"<SpanGuide kit='11074:11460' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='190~507' picketing1_comm='Рокишки' />"
        L"    <rwcoord picketing1_val='180~641' picketing1_comm='Рокишки' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11075:11076' orientation='even'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='191~671' picketing1_comm='Рокишки' />"
        L"    <rwcoord picketing1_val='197~682' picketing1_comm='Рокишки' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11081:11082' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='163~500' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='155~500' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Saule - Strenči -->"
        L"<SpanGuide kit='11082:11090' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='154~500' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='141~600' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Strenči - Brenguļi -->"
        L"<SpanGuide kit='11090:11091' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='140~600' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='130~' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Brenguļi - Valmiera -->"
        L"<SpanGuide kit='11091:11100' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='129~' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='121~' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Valmiera - Bāle -->"
        L"<SpanGuide kit='11100:11101' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='120~300' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='114~300' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Bāle - Lode -->"
        L"<SpanGuide kit='11101:11110' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='113~300' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='105~800' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Lode - Jāņamuiža -->"
        L"<SpanGuide kit='11110:11111' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='104~800' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='98~900' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Jāņamuiža - Cēsis -->"
        L"<SpanGuide kit='11111:11120' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='97~900' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='93~500' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"  <Way wayNum='2'>"
        L"    <rwcoord picketing1_val='97~900' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='93~500' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Cēsis - Āraiši -->"
        L"<SpanGuide kit='11120:11121' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='92~' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='83~800' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Āraiši - Ieriķi -->"
        L"<SpanGuide kit='11121:11130' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='82~800' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='74~300' picketing1_comm='Земитани-Валга' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11419:11420' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='310~674' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='302~651' picketing1_comm='Вентспилс' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Asote - Trepe -->"
        L"<SpanGuide kit='11419:11422' orientation='even'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='312~209' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='318~129' picketing1_comm='Вентспилс' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Krustpils - Pļaviņas -->"
        L"<SpanGuide kit='11420:11760' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='127~886' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='113~206' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"  <Way wayNum='2'>"
        L"    <rwcoord picketing1_val='127~886' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='113~206' picketing1_comm='Рига-Зилупе' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11422:11430' orientation='even'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='319~630' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='330~181' picketing1_comm='Вентспилс' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11430:11431' orientation='even'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='331~816' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='340~650' picketing1_comm='Вентспилс' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11431:11432' orientation='even'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='342~401' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='349~940' picketing1_comm='Вентспилс' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11432:11446' orientation='even'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='351~454' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='358~83' picketing1_comm='Вентспилс' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11442:11443' orientation='even'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='371~361' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='375~118' picketing1_comm='Вентспилс' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11442:11446' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='369~864' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='359~799' picketing1_comm='Вентспилс' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11443:11445' orientation='even'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='376~540' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='382~660' picketing1_comm='Вентспилс' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11445:11451' orientation='even'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='383~39' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='387~399' picketing1_comm='Вентспилс' />"
        L"  </Way>"
        L"  <Way wayNum='2'>"
        L"    <rwcoord picketing1_val='383~39' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='387~395' picketing1_comm='Вентспилс' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='11460:11470' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='178~933' picketing1_comm='Рокишки' />"
        L"    <rwcoord picketing1_val='174~435' picketing1_comm='Рокишки' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"<!-- перегон Eglaine - Rokiski -->"
        L"<SpanGuide kit='11470:12780' orientation='odd'>"
        L"  <Way wayNum='1'>"
        L"    <rwcoord picketing1_val='171~' picketing1_comm='Рокишки' />"
        L"    <rwcoord picketing1_val='140~' picketing1_comm='Рокишки' />"
        L"  </Way>"
        L"</SpanGuide>"
        L"</EsrGuide>";
    attic::a_document doc;
    doc.load_utf8( ToUtf8(text) );
    m_esrGuide->load_from( doc.document_element() );
}

void TC_Hem_aeAdiiasReporter::tearDown()
{
    LocoCharacteristics::Shutdowner();
}

class checkWalker : public pugi::xml_tree_walker
{
public:
    //
    checkWalker();
    // Callback that is called for each node traversed
    bool for_each(pugi::xml_node& node) override;

    // Callback that is called when traversal ends
    bool end(pugi::xml_node& node) override;

private:
    bool validate_tag( std::string fullTagName );
    bool validate_attr( const std::string& fullTagName, const std::string& attrName, const std::string& attrValue );
    // таблица допустимых тегов - разово или многократно
    std::map< std::string, int > m_onceTag;
    std::map< std::string, int > m_manyTag;
    
    // проверка корректности атрибута
    std::map< std::string,
        std::map< std::string, std::function<bool( std::string value )> >
        > m_attrPred;   //проверки на содержание атрибута
    // атрибуты, которые обязательно должны встретиться
    std::map< std::string, std::set< std::string > > m_attrMatch;  // обязательные атрибуты
};


checkWalker::checkWalker()
    : m_onceTag(), m_manyTag(), m_attrPred(), m_attrMatch()
{
    // однократные теги
    m_onceTag[ "/Wrapper/REPORTING_PERIOD" ] = 0;
    const std::string WR = "/Wrapper/REPORTING_PERIOD";
#if USE_CRR_REPORTING
    m_onceTag[ WR+"/CRR_REPORTING" ] = 0;
#endif
    m_onceTag[ WR+"/TRAFFIC" ] = 0;
    // многократные теги
    m_manyTag[ WR+"/TRAFFIC/Step_Train" ] = 0;
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Step" ] = 0;
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Step/Departure" ] = 0;
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Step/Arrival" ] = 0;
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Traction" ] = 0;
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Traction/locomotive" ] = 0;
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Traction/locomotive/crew" ] = 0;
#if USE_ELECTRIC_TRAIN
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Traction/electric_train" ] = 0;
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Traction/electric_train/crew" ] = 0;
#endif
#if USE_DIESEL_TRAIN
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Traction/diesel_train" ] = 0;
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Traction/diesel_train/crew" ] = 0;
#endif
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Structure" ] = 0;
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Wagons" ] = 0;
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Carrier" ] = 0;
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Analize" ] = 0;
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Analize/Delay_on_departure" ] = 0;
    m_manyTag[ WR+"/TRAFFIC/Step_Train/Analize/Delay_on_arrival" ] = 0;
    // атрибуты
    auto DDMMYY = [](std::string v){
        return std::regex_match( v, std::regex( "^\\d{2}\\.\\d{2}\\.\\d{2}$" ) );
    };
    auto HHMM   = [](std::string v){
        return std::regex_match( v, std::regex( "^\\d{2}\\:\\d{2}$" ) );
    };
    auto HHMMSS = [](std::string v){
        return std::regex_match( v, std::regex( "^\\d{2}\\:\\d{2}\\:\\d{2}$" ) );
    };
    auto DMY4_HMS = [](std::string v){
        return std::regex_match( v, std::regex( "^\\d{2}\\.\\d{2}\\.\\d{4}-\\d{2}\\:\\d{2}\\'\\d{2}$" ) );
    };
    auto DMY2_HM = [](std::string v){
        return std::regex_match( v, std::regex( "^\\d{2}\\.\\d{2}\\.\\d{2}-\\d{2}\\:\\d{2}$" ) );
    };
    auto DELAY_HMS = [](std::string v){
        return std::regex_match( v, std::regex( "^\\d{2}\\:\\d{2}\\'\\d{2}$" ) );
    };
    auto DELAY_MS = [](std::string v){
        return std::regex_match( v, std::regex( "^\\d{2}\\'\\d{2}$" ) );
    };
    auto DEC1 = [](std::string v){ return std::regex_match( v, std::regex( "^\\d$" ) ); };
    auto DEC2 = [](std::string v){ return std::regex_match( v, std::regex( "^\\d{1,2}$" ) ); };
    auto DEC2NZ=[](std::string v){ return std::regex_match( v, std::regex( "^[123456789](\\d)?$" ) ); };
    auto DEC2FX=[](std::string v){ return std::regex_match( v, std::regex( "^\\d{2}$" ) ); };
    auto DEC3 = [](std::string v){ return std::regex_match( v, std::regex( "^\\d{1,3}$" ) ); };
    auto DEC4 = [](std::string v){ return std::regex_match( v, std::regex( "^\\d{1,4}$" ) ); };
    auto DEC5 = [](std::string v){ return std::regex_match( v, std::regex( "^\\d{1,5}$" ) ); };
    auto DEC7 = [](std::string v){ return std::regex_match( v, std::regex( "^\\d{1,7}$" ) ); };
    auto I434 = [](std::string v){ return std::regex_match( v, std::regex( "^\\d{4}-\\d{3}-\\d{4}$" ) ); };
    auto DTNA = [](std::string v){ return std::regex_match( v, std::regex( "^G?S?(N\\d{1,4})?(BM)?M?$" ) ); };
    auto ESR5K= [](std::string v){ return std::regex_match( v, std::regex( "^\\d{5}(:\\d{5})?$" ) ); };
    auto KMPK = [](std::string v){ return std::regex_match( v, std::regex( "^\\d{1,4}\\.\\d{1,2}$" ) ); };
    auto TEXT = [](std::string v){ return !v.empty(); };
    // таблица валидации атрибутов
    m_attrPred[ WR ]["Report_code"]   = [](std::string v){ return "NEW" == v; };
    m_attrPred[ WR ]["Report_number"] = [](std::string v){ return std::string::npos == v.find_first_not_of("01234567879"); };
    m_attrPred[ WR ]["Creation_date"] = DDMMYY;
    m_attrPred[ WR ]["Start_date"] = DDMMYY;
    m_attrPred[ WR ]["Start_time"] = HHMM;
    m_attrPred[ WR ]["Finish_date"] = DDMMYY;
    m_attrPred[ WR ]["Finish_time"] = HHMM;
    m_attrPred[ WR ]["file"] = [](std::string /*v*/){ return true; };

#if USE_CRR_REPORTING
    m_attrPred[ WR + "/CRR_REPORTING" ]["Crr_Report_number"]   = [](std::string v){ return "NEW" == v; };
    m_attrPred[ WR + "/CRR_REPORTING" ]["Creation_date"] = DDMMYY;
#endif

    const std::string WRT = WR + "/TRAFFIC";
    m_attrPred[ WRT + "/Step_Train" ]["ID_code"] = DEC7;

    const std::string WRTS = WRT + "/Step_Train";

    m_attrPred[ WRTS + "/Step" ]["number"] = DEC4;
    m_attrPred[ WRTS + "/Step" ]["Liter"] = DTNA;
    m_attrPred[ WRTS + "/Step" ]["index"] = I434;

    {
        const std::string WRTSA = WRTS + "/Analize";

        m_attrPred[ WRTSA ]["Regulatory_train_number"] = DEC4;
        m_attrPred[ WRTSA ]["Regulatory_departure_time"] = DELAY_HMS;
        m_attrPred[ WRTSA ]["Regulatory_arrival_time"] = DELAY_HMS;
        m_attrPred[ WRTSA ]["Regulatory_running_time"] = DELAY_MS;

        ///\todo Пока заглушки для проверки.
        m_attrPred[ WRTSA + "/Delay_on_departure" ]["duration"] = TEXT;
        m_attrPred[ WRTSA + "/Delay_on_departure" ]["Code_guilty_Service"] = TEXT;
        m_attrPred[ WRTSA + "/Delay_on_departure" ]["reason_code_delay"] = TEXT;
        
        m_attrPred[ WRTSA + "/Delay_on_arrival" ]["duration"] = TEXT;
        m_attrPred[ WRTSA + "/Delay_on_arrival" ]["Code_guilty_Service"] = TEXT;
        m_attrPred[ WRTSA + "/Delay_on_arrival" ]["reason_code_delay"] = TEXT;
    }
    {
        const std::string WRTSS = WRTS + "/Step";

        m_attrPred[ WRTSS + "/Departure" ]["Esr"] = ESR5K;
        m_attrPred[ WRTSS + "/Departure" ]["KmPk"] = KMPK;
        m_attrPred[ WRTSS + "/Departure" ]["IoWay"] = DEC2;
        m_attrPred[ WRTSS + "/Departure" ]["SpanWay"] = DEC2NZ;
        m_attrPred[ WRTSS + "/Departure" ]["Fakt.data_time"] = DMY4_HMS;

        m_attrPred[ WRTSS + "/Arrival" ]["Esr"] = ESR5K;
        m_attrPred[ WRTSS + "/Arrival" ]["KmPk"] = KMPK;
        m_attrPred[ WRTSS + "/Arrival" ]["IoWay"] = DEC2;
        m_attrPred[ WRTSS + "/Arrival" ]["SpanWay"] = DEC2NZ;
        m_attrPred[ WRTSS + "/Arrival" ]["Fakt.data_time"] = DMY4_HMS;
    }
    {
        const std::string WRTST = WRTS + "/Traction";
        {
            const std::string WRTSTL = WRTST + "/locomotive";
            m_attrPred[ WRTSTL ]["number"] = DEC4;
            m_attrPred[ WRTSTL ]["series"] = DEC3;
            m_attrPred[ WRTSTL ]["movement_type"] = DEC1;

            m_attrPred[ WRTSTL + "/crew" ]["Driver_name"] = TEXT;
            m_attrPred[ WRTSTL + "/crew" ]["Person_mode"] = DEC1;
            m_attrPred[ WRTSTL + "/crew" ]["Onset"] = DMY2_HM;
        }
#if USE_ELECTRIC_TRAIN
        {
            const std::string WRTSTE = WRTST + "/electric_train";
            m_attrPred[ WRTSTE ]["number"] = DEC4;
            m_attrPred[ WRTSTE ]["series"] = DEC3;

            m_attrPred[ WRTSTE + "/crew" ]["Driver_name"] = TEXT;
            m_attrPred[ WRTSTE + "/crew" ]["Person_mode"] = DEC1;
            m_attrPred[ WRTSTE + "/crew" ]["Onset"] = DMY2_HM;
        }
#endif
#if USE_DIESEL_TRAIN
        {
            const std::string WRTSTD = WRTST + "/diesel_train";
            m_attrPred[ WRTSTD ]["number"] = DEC4;
            m_attrPred[ WRTSTD ]["series"] = DEC3;

            m_attrPred[ WRTSTD + "/crew" ]["Driver_name"] = TEXT;
            m_attrPred[ WRTSTD + "/crew" ]["Person_mode"] = DEC1;
            m_attrPred[ WRTSTD + "/crew" ]["Onset"] = DMY2_HM;
        }
#endif
    }
    {
        m_attrPred[ WRTS + "/Structure" ]["gross_weight"] = DEC4;
        m_attrPred[ WRTS + "/Structure" ]["net_weight"] = DEC4;
        m_attrPred[ WRTS + "/Structure" ]["Conventional_length"] = DEC3;
        m_attrPred[ WRTS + "/Structure" ]["Number_of_axes"]   = DEC3;
        m_attrPred[ WRTS + "/Structure" ]["loaded_cars"]      = DEC3;
        m_attrPred[ WRTS + "/Structure" ]["empty_cars"]       = DEC3;
        m_attrPred[ WRTS + "/Structure" ]["non-working_cars"] = DEC3;
        m_attrPred[ WRTS + "/Structure" ]["passenger_cars"]   = DEC3;
        m_attrPred[ WRTS + "/Structure" ]["non-working_locomotive"] = DEC3;
    }
    {
        const std::string WRTSW = WRTS + "/Wagons";
        std::string subWagons[] = {
            "boxcar", "platform", "gondola", "tank", "refrigerator",
            "other", "cement_car", "FPL", "MB", "Grain_car"
        };
        for( auto& sub : subWagons )
        {
            m_manyTag[  WRTSW + "/" + sub ] = 0;
            m_attrPred[ WRTSW + "/" + sub ]["loaded"]      = DEC3;
            m_attrPred[ WRTSW + "/" + sub ]["empty"]       = DEC3;
            m_attrPred[ WRTSW + "/" + sub ]["not_working"] = DEC3;
        }

        std::string lineCars[] = {
            // "luxury_cars", - открыть после дополнения тестовых данных
            // "compartment_cars", - открыть после дополнения тестовых данных
            // "reserved_seat_cars", - открыть после дополнения тестовых данных
            // "general_cars", - открыть после дополнения тестовых данных
            // "Postal_and_baggage_cars", - открыть после дополнения тестовых данных
            // "Diner_cars",              - открыть после дополнения тестовых данных
            "Electric_motor_cars",
            "Total_electric_cars",
            "Diesel_motor_cars",
            "Total_diesel_cars"
        };
        for( auto& li : lineCars )
            m_attrPred[ WRTSW ][li] = DEC3;
    }
    m_attrPred[ WRTS + "/Carrier" ]["code"] = DEC2FX;

    // таблица атрибутов, которые обязательно должны встретиться
    for( auto& t : m_attrPred )
    {
        for( auto& a : t.second )
            m_attrMatch[ t.first ].insert( a.first );
    }
}

bool checkWalker::for_each(pugi::xml_node& node)
{
    auto fullName = node.path();
    //OutputDebugString( FromUtf8("walk::for_each: " + fullName+"\n" ).c_str() );
    bool result = validate_tag(fullName);
    if ( result )
    {
        for( auto a : node.attributes() )
        {
            if ( !validate_attr( fullName, a.name(), a.value() ) )
            {
                result = false;
                break;
            }
        }
    }
    return result;
}

bool checkWalker::end(pugi::xml_node& node)
{
    auto fullName = node.path();
    //OutputDebugString( FromUtf8("walk::end:      " + fullName+"\n" ).c_str() );
    for( auto& sing : m_onceTag )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "<"+sing.first+">", 1, sing.second );
    }
    for( auto& mult : m_manyTag )
    {
        CPPUNIT_ASSERT_MESSAGE( "<"+mult.first+">", 1 <= mult.second );
    }
    for( auto& atrMatch : m_attrMatch )
    {
        for( auto& atr : atrMatch.second  )
        {
            CPPUNIT_FAIL( "<"+atrMatch.first+"> ["+atr+"] - not found" );
        }
    }

    return true;
}

bool checkWalker::validate_tag(std::string fullTagName)
{
    bool result = false;
    auto it1 = m_onceTag.find( fullTagName );
    auto it2 = m_manyTag.find( fullTagName );
    if ( m_onceTag.end() != it1 )
    {
        it1->second ++;
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "<"+fullTagName+">", 1, it1->second );
        result = true;
    }
    else if ( m_manyTag.end() != it2 )
    {
        it2->second ++;
        result = true;
    }
    else
    {
        CPPUNIT_FAIL( "<"+fullTagName+"> - not allowed" );
        result = false;
    }
    return result;
}

bool checkWalker::validate_attr(const std::string& fullTagName, const std::string& attrName, const std::string& attrValue) 
{
    bool result = false;
    auto itT = m_attrPred.find( fullTagName );

    const std::string TA = "<"+fullTagName+"> ["+attrName+"="+attrValue+"]";
    if ( m_attrPred.end() != itT && itT->second.find(attrName)!= itT->second.end() )
    {
        auto& attrs = itT->second;
        auto itA = attrs.find( attrName );
        if ( attrs.end() != itA )
        {
            result = itA->second( attrValue );
            if ( !result )
            {
                OutputDebugString( (FromUtf8(TA) + L" - predicat false\n").c_str() );
                CPPUNIT_ASSERT_MESSAGE( TA + " - predicat false", false );
            }
        }
    }
    else if ( m_attrPred.end() != itT )
    {
        CPPUNIT_FAIL( TA+" - attribute not allowed" );
        result = false;
    }
    else
    {
        CPPUNIT_FAIL( TA+" - tag not allowed" );
        result = false;
    }

    // удалить обнаруженный атрибут из таблицы встречаемости
    auto itM = m_attrMatch.find( fullTagName );
    if ( m_attrMatch.end() != itM &&
        0 != itM->second.erase( attrName ) &&
        itM->second.empty()
        )
        m_attrMatch.erase( itM );

    return result;
}

void TC_Hem_aeAdiiasReporter::makeReport()
{
    fillCargo();

    auto& hl = *m_happenLayer;
    auto& al = *m_asoupLayer;
    interval_time_t ri( 
        UTC_From_MoscowLocal( bt::ptime(boost::gregorian::date(2019,1,12), bt::hours(18) ) ),
        UTC_From_MoscowLocal( bt::ptime(boost::gregorian::date(2019,1,14), bt::hours(18) ) )
        );
    Hem::aeAdiiasReporter adRep( hl.GetReadAccess(), al.GetReadAccess(), *m_esrGuide, *m_carrier, ri );
    adRep.Action();
    std::shared_ptr<attic::a_document> resp = adRep.getResponse();
    //resp->save_file(L"E:/WorkCopy.all/Sample/Adiias/AdiiasReport.xml");
    CPPUNIT_ASSERT( resp );

    attic::a_node root = resp->document_element();
    checkWalker walker;
    root.traverse( walker );
}

void TC_Hem_aeAdiiasReporter::reportIntervals()
{
    fillCargo();

    auto& hl = *m_happenLayer;
    auto& al = *m_asoupLayer;
    {
        interval_time_t rep13( 
            UTC_From_Local( bt::ptime(boost::gregorian::date(2019,1,12), bt::hours(18) ) ),
            UTC_From_Local( bt::ptime(boost::gregorian::date(2019,1,13), bt::hours(18) ) )
            );
        Hem::aeAdiiasReporter adRep( hl.GetReadAccess(), al.GetReadAccess(), *m_esrGuide, *m_carrier, rep13 );
        adRep.Action();
        std::shared_ptr<attic::a_document> resp = adRep.getResponse();
        //resp->save_file(L"E:/WorkCopy.all/Sample/Adiias/AdiiasReport.xml");
        CPPUNIT_ASSERT( resp );
        attic::a_node rp = resp->document_element().child("REPORTING_PERIOD");
        CPPUNIT_ASSERT( rp.attribute("Creation_date").as_wstring() == L"13.01.19" );
        CPPUNIT_ASSERT( rp.attribute("Start_date"   ).as_wstring() == L"12.01.19" );
        CPPUNIT_ASSERT( rp.attribute("Finish_date"  ).as_wstring() == L"13.01.19" );
        CPPUNIT_ASSERT( rp.attribute("Start_time"   ).as_wstring() == L"18:00" );
        CPPUNIT_ASSERT( rp.attribute("Finish_time"  ).as_wstring() == L"18:00" );
        CPPUNIT_ASSERT( rp.attribute("file"  ).as_wstring() == L"DRP20190113.XML" );
    }
    {
        interval_time_t rep14( 
            UTC_From_Local( bt::ptime(boost::gregorian::date(2019,1,13), bt::hours(18) ) ),
            UTC_From_Local( bt::ptime(boost::gregorian::date(2019,1,14), bt::hours(18) ) )
            );
        Hem::aeAdiiasReporter adRep( hl.GetReadAccess(), al.GetReadAccess(), *m_esrGuide, *m_carrier, rep14 );
        adRep.Action();
        std::shared_ptr<attic::a_document> resp = adRep.getResponse();
        //resp->save_file(L"E:/WorkCopy.all/Sample/Adiias/AdiiasReport.xml");
        CPPUNIT_ASSERT( resp );
        attic::a_node rp = resp->document_element().child("REPORTING_PERIOD");
        CPPUNIT_ASSERT( rp.attribute("Creation_date").as_wstring() == L"14.01.19" );
        CPPUNIT_ASSERT( rp.attribute("Start_date"   ).as_wstring() == L"13.01.19" );
        CPPUNIT_ASSERT( rp.attribute("Finish_date"  ).as_wstring() == L"14.01.19" );
        CPPUNIT_ASSERT( rp.attribute("Start_time"   ).as_wstring() == L"18:00" );
        CPPUNIT_ASSERT( rp.attribute("Finish_time"  ).as_wstring() == L"18:00" );
        CPPUNIT_ASSERT( rp.attribute("file"  ).as_wstring() == L"DRP20190114.XML" );
    }
}

void TC_Hem_aeAdiiasReporter::fillCargo()
{
    std::wstring text_happen;
    std::wstring text_asoup;
    std::tie( text_happen, text_asoup ) = getHappenAndAsoup1();
    m_happenLayer->createPath( text_happen );
    m_asoupLayer->createPath( text_asoup );

    std::tie( text_happen, text_asoup ) = getHappenAndAsoup2();
    m_happenLayer->createPath( text_happen );
    m_asoupLayer->createPath( text_asoup );

    std::tie( text_happen, text_asoup ) = getHappenAndAsoup3();
    m_happenLayer->createPath( text_happen );
    m_asoupLayer->createPath( text_asoup );

    std::tie( text_happen, text_asoup ) = getHappenAndAsoup4();
    m_happenLayer->createPath( text_happen );
    m_asoupLayer->createPath( text_asoup );

    std::tie( text_happen, text_asoup ) = getHappenAndAsoup5();
    m_happenLayer->createPath( text_happen );
    m_asoupLayer->createPath( text_asoup );

    std::tie( text_happen, text_asoup ) = getHappenAndPvLdzElectro();
    m_happenLayer->createPath( text_happen );
    m_asoupLayer->createPath( text_asoup );

    std::tie( text_happen, text_asoup ) = getHappenAndPvLdzDiesel();
    m_happenLayer->createPath( text_happen );
    m_asoupLayer->createPath( text_asoup );

    text_happen = getSpanWork6();
    m_happenLayer->createPath( text_happen );

    text_happen = getSpanWork7();
    m_happenLayer->createPath( text_happen );

    CPPUNIT_ASSERT_EQUAL( m_happenLayer->path_count(), size_t(9) );
    CPPUNIT_ASSERT_EQUAL( m_asoupLayer ->path_count(), size_t(7) );
}

std::pair< std::wstring, std::wstring > TC_Hem_aeAdiiasReporter::getHappenAndAsoup1()
{
    const wchar_t * text_happen =
        L"<HemPath>"
        L"  <SpotEvent create_time='20190114T020000Z' name='Departure' Bdg='ASOUP 1042[11001]' parknum='1' index='1100-065-0900' num='2711' length='57' weight='5255' net_weight='3615' through='Y'>"
        L"    <feat_texts typeinfo='Т' />"
        L"    <Locomotive Series='2ТЭ10М' NumLoc='3451' Consec='1' CarrierCode='4'>"
        L"      <Crew EngineDriver='GEDZUNS' Tim_Beg='2019-01-14 02:22' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20190114T020740Z' name='Transition' Bdg='?[11002]' />"
        L"  <SpotEvent create_time='20190114T021303Z' name='Transition' Bdg='2SP[11451]' />"
        L"  <SpotEvent create_time='20190114T021303Z' name='Wrong_way' Bdg='MP2P[11445:11451]' waynum='2' />"
        L"  <SpotEvent create_time='20190114T022057Z' name='Transition' Bdg='5SP[11445]' />"
        L"  <SpotEvent create_time='20190114T023516Z' name='Arrival' Bdg='3BC[11443]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20190114T024937Z' name='Departure' Bdg='3AC[11443]' waynum='3' parknum='1' optCode='11442:11443' />"
        L"  <SpotEvent create_time='20190114T030020Z' name='Transition' Bdg='1AC[11442]' waynum='1' parknum='1' intervalSec='6' optCode='11442:11446' />"
        L"  <SpotEvent create_time='20190114T031728Z' name='Arrival' Bdg='1C[11446]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190114T031832Z' name='Departure' Bdg='1C[11446]' waynum='1' parknum='1' optCode='11432:11446' />"
        L"  <SpotEvent create_time='20190114T032829Z' name='Transition' Bdg='1C[11432]' waynum='1' parknum='1' intervalSec='18' optCode='11431:11432' />"
        L"  <SpotEvent create_time='20190114T033919Z' name='Transition' Bdg='1AC[11431]' waynum='1' parknum='2' optCode='11430:11431' />"
        L"  <SpotEvent create_time='20190114T034028Z' name='Transition' Bdg='1C[11431]' waynum='1' parknum='1' optCode='11430:11431' />"
        L"  <SpotEvent create_time='20190114T035722Z' name='Transition' Bdg='1C[11430]' waynum='1' parknum='1' intervalSec='14' optCode='11422:11430' />"
        L"  <SpotEvent create_time='20190114T035949Z' name='Span_move' Bdg='P8P[11422:11430]' waynum='1' />"
        L"</HemPath>"
        ;

    const wchar_t * text_asoup =
        L"<AsoupEvent create_time='20190114T020000Z' name='Departure' Bdg='ASOUP 1042[11001]' index='1100-065-0900' num='2711' length='57' weight='5255' net_weight='3615' through='Y' parknum='1' dirTo='11420' adjTo='11002' linkBadge='ASOUP 1042[11001]' linkTime='20190114T020000Z' linkCode='Departure'>"
        L"<![CDATA[(:1042 909/000+11000 2711 1100 065 0900 03 11420 14 01 04 00 02/04 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 05255 03615 256 74940669 94416179 000 000 00 063 001.60 007 000 000 70 055 000 000 73 055 000 000 90 001 001 000 95 001 000 000 96 000 001 000\n"
        L"Ю3 575 00034511 1 02 22 0000 00000 GEDZUNS     \n"
        L"Ю3 575 00034512 9\n"
        L"Ю4 00000 0 00 000 001.90 000 001 96 000 001 \n"
        L"Ю4 08620 2 08 056 000.70 055 000 73 055 000 90 001 000 95 001 000 \n"
        L"Ю4 00000 1 01 007 000.60 007 000 \n"
        L"Ю4 09660 1 01 007 000.60 007 000 \n"
        L"Ю12 00 74940669 1 065 08220 22106 3180 080 01 00 00 00 00 0266 20 70 0732 04 086 09000 08620 08620 08 11000 03 00000 0000 021 1 1080 00000  132 000 00000000\n"
        L"Ю12 00 74941550 159239255255 060081900270\n"
        L"Ю12 00 73957987 159239255247 0670822002550     \n"
        L"Ю12 00 73958076 191255255247 06000000 \n"
        L"Ю12 00 73036659 191237255247 058024007300     \n"
        L"Ю12 00 75135046 131237255247 0660819021505359236002650732OXP   \n"
        L"Ю12 00 73998411 191239255255 0670255\n"
        L"Ю12 00 73044612 191237255255 05502480730\n"
        L"Ю12 00 65430332 255239255255 0225\n"
        L"Ю12 00 94416179 131225081243 00000000421036302000020011960942105009600011000431448  128:)]]>\n"
        L"  <feat_texts typeinfo='Т' />"
        L"  <Locomotive Series='2ТЭ10М' NumLoc='3451' Consec='1' CarrierCode='4'>"
        L"    <Crew EngineDriver='GEDZUNS' Tim_Beg='2019-01-14 02:22' />"
        L"  </Locomotive>"
        L"</AsoupEvent>"
        ;
    return std::make_pair(text_happen,text_asoup);
}

std::pair< std::wstring, std::wstring > TC_Hem_aeAdiiasReporter::getHappenAndAsoup2()
{
    const wchar_t * text_happen =
        L"<HemPath>"
        L"  <SpotEvent create_time='20190113T172000Z' name='Transition' Bdg='ASOUP 1042[06610]' index='1800-850-1131' num='2613' length='58' weight='3674' net_weight='2178' through='Y'>"
        L"    <Locomotive Series='2ТЭ25KM' NumLoc='318' Depo='131' Consec='1' CarrierCode='4'>"
        L"      <Crew EngineDriver='KAЧAH' TabNum='7937' Tim_Beg='2019-01-13 10:32' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20190113T180411Z' name='Arrival' Bdg='5C[11290]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20190113T185450Z' name='Departure' Bdg='5C[11290]' waynum='5' parknum='1' optCode='11290:11291' />"
        L"  <SpotEvent create_time='20190113T191301Z' name='Arrival' Bdg='2C[11291]' waynum='2' parknum='1' />"
        L"  <SpotEvent create_time='20190113T192047Z' name='Departure' Bdg='2C[11291]' waynum='2' parknum='1' optCode='11291:11292' />"
        L"  <SpotEvent create_time='20190113T193657Z' name='Transition' Bdg='1C[11292]' waynum='1' parknum='1' intervalSec='8' optCode='11292:11300' />"
        L"  <SpotEvent create_time='20190113T194657Z' name='Transition' Bdg='1C[11300]' waynum='1' parknum='1' intervalSec='2' optCode='11300:11301' />"
        L"  <SpotEvent create_time='20190113T200109Z' name='Transition' Bdg='1C[11301]' waynum='1' parknum='1' intervalSec='15' optCode='11301:11302' />"
        L"  <SpotEvent create_time='20190113T200805Z' name='Transition' Bdg='1C[11302]' waynum='1' parknum='1' intervalSec='10' optCode='11302:11312' />"
        L"  <SpotEvent create_time='20190113T201544Z' name='Arrival' Bdg='7AC[11312]' waynum='7' parknum='1' />"
        L"  <SpotEvent create_time='20190114T023100Z' name='Disform' Bdg='ASOUP 1042[11312]' waynum='7' parknum='1' />"
        L"</HemPath>"
        ;

    const wchar_t * text_asoup =
        L"<AsoupEvent create_time='20190113T172000Z' name='Transition' Bdg='ASOUP 1042[06610]' index='1800-850-1131' num='2613' length='58' weight='3674' net_weight='2178' through='Y' dirFrom='06660' dirTo='11290' adjFrom='06660' adjTo='11290' linkBadge='ASOUP 1042[06610]' linkTime='20190113T172000Z' linkCode='Transition'>"
        L"<![CDATA[(:1042 909/000+06610 2613 1800 850 1131 04 06660+11290 13 01 19 20 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 058 03674 02178 240 94358884 95224945 000 000 00 038 022.20 000 001 000 60 007 000 000 70 006 020 000 73 001 000 000 90 025 001 000 92 007 000 000 95 003 000 000 96 015 000 000\n"
        L"Ю3 640 00003181 1 10 32 0131 07937 KAЧAH       \n"
        L"Ю4 00000 1 01 034 022.20 000 001 60 003 000 70 006 020 73 001 000 90 025 001 92 007 000 95 003 000 96 015 000 \n"
        L"Ю4 09000 1 01 000 001.20 000 001 \n"
        L"Ю4 09830 1 01 025 020.70 001 020 73 001 000 90 024 000 92 007 000 95 002 000 96 015 000 \n"
        L"Ю4 09050 1 01 000 001.90 000 001 \n"
        L"Ю4 11470 2 10 001 000.60 001 000 \n"
        L"Ю4 00000 1 02 003 000.60 003 000 \n"
        L"Ю4 09060 1 01 001 000.60 001 000 \n"
        L"Ю4 09440 1 01 002 000.60 002 000 \n"
        L"Ю4 11420 1 02 003 000.60 003 000 \n"
        L"Ю4 09070 1 01 002 000.70 002 000 \n"
        L"Ю4 09660 1 01 003 000.70 003 000 \n"
        L"Ю4 09290 1 01 001 000.90 001 000 95 001 000 \n"
        L"Ю12 00 94358884 1 049 09830 00300 8319 485 00 00 00 02 00 0210 30 96 0942 04 105 11310 09830 00000 01 09820 42 62090 8079 020 0 0000 0      132 000 00000000\n"
        L"Ю12 00 94359452 255255255255 \n"
        L"Ю12 00 94429685 255239255255 0209\n"
        L"Ю12 00 94359551 255239255255 0210\n"
        L"Ю12 00 94358942 255255255255:)]]>"
        L"<Locomotive Series='2ТЭ25KM' NumLoc='318' Depo='131' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='KAЧAH' TabNum='7937' Tim_Beg='2019-01-13 10:32' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        ;
    return std::make_pair(text_happen,text_asoup);
}

std::pair< std::wstring, std::wstring > TC_Hem_aeAdiiasReporter::getHappenAndAsoup3()
{
    const wchar_t * text_happen =
        L"<HemPath>"
        L"  <SpotEvent create_time='20190113T173829Z' name='Form' Bdg='4JC[09006]' waynum='4' parknum='1' index='0900-198-1100' num='2766' length='57' weight='1697' net_weight='459' through='Y'>"
        L"  <feat_texts typeinfo='Д' />"
        L"  <Locomotive Series='2М62УМ' NumLoc='90' Consec='1' CarrierCode='4'>"
        L"  <Crew EngineDriver='BALAPKIN' Tim_Beg='2019-01-13 21:02' />"
        L"  </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20190113T201523Z' name='Departure' Bdg='4JC[09006]' waynum='4' parknum='1' optCode='09008' />"
        L"  <SpotEvent create_time='20190113T202129Z' name='Transition' Bdg='3C[09008]' waynum='3' parknum='1' intervalSec='26' optCode='09000:09420' />"
        L"  <SpotEvent create_time='20190113T202403Z' name='Transition' Bdg='2AC[09000]' waynum='2' parknum='1' intervalSec='19' optCode='09000:09420' />"
        L"  <SpotEvent create_time='20190113T203605Z' name='Transition' Bdg='2C[09420]' waynum='2' parknum='1' intervalSec='16' optCode='09410:09420' />"
        L"  <SpotEvent create_time='20190113T205209Z' name='Transition' Bdg='1C[09410]' waynum='1' parknum='1' intervalSec='2' optCode='09400:09410' />"
        L"  <SpotEvent create_time='20190113T211046Z' name='Transition' Bdg='2C[09400]' waynum='2' parknum='1' intervalSec='16' optCode='09380:09400' />"
        L"  <SpotEvent create_time='20190113T213237Z' name='Transition' Bdg='2C[09380]' waynum='2' parknum='1' intervalSec='19' optCode='09370:09380' />"
        L"  <SpotEvent create_time='20190113T214247Z' name='Transition' Bdg='2C[09370]' waynum='2' parknum='1' intervalSec='12' optCode='09360:09370' />"
        L"  <SpotEvent create_time='20190113T215512Z' name='Transition' Bdg='2C[09360]' waynum='2' parknum='1' intervalSec='1' optCode='09360:11760' />"
        L"  <SpotEvent create_time='20190113T221327Z' name='Transition' Bdg='2AC[11760]' waynum='2' parknum='2' intervalSec='2' optCode='11420:11760' />"
        L"  <SpotEvent create_time='20190113T221355Z' name='Transition' Bdg='2C[11760]' waynum='2' parknum='1' optCode='11420:11760' />"
        L"  <SpotEvent create_time='20190113T223453Z' name='Arrival' Bdg='4C[11420]' waynum='4' parknum='1' />"
        L"  <SpotEvent create_time='20190114T000009Z' name='Departure' Bdg='4C[11420]' waynum='4' parknum='1' optCode='11419:11420' index='0900-198-1100' num='2766' length='58' weight='1757' net_weight='459' through='Y'>"
        L"  <feat_texts typeinfo='Д' />"
        L"  <Locomotive Series='2М62УМ' NumLoc='90' Consec='1' CarrierCode='4'>"
        L"  <Crew EngineDriver='BALAPKIN' Tim_Beg='2019-01-13 21:02' />"
        L"  </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20190114T001653Z' name='Arrival' Bdg='2C[11419]' waynum='2' parknum='1' />"
        L"  <SpotEvent create_time='20190114T002319Z' name='Departure' Bdg='2C[11419]' waynum='2' parknum='1' optCode='11419:11422' />"
        L"  <SpotEvent create_time='20190114T003454Z' name='Arrival' Bdg='3AC[11422]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20190114T004705Z' name='Departure' Bdg='3C[11422]' waynum='3' parknum='1' optCode='11422:11430' />"
        L"  <SpotEvent create_time='20190114T010525Z' name='Arrival' Bdg='2C[11430]' waynum='2' parknum='1' />"
        L"  <SpotEvent create_time='20190114T010545Z' name='Departure' Bdg='2C[11430]' waynum='2' parknum='1' optCode='11430:11431' />"
        L"  <SpotEvent create_time='20190114T011750Z' name='Transition' Bdg='1C[11431]' waynum='1' parknum='1' optCode='11431:11432' />"
        L"  <SpotEvent create_time='20190114T011823Z' name='Transition' Bdg='1AC[11431]' waynum='1' parknum='2' optCode='11431:11432' />"
        L"  <SpotEvent create_time='20190114T012752Z' name='Transition' Bdg='1C[11432]' waynum='1' parknum='1' intervalSec='16' optCode='11432:11446' />"
        L"  <SpotEvent create_time='20190114T013838Z' name='Arrival' Bdg='1C[11446]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190114T014113Z' name='Departure' Bdg='1C[11446]' waynum='1' parknum='1' optCode='11442:11446' />"
        L"  <SpotEvent create_time='20190114T015510Z' name='Transition' Bdg='1BC[11442]' waynum='1' parknum='1' intervalSec='4' optCode='11442:11443' />"
        L"  <SpotEvent create_time='20190114T020035Z' name='Transition' Bdg='1C[11443]' waynum='1' parknum='1' optCode='11443:11445' />"
        L"  <SpotEvent create_time='20190114T020710Z' name='Transition' Bdg='1SP[11445]' />"
        L"  <SpotEvent create_time='20190114T020710Z' name='Wrong_way' Bdg='N1P_BP387[11445:11451]' waynum='1' />"
        L"  <SpotEvent create_time='20190114T021333Z' name='Transition' Bdg='8SP[11451]' />"
        L"  <SpotEvent create_time='20190114T021800Z' name='Arrival' Bdg='ASOUP 1042[11010]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190114T021800Z' name='Death' Bdg='ASOUP 1042[11010]' waynum='1' parknum='1' />"
        L"</HemPath>"
        ;

    const wchar_t * text_asoup =
        L"<AsoupEvent create_time='20190113T201400Z' name='Departure' Bdg='ASOUP 1042[09006]' index='0900-198-1100' num='2766' length='57' weight='1697' net_weight='459' through='Y' waynum='4' parknum='1' dirTo='11760' adjTo='09000' linkBadge='4JC[09006]' linkTime='20190113T201523Z' linkCode='Departure'>"
        L"<![CDATA[(:1042 909/000+09000 2766 0900 198 1100 03 11760 13 01 22 14 04/04 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01697 00459 208 80014657 95700001 000 011 00 008 033.20 002 006 000 60 001 007 003 70 000 001 003 73 000 001 001 87 000 000 002 90 005 019 003 93 001 000 000 95 004 019 003\n"
        L"Ю3 583 00000901 1 21 02 0000 00000 BALAPKIN    \n"
        L"Ю3 583 00000902 9\n"
        L"Ю4 00000 0 00 000 006.60 000 005 70 000 001 73 000 001 \n"
        L"Ю4 16169 2 13 007 008.20 002 006 60 000 002 90 005 000 93 001 000 95 004 000 \n"
        L"Ю4 16169 2 35 000 008.90 000 008 95 000 008 \n"
        L"Ю4 00000 1 02 000 011.90 000 011 95 000 011 \n"
        L"Ю4 11050 1 02 000 003.90 000 003 95 000 003 \n"
        L"Ю4 11160 1 02 000 008.90 000 008 95 000 008 \n"
        L"Ю4 11290 2 83 001 000.60 001 000 \n"
        L"Ю4 00000 3 00 002 000.70 002 000 \n"
        L"Ю12 00 80014657 1 000 11000 42103 6302 905 02 00 00 00 00 0330 40 87 0800 04 159 11000 11000 00000 02 09000 43 00000 0000 025 0 0000 ПEPECЛ 128 000 00000000\n"
        L"Ю12 00 64194509 233233127255 42100900000240600600100\n"
        L"Ю12 00 57352114 229233127191 421007367010269705700086026\n"
        L"Ю12 00 57361776 255239255255 0268\n"
        L"Ю12 00 95696746 193225071135 36650421030660300000233129509501061613016169350221102009247 \n"
        L"Ю12 00 95696779 255255255255 :)]]>"
        L"<Locomotive Series='2М62УМ' NumLoc='90' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='BALAPKIN' Tim_Beg='2019-01-13 21:02' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        ;
    return std::make_pair(text_happen,text_asoup);
}

std::pair< std::wstring, std::wstring > TC_Hem_aeAdiiasReporter::getHappenAndAsoup4()
{
    const wchar_t * text_happen =
        L"<HemPath>"
        L"  <SpotEvent create_time='20190113T132404Z' name='Form' Bdg='10JC[09006]' waynum='10' parknum='1' index='0900-319-0862' num='2514' length='57' weight='3715' net_weight='2216' through='Y'>"
        L"  <feat_texts typeinfo='ТВМ' />"
        L"  <Locomotive Series='2ТЭ10У' NumLoc='184' Consec='1' CarrierCode='4'>"
        L"  <Crew EngineDriver='ZOMMERS' Tim_Beg='2019-01-13 14:44' />"
        L"  </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20190113T140302Z' name='Departure' Bdg='10JC[09006]' waynum='10' parknum='1' optCode='09006:09500' />"
        L"  <SpotEvent create_time='20190113T141623Z' name='Transition' Bdg='5C[09500]' waynum='5' parknum='1' optCode='09500:09580' />"
        L"  <SpotEvent create_time='20190113T142222Z' name='Transition' Bdg='2C[09580]' waynum='2' parknum='1' intervalSec='27' optCode='09570:09580' />"
        L"  <SpotEvent create_time='20190113T143012Z' name='Transition' Bdg='2C[09570]' waynum='2' parknum='1' optCode='09550:09570' />"
        L"  <SpotEvent create_time='20190113T144618Z' name='Transition' Bdg='2C[09550]' waynum='2' parknum='1' optCode='09540:09550' />"
        L"  <SpotEvent create_time='20190113T145316Z' name='Transition' Bdg='2C[09540]' waynum='2' parknum='1' intervalSec='27' optCode='09530:09540' />"
        L"  <SpotEvent create_time='20190113T150147Z' name='Transition' Bdg='2C[09530]' waynum='2' parknum='1' optCode='09520:09530' />"
        L"  <SpotEvent create_time='20190113T151129Z' name='Transition' Bdg='2C[09520]' waynum='2' parknum='1' intervalSec='12' optCode='09511:09520' />"
        L"  <SpotEvent create_time='20190113T152809Z' name='Transition' Bdg='4C[09511]' waynum='4' parknum='1' intervalSec='11' optCode='09510:09511' />"
        L"  <SpotEvent create_time='20190113T154223Z' name='Transition' Bdg='1C[09510]' waynum='1' parknum='1' optCode='09510:11130' />"
        L"  <SpotEvent create_time='20190113T155952Z' name='Arrival' Bdg='1C[11130]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190113T163039Z' name='Departure' Bdg='1C[11130]' waynum='1' parknum='1' optCode='11121:11130' />"
        L"  <SpotEvent create_time='20190113T164943Z' name='Transition' Bdg='1C[11121]' waynum='1' parknum='1' intervalSec='5' optCode='11120:11121' />"
        L"  <SpotEvent create_time='20190113T170017Z' name='Transition' Bdg='1V[11120]' waynum='1' parknum='1' intervalSec='5' optCode='11111:11120' />"
        L"  <SpotEvent create_time='20190113T170725Z' name='Transition' Bdg='NDP[11111]'>"
        L"  <rwcoord picketing1_val='98~900' picketing1_comm='Земитани-Валга' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20190113T171718Z' name='Transition' Bdg='1C[11110]' waynum='1' parknum='1' intervalSec='6' optCode='11101:11110' />"
        L"  <SpotEvent create_time='20190113T172613Z' name='Transition' Bdg='1С[11101]' waynum='1' parknum='1' optCode='11100:11101' />"
        L"  <SpotEvent create_time='20190113T173442Z' name='Transition' Bdg='1C[11100]' waynum='1' parknum='1' intervalSec='16' optCode='11091:11100' />"
        L"  <SpotEvent create_time='20190113T174333Z' name='Transition' Bdg='1C[11091]' waynum='1' parknum='1' optCode='11090:11091' />"
        L"  <SpotEvent create_time='20190113T175636Z' name='Transition' Bdg='1C[11090]' waynum='1' parknum='1' intervalSec='17' optCode='11082:11090' />"
        L"  <SpotEvent create_time='20190113T181535Z' name='Transition' Bdg='1C[11082]' waynum='1' parknum='1' intervalSec='5' optCode='11081:11082' />"
        L"  <SpotEvent create_time='20190113T182751Z' name='Transition' Bdg='1C[11081]' waynum='1' parknum='1' intervalSec='16' optCode='08620:11081' />"
        L"  <SpotEvent create_time='20190113T183700Z' name='Arrival' Bdg='?p1w1[08620]' waynum='1' parknum='1' index='0900-319-0862' num='2514' length='57' weight='3715' net_weight='2216' through='Y'>"
        L"  <feat_texts typeinfo='ТВМ' />"
        L"  <Locomotive Series='2ТЭ10У' NumLoc='184' Consec='1' CarrierCode='4'>"
        L"  <Crew EngineDriver='ZOMMERS' Tim_Beg='2019-01-13 14:44' />"
        L"  </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20190113T183700Z' name='Death' Bdg='?p1w1[08620]' waynum='1' parknum='1' />"
        L"</HemPath>"
        ;

    const wchar_t * text_asoup =
        L"<AsoupEvent create_time='20190113T182700Z' name='Transition' Bdg='ASOUP 1042[11081]' index='0900-319-0862' num='2514' length='57' weight='3715' net_weight='2216' through='Y' waynum='1' parknum='1' dirFrom='09000' dirTo='08620' adjFrom='11082' adjTo='08620' linkBadge='1C[11081]' linkTime='20190113T182751Z' linkCode='Transition'>"
        L"<![CDATA[(:1042 909/000+11080 2514 0900 319 0862 04 09000+08620 13 01 20 27 01/01 0 0/00 00 0\n"
        L"Ю2 B  T 00 00 00 0000 0 0 057 03715 02216 244 67893685 62789508 000 000 00 037 024.20 001 000 000 40 001 000 000 60 005 021 000 70 029 003 000 73 026 000 000 90 001 000 000 95 001 000 000\n"
        L"Ю3 550 00001841 1 14 44 0000 00000 ZOMMERS     \n"
        L"Ю3 550 00001842 9\n"
        L"Ю4 00000 0 00 000 021.60 000 021 \n"
        L"Ю4 08620 2 08 037 003.20 001 000 40 001 000 60 005 000 70 029 003 73 026 000 90 001 000 95 001 000 \n"
        L"Ю12 00 67893685 1 000 00000 10307 7494 000 00 00 00 00 00 0230 11 60 0600 04 100 08620 00600 00000 00 09000 24 09060 6302 026 0 0000 09069  128 000 00000000\n"
        L"Ю12 00 68637875 255237255255 02130608\n"
        L"Ю12 00 67622175 255237255255 02250600\n"
        L"Ю12 00 68622422 255237255255 02170608\n"
        L"Ю12 00 68686427 255239255255 0210\n"
        L"Ю12 00 68751510 255255255255 \n"
        L"Ю12 00 68974278 255239127255 0230099\n"
        L"Ю12 00 68974674 255255255255 \n"
        L"Ю12 00 67474619 255253127255 0600100\n"
        L"Ю12 00 42329714 135225070001 0500844008104379302392040040410500000086200812840941302411080128401132064\n"
        L"Ю12 00 77221125 133225124049 000081105562222770102701270077408626108402753020ARENDA128000\n"
        L"Ю12 00 62691837 255255255255 \n"
        L"Ю12 00 62789508 167255255255 066324111999:)]]>"
        L"<feat_texts typeinfo='ТВМ' />"
        L"<Locomotive Series='2ТЭ10У' NumLoc='184' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ZOMMERS' Tim_Beg='2019-01-13 14:44' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        ;
    return std::make_pair(text_happen,text_asoup);
}

std::pair< std::wstring, std::wstring > TC_Hem_aeAdiiasReporter::getHappenAndAsoup5()
{
    const wchar_t * text_happen =
        L"<HemPath>"
        L"  <SpotEvent create_time='20190113T220057Z' name='Form' Bdg='P2P_ER[11470:12780]' waynum='1' index='1255-600-1100' num='2356' length='52' weight='2650' net_weight='1533' through='Y'>"
        L"    <feat_texts typeinfo='Н1220' />"
        L"    <Locomotive Series='ER20CF' NumLoc='10' Consec='1' CarrierCode='4'>"
        L"      <Crew EngineDriver='MINKOV2' Tim_Beg='2019-01-13 19:28' />"
        L"    </Locomotive>"
        L"    <Locomotive Series='ER20CF' NumLoc='20' Depo='1202' Consec='2' CarrierCode='4'>"
        L"      <Crew EngineDriver='MINKOV2' Tim_Beg='2019-01-13 19:28' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20190113T220212Z' name='Transition' Bdg='1C_[11470]' waynum='1' parknum='1' intervalSec='6' optCode='11460:11470' />"
        L"  <SpotEvent create_time='20190113T220740Z' name='Arrival' Bdg='1C[11460]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190113T220808Z' name='Departure' Bdg='1C[11460]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190113T221742Z' name='Transition' Bdg='2SP:2+[11074]' />"
        L"  <SpotEvent create_time='20190113T221839Z' name='Transition' Bdg='1SP[11075]' />"
        L"  <SpotEvent create_time='20190113T222559Z' name='Transition' Bdg='2SP:2+[11076]' />"
        L"  <SpotEvent create_time='20190113T222906Z' name='Transition' Bdg='2C[11071]' waynum='2' parknum='1' intervalSec='2' optCode='11010:11071' />"
        L"  <SpotEvent create_time='20190113T224000Z' name='Arrival' Bdg='ASOUP 1042[11010]' waynum='8' parknum='1' />"
        L"  <SpotEvent create_time='20190113T224000Z' name='Death' Bdg='ASOUP 1042[11010]' waynum='8' parknum='1' />"
        L"</HemPath>"
        ;

    const wchar_t * text_asoup =
        L"<AsoupEvent create_time='20190113T220200Z' name='Transition' Bdg='ASOUP 1042[11470]' index='1255-600-1100' num='2356' length='52' weight='2650' net_weight='1533' through='Y' waynum='1' parknum='1' dirFrom='12550' dirTo='11000' adjFrom='12780' adjTo='11460' linkBadge='1C_[11470]' linkTime='20190113T220212Z' linkCode='Transition'>"
        L"<![CDATA[(:1042 909/000+11470 2356 1255 600 1100 04 12550+11000 14 01 00 02 01/01 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 1220 0 0 052 02650 01533 180 55375638 42361360 000 000 00 031 014.20 007 000 000 40 006 000 000 60 008 009 000 70 001 005 000 90 009 000 000 96 007 000 000\n"
        L"Ю3 618 00000100 1 19 28 0000 00000 MINKOV2     \n"
        L"Ю3 618 00000200 2 19 28 1202 00000 MINKOV2     \n"
        L"Ю4 07792 2 01 000 011.60 000 009 70 000 002 \n"
        L"Ю4 00000 1 02 001 003.70 001 003 \n"
        L"Ю4 11310 1 02 001 003.70 001 003 \n"
        L"Ю4 11290 2 17 001 000.20 001 000 \n"
        L"Ю4 11290 2 68 028 000.20 005 000 40 006 000 60 008 000 90 009 000 96 007 000 \n"
        L"Ю4 11290 2 70 001 000.20 001 000 \n"
        L"Ю12 00 55375638 1 000 04720 43619 8767 000 00 00 00 00 00 0234 12 60 5600 04 100 11000 03000 07792 01 12550 17 00000 0000 020 0 0000 124104 128 000 00000000\n"
        L"Ю12 00 52389384 133225069243 0286962075601129208026020205206122180001129068180     132\n"
        L"Ю12 00 62877048 255255255255 :)]]>"
        L"<feat_texts typeinfo='Н1220' />"
        L"<Locomotive Series='ER20CF' NumLoc='10' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='MINKOV2' Tim_Beg='2019-01-13 19:28' />"
        L"</Locomotive>"
        L"<Locomotive Series='ER20CF' NumLoc='20' Depo='1202' Consec='2' CarrierCode='4'>"
        L"<Crew EngineDriver='MINKOV2' Tim_Beg='2019-01-13 19:28' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        ;
    return std::make_pair(text_happen,text_asoup);
}

std::wstring TC_Hem_aeAdiiasReporter::getSpanWork6()
{
    const wchar_t* text_happen =
        L"<HemPath>"
        L"<SpotEvent create_time='20190114T070500Z' name='Departure' Bdg='?p2w57[09860]' waynum='57' parknum='2' index='0001-071-0935' num='8204' stone='Y' />"
        L"<SpotEvent create_time='20190114T072200Z' name='Arrival' Bdg='ASOUP 1042[09355]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190114T081000Z' name='Departure' Bdg='?p1w1[09355]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190114T081700Z' name='Arrival' Bdg='?[09355]' />"
        L"<SpotEvent create_time='20190114T081700Z' name='Departure' Bdg='?[09355]' />"
        L"<SpotEvent create_time='20190114T082300Z' name='Span_stopping_begin' Bdg='spanwork[09351:09355]' waynum='1'>"
        L"<rwcoord picketing1_val='190~100' picketing1_comm='Лиепая' />"
        L"<rwcoord picketing1_val='190~1000' picketing1_comm='Лиепая' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190114T083300Z' name='Span_stopping_end' Bdg='spanwork[09351:09355]' waynum='1'>"
        L"<rwcoord picketing1_val='190~100' picketing1_comm='Лиепая' />"
        L"<rwcoord picketing1_val='190~1000' picketing1_comm='Лиепая' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190114T083300Z' name='Death' Bdg='spanwork[09351:09355]' waynum='1'>"
        L"<rwcoord picketing1_val='190~100' picketing1_comm='Лиепая' />"
        L"<rwcoord picketing1_val='190~1000' picketing1_comm='Лиепая' />"
        L"</SpotEvent>"
        L"</HemPath>";
    return text_happen;
}

std::wstring TC_Hem_aeAdiiasReporter::getSpanWork7()
{
    const wchar_t* text_happen =
        L"<HemPath>"
        L"<SpotEvent create_time='20190114T100000Z' name='Form' Bdg='spanwork[09351:09355]' index='' num='8205' stone='Y'>"
        L"<rwcoord picketing1_val='190~' picketing1_comm='Лиепая' />"
        L"<rwcoord picketing1_val='190~100' picketing1_comm='Лиепая' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190114T100000Z' name='Span_stopping_begin' Bdg='spanwork[09351:09355]' waynum='1'>"
        L"<rwcoord picketing1_val='190~' picketing1_comm='Лиепая' />"
        L"<rwcoord picketing1_val='190~100' picketing1_comm='Лиепая' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190114T101000Z' name='Span_stopping_end' Bdg='spanwork[09351:09355]' waynum='1'>"
        L"<rwcoord picketing1_val='190~' picketing1_comm='Лиепая' />"
        L"<rwcoord picketing1_val='190~100' picketing1_comm='Лиепая' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190114T101900Z' name='Arrival' Bdg='?p1w1[09355]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190114T101900Z' name='Death' Bdg='?p1w1[09355]' waynum='1' parknum='1' />"
        L"</HemPath>";
    return text_happen;
}

std::pair< std::wstring, std::wstring > TC_Hem_aeAdiiasReporter::getHappenAndPvLdzElectro()
{
    const wchar_t * text_happen =
        L"<HemPath>"
        L"<SpotEvent create_time='20190114T023438Z' name='Form' Bdg='1BC[09751]' waynum='1' parknum='1' index='' num='6520'  length='24' weight='234' suburbreg='Y'>"
        L"<Locomotive Series='2ТЭ116' NumLoc='435'>"
        L"<Crew EngineDriver='Ziiins' Tim_Beg='2019-02-12 00:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190114T024438Z' name='Transition' Bdg='1BC[09750]' waynum='1' parknum='1' />"
        L"</HemPath>"
        ;

    const wchar_t * text_asoup =
        L"<AsoupEvent create_time='20190114T025500Z' name='Departure' Bdg='0,0[09750]' index='' num='6520' length='24' weight='234' suburbreg='Y' linkBadge='1BC[09750]' linkTime='20190114T024438Z' linkCode='Transition'>"
        L"<![CDATA[<TrainAssignment operation='start' date='06-03-2019 14:55' esr='09750' location='0,0' number='6520'>"
        L"<Unit number='435--2ТЭ116' driver='Ziiins' driverTime='12-02-2019 00:00' weight='234' length='24' specialConditions='M' carrier='22'>"
        L"<Wagon number='100067' axisCount='3' weight='12' length='1' type='1' owner='22' />"
        L"<Wagon number='166688' axisCount='4' weight='23' length='1' type='2' owner='6' />"
        L"<Wagon number='299973' axisCount='3' weight='13' length='1' type='3' owner='23' />"
        L"<Wagon number='219993' axisCount='4' weight='23' length='1' type='3' owner='23' />"
        L"</Unit>"
        L"</TrainAssignment>]]>"
        L"<Locomotive Series='2ТЭ116' NumLoc='435'>"
        L"<Crew EngineDriver='Ziiins' Tim_Beg='2019-02-12 00:00' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        ;
    return std::make_pair(text_happen,text_asoup);
}


std::pair< std::wstring, std::wstring > TC_Hem_aeAdiiasReporter::getHappenAndPvLdzDiesel()
{
    const wchar_t * text_happen =
        L"<HemPath>"
        L"  <SpotEvent create_time='20190114T092803Z' name='Form' Bdg='2C[09511]' waynum='2' parknum='1' index='' num='815' length='3' weight='183' mvps='Y'>"
        L"    <Locomotive Series='ДР1АМ' NumLoc='222' CarrierCode='24'>"
        L"      <Crew EngineDriver='Mazurēvičs' Tim_Beg='2019-01-13 12:27' PersonMode='1' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20190114T092803Z' name='Departure' Bdg='2C[09511]' waynum='2' parknum='1' />"
        L"  <SpotEvent create_time='20190114T094355Z' name='Arrival' Bdg='1C[09520]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190114T094559Z' name='Departure' Bdg='1C[09520]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190114T095105Z' name='Arrival' Bdg='1C[09530]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190114T095303Z' name='Departure' Bdg='1C[09530]' waynum='1' parknum='1' optCode='09530:09540' />"
        L"  <SpotEvent create_time='20190114T095640Z' name='Arrival' Bdg='1C[09540]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190114T095859Z' name='Departure' Bdg='1C[09540]' waynum='1' parknum='1' optCode='09540:09550' />"
        L"  <SpotEvent create_time='20190114T100341Z' name='Arrival' Bdg='1C[09550]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190114T100554Z' name='Departure' Bdg='1C[09550]' waynum='1' parknum='1' optCode='09550:09570' />"
        L"  <SpotEvent create_time='20190114T101723Z' name='Arrival' Bdg='1C[09570]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190114T101932Z' name='Departure' Bdg='1C[09570]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190114T102224Z' name='Arrival' Bdg='1C[09580]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190114T102425Z' name='Departure' Bdg='1C[09580]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190114T102741Z' name='Arrival' Bdg='1C[09500]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190114T102958Z' name='Departure' Bdg='1C[09500]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190114T103451Z' name='Arrival' Bdg='11C[09010]' waynum='11' parknum='1' />"
        L"  <SpotEvent create_time='20190114T103451Z' name='Disform' Bdg='11C[09010]' waynum='11' parknum='1' />"
        L"</HemPath>"
        ;

    const wchar_t * text_asoup =
        L"<AsoupEvent create_time='20190114T092700Z' name='Departure' Bdg='24.855541,57.153164[09511]' index='' num='815' length='3' weight='183' mvps='Y' linkBadge='2C[09511]' linkTime='20190114T092803Z' linkCode='Form'>"
        L"<![CDATA[<TrainAssignment operation='5' date='14-01-2019 12:27' esr='09511' location='24.855541,57.153164' number='815' issue_moment='20190114T094003Z'>"
        L"<Unit number='222-3--DR1AM' driver='Mazurēvičs' driverTime='14-01-2019 12:27' weight='183' length='3' specialConditions='M' carrier='24'>"
        L"<Wagon number='222-3' axisCount='4' weight='71' length='1' type='2' owner='24' />"
        L"<Wagon number='222-8' axisCount='4' weight='54' length='1' type='3' owner='24' />"
        L"<Wagon number='222-4' axisCount='4' weight='58' length='1' type='1' owner='24' />"
        L"</Unit>"
        L"</TrainAssignment>"
        L"]]><Locomotive NumLoc='222'>"
        L"<Crew EngineDriver='Mazurēvičs' Tim_Beg='2019-01-14 12:27' PersonMode='1' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        ;
    return std::make_pair(text_happen,text_asoup);
}
