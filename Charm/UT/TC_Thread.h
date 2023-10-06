/*!
	created:	2016/12/21
	created: 	16:57  21 Декабрь 2016
	filename: 	D:\potap\Charm\UT\TC_Thread.h
	file path:	D:\potap\Charm\UT
	file base:	TC_Thread
	file ext:	h
	author:		 Dremin
	
	purpose:	тест событий/нитей
 */
#ifndef TC_Thread_h__
#define TC_Thread_h__
#include <cppunit/extensions/HelperMacros.h>
#include "../Augur/DefType.h"
class TestAAdmin;
struct NsiBasis;
struct SData;

class TC_AThread : 
	public CPPUNIT_NS::TestFixture
{
	void tstTRACE(const NsiBasis& bs,bool full=false);
	void tstTRACE(const TestAAdmin& adm, bool full=false);
	void tstTRACEtrn(const TestAAdmin& adm );
	void testSemiLock_(bool dbl);
	static void outTRACE(std::string t);
	static void outTRACE(std::wstring t);
private:
	CPPUNIT_TEST_SUITE( TC_AThread );
	CPPUNIT_TEST(testTopStart);// учет Тор при отправлении
	CPPUNIT_TEST(testPassOutrun);// задание для обгона при попутном обгоне
	CPPUNIT_TEST(testAdjstFixSpanPassTrn);// согласование фикс.события на перегоне при окне на пути и попутном ПЕ
	CPPUNIT_TEST(testOpposedStopRoute);// ПЕ враждебные маршруты со стоянкой
	CPPUNIT_TEST(testOpposedRoute);// ПЕ используют один и тот же путь
	CPPUNIT_TEST(testStopStationToOutrun);// тест подбора станции торможения при задании на обгон
	CPPUNIT_TEST(testAccordPath);// тест подбора пути на станции
	CPPUNIT_TEST(testOpposedDisblPath);// запрет пути при учете вражд.маршрутов
	CPPUNIT_TEST(testManagePath);//менеджер распределения путей на станции (станц.диспетчер)
	CPPUNIT_TEST(testStationPathEndNorm);//тест  c подбором пути на станции в конце маршрута по нормативке
	CPPUNIT_TEST(testResortTrn);// поиск обгонов попутных ПЕ
	CPPUNIT_TEST(testGenerateIntrusion);// генерация заданий по обгонам при неразрешимом конфликте с путями на станции
	CPPUNIT_TEST(testManyBackAdjust) ;// множественное  согласование ПЕ с другой и Третьей Пе с первой
	CPPUNIT_TEST(testMorthStopMove);// тест превращения стоянки в проследование (борьба с жуком)
	CPPUNIT_TEST(testTnTsk);// учет станц.интервалов
	CPPUNIT_TEST(testWndViaOpposed);// учет окна на перегоне при проверки вреждебнности маршрутов
	CPPUNIT_TEST(testModifyFrstStep);// тест изменения дотягивания с перегона 
	CPPUNIT_TEST(testTopMove);// конфликт по Top при невозможности стоянки на станции
	CPPUNIT_TEST( testAdvEvent);//линковка c разными типами подходов
	CPPUNIT_TEST(testLnkRealErrorPvx);// линковка при влиянии реальной нити с нарушением ПВХ
	CPPUNIT_TEST(testLinkFixFrc);// тест склейки фикс. и расчетного прогнозов
	CPPUNIT_TEST(testReLinkFixSpan);// перелинковка  обратного дотягивания с перегона
	CPPUNIT_TEST(testLnkFixArrival);// линковка фикс.прибытия
	CPPUNIT_TEST(testFixFrcLine);// построение прогноза с ниткой фиксированного прогноза
	CPPUNIT_TEST(testAdjstStatFixWarning);// согласование при фиксации на станции около перегона с предупреждением
	CPPUNIT_TEST(testAdjstFixSpanOnWnd);// согласование фикс.события на перегоне при окне на пути
	CPPUNIT_TEST(testLnkFixDepart);// фиксация на перегоне перед ст. и фикс.отправления с нее
	CPPUNIT_TEST(testAdjstFixWarning);// согласование при фиксации на перегоне с предупреждением
	CPPUNIT_TEST(testFixLnk);// фиксация и путь после нее
	CPPUNIT_TEST(testBackAdjustFixMoveStop);// обратное согласование при изменении фикс проследования на стоянку
	CPPUNIT_TEST(testAdjstFixStopViaStart);// согласование фикс на перегоне и фикс. прибытия/отправления
	CPPUNIT_TEST(testAdjstFixStopViaSpan);// согласование фикс. прибытия и фикс на перегоне +прибытие
	CPPUNIT_TEST( testAdjustIntrusion);// согласование при конфликтах впереди
	CPPUNIT_TEST(testPackViaPack); // тест разрешения на пропуск пакета пакетом
	CPPUNIT_TEST(testIntrusionPackNoBan);
	CPPUNIT_TEST(testBanIntrusion);//запрет при задании по обгону пакета c вставкой высокоприор.ПЕ
	CPPUNIT_TEST(testBackAdjustIntrusion);// обратное согласование интрузии 
	CPPUNIT_TEST(testIntrusionNoRoot);// тест интрузии с выполнением не на корневой станции
	CPPUNIT_TEST( testConflCounterViaDiapBreakService);// проверка разрешения конфликта встречных с проверкой времени проверки тормозов
	CPPUNIT_TEST(testRelinkNormTech);// тест пересвязывания нормативки на тех.станции с разрешением конфликта
	CPPUNIT_TEST(testIntrusionPack);// задания по обгону пакета
	CPPUNIT_TEST(testBlockPost);// тест На блокпостах необходимо прогнозировать только безостановочный пропуск (проследование).
	CPPUNIT_TEST(testBackAdjustSpan);// обратное  согласование при морфинге на перегоне
	CPPUNIT_TEST(testAbsenseStationPath);// отсутствие пути для стоянки
	CPPUNIT_TEST(testHardClashPassWnd);// решение тяжелых исключений c попутными при окне на перегоне
	CPPUNIT_TEST(testHardClashWnd);// решение тяжелых исключений  при окне на перегоне
	CPPUNIT_TEST(testManyHardClashWnd);// решение тяжелого исключения при множественности конфликтов и окне на перегоне
	CPPUNIT_TEST(testMisOutrun);// возможная потеря обгонов из-за нехватки путей на станции
	CPPUNIT_TEST(testAdvFrc);// построение прогноза с событиями подхода
	CPPUNIT_TEST(testFixTechSt);// фиксация прогноза на тех.станции
	CPPUNIT_TEST(testOutrunInfo);// тест сбора информации по обгонам
	CPPUNIT_TEST(testStopOnSpan); // тест стоянки на перегоне
	CPPUNIT_TEST(testHardClash);// решение тяжелых исключений  c менеджером путей
	CPPUNIT_TEST(testBackAdjust);// тест обратной коррекции после конфликта
	CPPUNIT_TEST(testConflCounterTechSt);// конфликт с встречным от тех.станции
	CPPUNIT_TEST(testFixDepartMove);// фиксация отправления в конфликте со встречным
	CPPUNIT_TEST(testFndPVX);// подбор ПВХ для разных типов ПЕ
	CPPUNIT_TEST(testRejectArtifact);// тест уборки устаревших артефактов
	CPPUNIT_TEST(testAdvantageBrutto);// тест на преимущество груженых грузовых
	CPPUNIT_TEST(testInflConfl);//конфликты с взаимовлиянием (ликвидируем  попутный обгон при решении встречного конфликта)
	CPPUNIT_TEST(testOutrunFltrTask);// тест фильтра для задания по обгонам
	CPPUNIT_TEST(testNormNormStrong);// учет интервалов между ПЕ. Обе строго по нормативке
	CPPUNIT_TEST(testNormNormInterval);// учет интервалов между ПЕ. Обе по нормативке
	CPPUNIT_TEST(testWaitlock);
	CPPUNIT_TEST(testCatchNorm);// догоняем нормативку
	CPPUNIT_TEST(testFixFrcConfl);// построение прогноза с кусками фиксированного прогноза и конфликтом при линковке с фикс.прогнозом
	CPPUNIT_TEST(testOutrunTechStation);// обгон попутных на тех.станции при отборе нормативки
	CPPUNIT_TEST(testELECTRIC_TOKENbndWndNorm); //электрожезловая блокировка и ПЕ на границе окна c нормат.стоянкой
	CPPUNIT_TEST(testCounterTELEPHONE_CONNECTION); //телефонные средства связи для встречных
	CPPUNIT_TEST(testBIDIR_AUTOBLOCK_WITH_LOCOSEMAS);//двухсторонняя автоблокировка с движением по сигналам локомотивных светофоров
	CPPUNIT_TEST(testPartWindow);// пересечение границы окна
	CPPUNIT_TEST(testCrossBndWnd);// пересечение границы окна( измененние средств связи)
	CPPUNIT_TEST(testELECTRIC_TOKEN); //электрожезловая блокировка для попутных
	CPPUNIT_TEST(testFixFrc);// построение прогноза с кусками фиксированного прогноза
	CPPUNIT_TEST(testTELEPHONE_CONNECTION); //телефонные средства связи для попутных
 	CPPUNIT_TEST(testExcepToOutrun);// перевод недопустимого занятия пути в задание для обгона
	CPPUNIT_TEST(testCounterELECTRIC_TOKEN); //электрожезловая блокировка для встречных
	CPPUNIT_TEST(testCounterBIDIR_AUTOBLOCK_WITH_LOCOSEMAS);//двухсторонняя автоблокировка с движением по сигналам локомотивных светофоров
	CPPUNIT_TEST(testIntrusionYieldMany);//много заданий по отправлению после другого ПЕ
	CPPUNIT_TEST(testWarnOnSpanFix);// тест возникновения исключений на перегоне при фиксации
	CPPUNIT_TEST(testELECTRIC_TOKENbndWnd); //электрожезловая блокировка и ПЕ на границе окна
	CPPUNIT_TEST(testConflInWindow);//конфликт в окне с проездом по непр.пути
	CPPUNIT_TEST(testAutoBlockDiffSpeed);// отправление ПЕ с разной скоростью при автобокировке 
	CPPUNIT_TEST(testAutoBlockPack);// отправление пакета при автобокировке 
	CPPUNIT_TEST(testConflStat);// задержка из-за занятости путей на станции (вынужденная стоянка на пред.станции)
	CPPUNIT_TEST(testFndNormOccupy);// поиск требуемой нормативки для отправления со станции c захватом для ПЕ
	CPPUNIT_TEST(testIntrusionShouldYieldForOtherCounter);
	CPPUNIT_TEST(testStationPath);//тест подбора путей на станции  для событий с ПЕ
	CPPUNIT_TEST(testFirstStepConfl);//тест первый шаг прогноза c конфликтом по прибытию
	CPPUNIT_TEST(testMorphFrcReal);//превращение событий в прогнозе в зависимости от соседней реальной нити 
	CPPUNIT_TEST(testMorphFrcFrc);//превращение событий в прогнозе в зависимости от соседней прогнозной нити 
	CPPUNIT_TEST(testMorphPack);//превращение событий в прогнозе в зависимости от соседней прогнозной нити 
	CPPUNIT_TEST(testMorphConflReal);//превращение событий в прогнозе в зависимости от соседней реальной нити при конфликте
	CPPUNIT_TEST(testFixSpeedupPass);// обгон фиксированного участка
	CPPUNIT_TEST(testPackConflNextStation);//конфликт с пакетом решается на др.станции и проверкой попутных обгонов
	
	CPPUNIT_TEST( testWrongPath );// проезд по неправильному пути
	CPPUNIT_TEST( testLimStatPath);// выбор пути для проследования при предупреждении (ограничение скорости) на станц.пути

	CPPUNIT_TEST(testProxySpan);// тест псевдо перегона
	CPPUNIT_TEST(testFiltrPassEquPr);// фильтр обгонов попутных одноприоритетных
	CPPUNIT_TEST(testTrOnSt);//  первый шаг прогноза для ПЕ на станции с ускоренным прибытием в реальной нити
	CPPUNIT_TEST(testTnpRegim);// тест режима учета интервала Тн в случае, если оба поезда прибывают с остановкой, то  можно не соблюдать.
	CPPUNIT_TEST(testWaitCounterPack);// задание на макс.ожидание встречного пакета
	CPPUNIT_TEST( testReductionStInterval);// смягчение (уменьшение ) ст.интервалов при проверке их соблюдения 
	CPPUNIT_TEST(testAllowPackWait); // тест разрешения на пропуск пакетом
	CPPUNIT_TEST(testPassOutrunResort);// тест решени попутных обгонов путем переупорядочения попутных ПЕ
	CPPUNIT_TEST(testResort);//  тест пересортировки списка ПЕ
	CPPUNIT_TEST(testTopInterval);// тест учета Top (Станционный интервал неодновременного отправления и попутного прибытия, для станций, где пропуск по главному пути осуществляется без остановки)
	CPPUNIT_TEST(testDeadlock);// тест клинча
	CPPUNIT_TEST(testIntrusionShouldYieldForOtherPass);// задания по отправлению после другого ПЕ
	CPPUNIT_TEST(testIntrusionCounterCorrection);// задания по отправлению после другого ПЕ c корректировкой
	CPPUNIT_TEST(testAdjustFirstStep);// тест согласования. коррекция дотягивания реальной нити до станции с перегона по полученным результатам прогноза. учет возникшего торможения.
	CPPUNIT_TEST(testPrognosisFeedback);// построить данные для ответа
	CPPUNIT_TEST(testIntrusionTransitStations); // задание по транзитным станциям
	CPPUNIT_TEST(testSkrechArr);// тест не учета интервалов скрещения для ПЕ на станции
	CPPUNIT_TEST(testUpOrderIntrusion);// тест превосходства на станции
	CPPUNIT_TEST(testFixFrcSpan);// тест фиксации ПЕ на перегоне
	CPPUNIT_TEST(testAdvFix);// построение прогноза с событиями подхода и фикс.событиями 
	CPPUNIT_TEST(testWarnOnSpan);// тест возникновения исключений на перегоне
	CPPUNIT_TEST(testFrstStep);// первый шаг - дотягивание до станции с перегона, выдержка стоянки по нормативке на станции
	CPPUNIT_TEST(testSortSecPath);// тест сортировки  путей посекционно 
	CPPUNIT_TEST(testEmptyStationPathStop);//тест  c отсутствием  путей для стоянки на станции 
	CPPUNIT_TEST(testFixOnSpan);// тест настройки фикс.событий появления на перегоне ( путь)
	CPPUNIT_TEST( testFixSpan);// тест настройки фикс.событий на перегоне ( длительность+ путь)
	CPPUNIT_TEST( testDrawThread);// #4809 	Прогнозирование нарисованных нитей
	CPPUNIT_TEST(testAuditArch);// тест ревизии архивов
	CPPUNIT_TEST(testMoveFix);// построение прогноза с фиксированным проследованием 
	CPPUNIT_TEST(testNormOnTechStation);// тест ухода со станций с тех.обслуживанием по нормативке
	CPPUNIT_TEST(testLimNormSpeedupSpan);// тест предупреждений на перегоне (огран.времени) при попытке нагнать нормативку
	CPPUNIT_TEST(testLimWarnSpan);// тест предупреждений на перегоне (огран.времени)
	CPPUNIT_TEST(testFewEvPrepare);// несколько событий на перегоне
	CPPUNIT_TEST(testArch);// тест архивации
	CPPUNIT_TEST(testTrnEmptyInd);// отбор маршрута для ПЕ без индекса
	CPPUNIT_TEST(testOutrunTask);// тест задания на обгоны
	CPPUNIT_TEST(testOutrunUser);// тест запросов пользователя на обгоны
	CPPUNIT_TEST(testProxyStation);// тест псевдо станции
	CPPUNIT_TEST(testFixSimplFrc);// построение простого прогноза с кусками фиксированного прогноза
	CPPUNIT_TEST(testSortCargoTr);// алгоритм сортировки Грузовых ПЕ
	CPPUNIT_TEST(testTypeEvIdealFrc);// типы событий в идеальном прогнозе
	CPPUNIT_TEST(testTechStation);// тест станций с тех.обслуживанием
	CPPUNIT_TEST(testSkrech);// тест учета интервалов скрещения 
	CPPUNIT_TEST(testIdealForTrGoods);// построение идеального прогноза для Грузового ПЕ
	CPPUNIT_TEST(testTrainStation);// тест ПЕ на станции без маршрута
	CPPUNIT_TEST(testConfigNum);// тест разбора конфигурации
	CPPUNIT_TEST(testCurrTime);// тест добавления в реальные нити события с текущим реальным временем 
	CPPUNIT_TEST(testSortTrn); // тест сортировки поездов
	CPPUNIT_TEST(testNormWnd);// обход окна при движении по нормативке
	CPPUNIT_TEST( testFiltr );// фильтрация и сжатие нач.данных
	CPPUNIT_TEST(testOnePath);// проезд по однопутке (окно на перегоне)
	CPPUNIT_TEST(testSemiLock);// тест при ПАБ (полуавтоблокировке)
	CPPUNIT_TEST(testTopology);
	CPPUNIT_TEST_SUITE_END();

	SData & sd;

public:
	TC_AThread();
	void testPassOutrun();// задание для обгона при попутном обгоне
	void testSpanFrstStp();// тест дотягивания ПЕ с перегона
	void testSkrechArr();// тест не учета интервалов скрещения для ПЕ на станции
	void testSortCargoTr();// алгоритм сортировки Грузовых ПЕ
	void testNormWnd();// обход окна при движении по нормативке
	void testFiltr();
	void testTopology();
	void testBldIdeal();// построение идеального прогноза нормативка+ПВХ
	void testPass();
	void testFrstStep();// первый шаг - дотягивание до станции с перегона, выдержка стоянки по нормативке на станции
	void testCatchNorm();// догоняем нормативку
	void testConflStat();// задержка из-за занятости путей на станции (вынужденная стоянка на пред.станции)
	void testOnePath();// проезд по неправ.пути (окно на перегоне)
	void testWrongPath();// проезд по неправильному пути
	void testDeadlock();// тест клинча
	void testWaitlock();//  ожидание занятых путей на станции
	void testSemiLock();// тест при ПАБ (полуавтоблокировке)
	void testEmptyTrain();// тест пустой нити ПЕ
	void testSkrech();// тест учета интервалов скрещения 
	void testOutrunInfo();// тест сбора информации по обгонам
	void testProxyStation();// тест псевдо станции
	void testProxySpan();// тест псевдо перегона
	void testAccordPath();// тест подбора пути на станции
	void testTechStation();// тест станций с тех.обслуживанием
	void testNormOnTechStation();// тест ухода со станций с тех.обслуживанием по нормативке
	void testVarForecast();// тест построения вариантного графика  с вариацией окон
	void testVarForecastCreate();//тест создания вариантного графика без окон
	void testFirstStepWrongWay();//тест первый шаг прогноза по неправ.пути
	void testCurrTime();// тест добавления в реальные нити события с текущим реальным временем 
	void testTopInterval();// тест учета Top (Станционный интервал неодновременного отправления и попутного прибытия, для станций, где пропуск по главному пути осуществляется без остановки)
	void testBlockPost();// тест На блокпостах необходимо прогнозировать только безостановочный пропуск (проследование).
	void testSortTrn();// тест сортировки поездов
	void testStopOnSpan(); // тест стоянки на перегоне
	void testConfigNum();// тест разбора конфигурации
	void testTrainStation();// тест ПЕ на станции без маршрута
	void testIdealForTrGoods();// построение идеального прогноза для Грузового ПЕ
	void testTrnEmptyInd();// отбор маршрута для ПЕ без индекса
	void testNoInfoTrn();//техн.время для типа поезда "Неопределенные" относится ко всем поездам за исключением поездов прогнозируемых по нормативному графику.
	void testTypeEvIdealFrc();// типы событий в идеальном прогнозе
	void testProperNorm();// подбор требуемой нормативки для отправления со станции
	void testFndPVX();// подбор ПВХ для разных типов ПЕ
	void testFndProNorm();// поиск требуемой нормативки для отправления со станции с синонимами
	void testFixSimplFrc();// построение простого прогноза с кусками фиксированного прогноза
	void testFixFrc();// построение прогноза с кусками фиксированного прогноза
	void testFixFrcLine();// построение прогноза с ниткой фиксированного прогноза
	void testFixFrcConfl();// построение прогноза с кусками фиксированного прогноза и конфликтом при линковке с фикс.прогнозом
	void testFixFrcPoss();// прогноз с кусками фиксированного прогноза и конфликтом при занятии путей
	void testOutrunUser();// тест запросов пользователя на обгоны
	void testOutrunTask();// тест задания на обгоны
	void testArch();// тест архивации
	void testFewEvPrepare();// несколько событий на перегоне
	void testFirstStepConfl();//тест первый шаг прогноза c конфликтом по прибытию
	void testTrOnSt();//  первый шаг прогноза для ПЕ на станции с ускоренным прибытием в реальной нити
	void testLimWarnSpan();// тест предупреждений на перегоне (огран.времени)
	void testLimNormSpeedupSpan();// тест предупреждений на перегоне (огран.времени) при попытке нагнать нормативку
	void testStationPath();//тест подбора путей на станции  для событий с ПЕ
	void testOutrunFltrTask();// тест фильтра для задания по обгонам
	void testEmptyStationPathStop();//тест  c отсутствием  путей для стоянки на станции 
	void testStationPathEndNorm();//тест  c подбором пути на станции в конце маршрута по нормативке
	void testFndNormOccupy();// поиск требуемой нормативки для отправления со станции c захватом для ПЕ
	void testExcepToOutrun();// перевод недопустимого занятия пути в задание для обгона
	void testAdvFrc();// построение прогноза с событиями подхода
	void testMoveFix();// построение прогноза с фиксированным проследованием 
	void testAuditArch();// тест ревизии архивов
	void testFixFrcSpan();// тест фиксации ПЕ на перегоне
	void testFixSpan();// тест настройки фикс.событий на перегоне ( длительность+ путь)
	void testDrawThread();// #4809 	Прогнозирование нарисованных нитей
	void testFixOnSpan();// тест настройки фикс.событий появления на перегоне ( путь)
	void testSortSecPath();// тест сортировки  путей посекционно 
	void testLinkFixFrc();// тест склейки фикс. и расчетного прогнозов
	void testWarnOnSpan();// тест возникновения исключений на перегоне
	void testWarnOnSpanFix();// тест возникновения исключений на перегоне при фиксации
	void testWaitCounterPack();// задание на макс.ожидание встречного пакета
	void testAdvFix();// построение прогноза с событиями подхода и фикс.событиями 
	void testStopStationToOutrun();// тест подбора станции торможения при задании на обгон
	void testUpOrderIntrusion();// тест превосходства на станции
	void testFiltrPassEquPr();// фильтр обгонов попутных одноприоритетных
	void testIntrusionShouldYieldForOtherPass();// задания по отправлению после другого ПЕ
	void testIntrusionShouldYieldForOtherCounter();// задания по отправлению после другого ПЕ
	void testIntrusionTransitStations(); // задание по транзитным станциям
	void testIntrusionCounterCorrection();// задания по отправлению после другого ПЕ c корректировкой
	void testPrognosisFeedback();// построить данные для ответа
	void testBackAdjust();// тест обратной коррекции после конфликта
	void testAdjustFirstStep();// тест согласования. коррекция дотягивания реальной нити до станции с перегона по полученным результатам прогноза. учет возникшего торможения.
	void testRejectArtifact();// тест уборки устаревших артефактов
	void testAllowPackWait(); // тест разрешения на пропуск пакетом
	void testResort();//  тест пересортировки списка ПЕ
	void testPassOutrunResort();// тест решения попутных обгонов путем переупорядочения попутных ПЕ
	void testIntrusionPack();// задания по обгону пакета
	void testBanIntrusion();//запрет при задании по обгону пакета c вставкой высокоприор.ПЕ
	void testIntrusionPackNoBan();
	void testOutrunTechStation();// обгон попутных на тех.станции при отборе нормативки
	void testConflCounterViaDiapBreakService();// проверка разрешения конфликта встречных с проверкой времени проверки тормозов
	void testMorphFrcReal();//превращение событий в прогнозе в зависимости от соседней реальной нити 
	void testReductionStInterval();// смягчение (уменьшение ) ст.интервалов при проверке их соблюдения  
	void testMorphFrcFrc();//превращение событий в прогнозе в зависимости от соседней прогнозной нити 
	void testPackViaPack(); // тест разрешения на пропуск пакета пакетом
	void testAdvantageBrutto();// тест на преимущество груженых грузовых
	void testModifyFrstStep();// тест изменения дотягивания с перегона 
	void testRelinkNormTech();// тест пересвязывания нормативки на тех.станции с разрешением конфликта
	void testMorthStopMove();// тест превращения стоянки в проследование (борьба с жуком)
	void testTnpRegim();// тест режима учета интервала Тн в случае, если оба поезда прибывают с остановкой, то  можно не соблюдать.
	void testIntrusionNoRoot();// тест интрузии с выполнением не на корневой станции
	void testMorphConflReal();//превращение событий в прогнозе в зависимости от соседней реальной нити при конфликте 
	void testInflConfl();//конфликты с взаимовлиянием (ликвидируем  попутный обгон при решении встречного конфликта)
	// для попутных
	void testBIDIR_AUTOBLOCK_WITH_LOCOSEMAS();//двухсторонняя автоблокировка с движением по сигналам локомотивных светофоров
	void testELECTRIC_TOKEN(); //электрожезловая блокировка
	void testTELEPHONE_CONNECTION(); //телефонные средства связи
	void testELECTRIC_TOKENbndWnd(); //электрожезловая блокировка и ПЕ на границе окна
	// для встречных
	void testCounterBIDIR_AUTOBLOCK_WITH_LOCOSEMAS();//двухсторонняя автоблокировка с движением по сигналам локомотивных светофоров
	void testCounterELECTRIC_TOKEN(); //электрожезловая блокировка
	void testCounterTELEPHONE_CONNECTION(); //телефонные средства связи
	void testLimStatPath();// выбор пути для проследования при предупреждении (ограничение скорости) на станц.пути
	void testPackConflNextStation();//конфликт с пакетом решается на др.станции и проверкой попутных обгонов
	void testBackAdjustIntrusion();// обратное согласование интрузии 
	void testFixSpeedupPass();// обгон фиксированного участка
	void testMorphPack();//превращение событий в прогнозе в зависимости от соседней прогнозной нити 
	void testFixTechSt();// фиксация прогноза на тех.станции
	void testBackAdjustSpan();// обратное  согласование при морфинге на перегоне
	void testAutoBlockPack();// отправление пакета при автобокировке 
	void testAutoBlockDiffSpeed();// отправление ПЕ с разной скоростью при автобокировке 
	void testIntrusionYieldMany();//много заданий по отправлению после другого ПЕ
	void testConflInWindow();//конфликт в окне с проездом по непр.пути
	void testMisOutrun();// возможная потеря обгонов из-за нехватки путей на станции
	void testPartWindow();// пересечение границы окна
	void testELECTRIC_TOKENbndWndNorm(); //электрожезловая блокировка и ПЕ на границе окна c нормат.стоянкой
	void testFixLnk();// фиксация и путь после нее
	void testLnkFixDepart();// фиксация на перегоне перед ст. и фикс.отправления с нее
	void testFixDepartMove();// фиксация отправления в конфликте со встречным
	void testNormNormInterval();// учет интервалов между ПЕ. Обе по нормативке
	void testOpposedRoute();// ПЕ используют один и тот же путь
	void testOpposedStopRoute();// ПЕ враждебные маршруты со стоянкой
	void testNormNormStrong();// учет интервалов между ПЕ. Обе строго по нормативке
	void testCrossBndWnd();// пересечение границы окна( измененние средств связи)
	void testAdjstFixWarning();// согласование при фиксации на перегоне с предупреждением
	void testAdjstStatFixWarning();// согласование при фиксации на станции около перегона с предупреждением
	void testBackAdjustFixMoveStop();// обратное согласование при изменении фикс проследования на стоянку
	void testAdjustIntrusion();// согласование при конфликтах впереди
	void testTopStart();// учет Тор при отправлении
	void testWndViaOpposed();// учет окна на перегоне при проверки вреждебнности маршрутов
	void testManagePath();//менеджер распределения путей на станции (станц.диспетчер)
	void testConflCounterTechSt();// конфликт с встречным от тех.станции
	void testHardClash();// решение тяжелых исключений  c менеджером путей
	void testTnTsk();// учет станц.интервалов
	void testHardClashWnd();// решение тяжелых исключений  при окне на перегоне
	void testManyHardClashWnd();// решение тяжелого исключения при множественности конфликтов и окне на перегоне
	void testHardClashPassWnd();// решение тяжелых исключений c попутными при окне на перегоне
	void testAbsenseStationPath();// отсутствие пути для стоянки
 	void testResortTrn();// поиск обгонов попутных ПЕ
	void testTopMove();// конфликт по Top при невозможности стоянки на станции
	void testAdjstFixStopViaStart();// согласование фикс на перегоне и фикс. прибытия/отправления
	void testAdjstFixStopViaSpan();// согласование фикс. прибытия и фикс на перегоне +прибытие
	void testAdjstFixSpanOnWnd();// согласование фикс.события на перегоне при окне на пути
	void testLnkFixArrival();// линковка фикс.прибытия
	void testReLinkFixSpan();// перелинковка  обратного дотягивания с перегона
	void testLnkRealErrorPvx();// линковка при влиянии реальной нити с нарушением ПВХ
	void testAdvEvent();//линковка c разными типами подходов
	void testManyBackAdjust() ;// множественное  согласование ПЕ с другой и Третьей Пе с первой
	void testGenerateIntrusion();// генерация заданий по обгонам при неразрешимом конфликте с путями на станции
	void testOpposedDisblPath();// запрет пути при учете вражд.маршрутов
	void testAdjstFixSpanPassTrn();// согласование фикс.события на перегоне при окне на пути и попутном ПЕ
};


#endif // TC_Thread_h__

