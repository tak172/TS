#include "stdafx.h"
#include "TC_TrackerChains.h"
#include "../UT/TestTracker.h"
#include "../Guess/Chain.h"
#include "../Guess/ChainContainer.h"
#include "GuessNsiBasis.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerChains );

TC_TrackerChains::TC_TrackerChains()
{
}

void TC_TrackerChains::setUp()
{
	TI.Reset();
	TrackingConfiguration tconf;
	shared_ptr<const TestGuessNsiBasis> nsiBasisPtr( new TestGuessNsiBasis() );
	TFReverted.reset( new TrainField( tconf.defSearchPathDepth, nsiBasisPtr ) );
	TI.CreateStation( *TFReverted, false );
}

void TC_TrackerChains::tearDown()
{
	HemEventRegistry::Shutdowner();
}

void TC_TrackerChains::AppearanceScenario()
{
	const auto & chacont = tracker->GetChainContainer();
	CPPUNIT_ASSERT( nullptr == chacont.GetConstChain( TI.Get( L"A" ).bdg ) );
	
	{
		TI.DetermineStripBusy( L"A", *tracker );
		BadgeE test_chains[] = { TI.Get(L"A").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}

	{
		TI.DetermineStripBusy( L"C", *tracker );
		BadgeE test_chains[] = { TI.Get(L"A").bdg, BadgeE(), TI.Get(L"C").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}

	{
		TI.DetermineSwitchMinus( L"11", *tracker );
		TI.DetermineStripBusy( L"D", *tracker );
		BadgeE test_chains[] = { TI.Get(L"A").bdg, BadgeE(), TI.Get(L"C").bdg, BadgeE(), TI.Get(L"D").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChains::MergingScenario()
{
	const auto & chacont = tracker->GetChainContainer();
	CPPUNIT_ASSERT( nullptr == chacont.GetConstChain( TI.Get( L"C" ).bdg ) );

	{
		TI.DetermineStripBusy( L"C", *tracker );
		TI.DetermineStripBusy( L"F", *tracker );
		BadgeE test_chains[] = { TI.Get(L"C").bdg, TI.Get(L"F").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}

	{
		TI.DetermineStripBusy( L"G", *tracker );
		BadgeE test_chains[] = { TI.Get(L"C").bdg, TI.Get(L"F").bdg, BadgeE(), TI.Get(L"G").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}

	{
		TI.DetermineSwitchPlus( L"13", *tracker );
		BadgeE test_chains[] = { TI.Get(L"C").bdg, TI.Get(L"F").bdg, TI.Get(L"G").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}

	{
		TI.DetermineStripBusy( L"A", *tracker );
		BadgeE test_chains[] = { TI.Get(L"A").bdg, BadgeE(), TI.Get(L"C").bdg, TI.Get(L"F").bdg, TI.Get(L"G").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}

	{
		TI.DetermineStripBusy( L"B", *tracker );
		TI.DetermineSwitchMinus( L"11", *tracker );
		BadgeE test_chains[] = { TI.Get(L"A").bdg, TI.Get(L"B").bdg, TI.Get(L"C").bdg, TI.Get(L"F").bdg, TI.Get(L"G").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChains::LongMergingScenario()
{
	const auto & chacont = tracker->GetChainContainer();
	CPPUNIT_ASSERT( nullptr == chacont.GetConstChain( TI.Get( L"Q" ).bdg ) );

	{
		TI.DetermineStripBusy( L"Q", *tracker );
		TI.DetermineStripBusy( L"A", *tracker );
		BadgeE test_chains[] = { TI.Get(L"Q").bdg, TI.Get(L"A").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}

	TI.DetermineSwitchMinus( L"11", *tracker );

	{
		TI.DetermineStripBusy( L"C", *tracker );
		TI.DetermineStripBusy( L"F", *tracker );
		BadgeE test_chains[] = { TI.Get(L"Q").bdg, TI.Get(L"A").bdg, BadgeE(), TI.Get(L"C").bdg, TI.Get(L"F").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}

	{
		TI.DetermineStripBusy( L"B", *tracker );
		BadgeE test_chains[] = { TI.Get(L"Q").bdg, TI.Get(L"A").bdg, TI.Get(L"B").bdg, TI.Get(L"C").bdg, TI.Get(L"F").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChains::SpanToStatScenario()
{
	const ChainContainer & BChains = tracker->GetChainContainer();
	CPPUNIT_ASSERT( nullptr == BChains.GetConstChain( TI.Get( L"Q" ).bdg ) );
	CPPUNIT_ASSERT( nullptr == BChains.GetConstChain( TI.Get( L"A" ).bdg ) );

	{
		TI.DetermineStripBusy( L"Q", *tracker );
		TI.DetermineStripBusy( L"A", *tracker );
		BadgeE test_chains[] = { TI.Get(L"Q").bdg, TI.Get(L"A").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}

	{
		TI.DetermineStripFree( L"Q", *tracker );
		BadgeE test_chains[] = { TI.Get(L"A").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChains::SplittingScenario()
{
	unique_ptr<TestTracker> loctracker;
	auto TF = TI.TField();
	TrainFieldPtr TFArr[] = { TF, TFReverted };
	for( unsigned int k = 0; k < size_array( TFArr ); ++k )
	{
		loctracker.reset( new TestTracker( TFArr[k], TrackingConfiguration() ) );
		loctracker->Reset();
		const ChainContainer & BChains = loctracker->GetChainContainer();
		CPPUNIT_ASSERT( nullptr == BChains.GetConstChain( TI.Get( L"A" ).bdg ) );

		{
			TI.DetermineStripBusy( L"A", *tracker );
			TI.DetermineStripBusy( L"B", *tracker );
			TI.DetermineStripBusy( L"C", *tracker );
			TI.DetermineStripBusy( L"F", *tracker );
			TI.DetermineStripBusy( L"G", *tracker );
			TI.DetermineSwitchMinus( L"11", *tracker );
			TI.DetermineSwitchPlus( L"13", *tracker );
			BadgeE test_chains[] = { TI.Get(L"A").bdg, TI.Get(L"B").bdg, TI.Get(L"C").bdg, TI.Get(L"F").bdg, TI.Get(L"G").bdg, BadgeE() };
			CheckUniqueExistence( test_chains, size_array( test_chains ) );
		}

		{
			TI.DetermineStripFree( L"B", *tracker );
			BadgeE test_chains[] = { TI.Get(L"A").bdg, BadgeE(), TI.Get(L"C").bdg, TI.Get(L"F").bdg, TI.Get(L"G").bdg, BadgeE() };
			CheckUniqueExistence( test_chains, size_array( test_chains ) );
		}

		{
			TI.DetermineSwitchMinus( L"13", *tracker );
			BadgeE test_chains[] = { TI.Get(L"A").bdg, BadgeE(), TI.Get(L"C").bdg, TI.Get(L"F").bdg, BadgeE(), TI.Get(L"G").bdg, BadgeE() };
			CheckUniqueExistence( test_chains, size_array( test_chains ) );
		}
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	}
}

void TC_TrackerChains::TripleSwitchScenario()
{
	const ChainContainer & BChains = tracker->GetChainContainer();
	CPPUNIT_ASSERT( nullptr == BChains.GetConstChain( TI.Get( L"A2" ).bdg ) );

	{
		TI.DetermineStripBusy( L"A2", *tracker );
		TI.DetermineStripBusy( L"B2", *tracker );
		TI.DetermineStripBusy( L"G2", *tracker );
		TI.DetermineSwitchMinus( L"24", *tracker );
		BadgeE test_chains[] = { TI.Get(L"A2").bdg, TI.Get(L"B2").bdg, TI.Get(L"G2").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}

	{
		TI.DetermineStripBusy( L"H2", *tracker );
		TI.DetermineStripBusy( L"K2", *tracker );
		TI.DetermineSwitchMinus( L"22", *tracker );
		TI.DetermineSwitchMinus( L"23", *tracker );
		TI.DetermineStripFree( L"D2", *tracker );
		TI.DetermineStripBusy( L"E2", *tracker );
		BadgeE test_chains[] = { TI.Get(L"A2").bdg, TI.Get(L"B2").bdg, TI.Get(L"G2").bdg, TI.Get(L"H2").bdg, TI.Get(L"E2").bdg, TI.Get(L"K2").bdg, BadgeE() };
		CheckUniqueExistence( test_chains, size_array( test_chains ) );
	}

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChains::SwitchToggling()
{
	//ЦЗ: G2-H2-E2-F2
	TI.DetermineSwitchMinus( L"22", *tracker, false );
	TI.DetermineSwitchPlus( L"23", *tracker, false );
	TI.DetermineStripBusy( L"G2", *tracker, false );
	TI.DetermineStripBusy( L"H2", *tracker, false );
	TI.DetermineStripBusy( L"E2", *tracker, false );
	TI.DetermineStripBusy( L"F2", *tracker, false );
	TI.FlushData( *tracker, false );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );

	//переключение стрелки в "+" и измененеие прилегающих участков
	TI.DetermineStripBusy( L"M2", *tracker, false );
	TI.DetermineStripBusy( L"D2", *tracker, false );
	TI.DetermineStripBusy( L"L2", *tracker, false );
	TI.DetermineSwitchPlus( L"22", *tracker, false );
	TI.DetermineStripFree( L"G2", *tracker, false );
	TI.DetermineStripFree( L"H2", *tracker, false );
	TI.FlushData( *tracker, false );

	const ChainContainer & BChains = tracker->GetChainContainer();
	const EsrKit & m2Ecode = TI.Get( L"M2" ).bdg.num();
	BadgeE test_chains[] = { TI.Get(L"M2").bdg, TI.Get(L"D2").bdg, TI.Get(L"E2").bdg, TI.Get(L"F2").bdg, TI.Get(L"L2").bdg, BadgeE() };
	CheckUniqueExistence( test_chains, size_array( test_chains ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChains::CheckUniqueExistence( BadgeE badge_chain[], unsigned int sz/*, const TestTracker & trackerRef*/ ) const
{
	//правило формирования массива badge_chain:
	//{ бэйдж 1 эл-та 1 -й цепочки, бейдж 2-го эл-та 1-й цепочки .. бейдж n-го эл-та 1-й цепочки, ПУСТОЙ бейдж,
	//	 бейдж 1 эл-та 2-й цепочки, бейдж 2-го эл-та 2-й цепочки .. бейдж m-го эл-та 2-й цепочки, ПУСТОЙ бейдж и т.д. }
	// size_array = общее число элементов массива

	const auto & chacont = tracker->GetChainContainer();
	set<ChainCPtr> busy_chains;
	for ( unsigned int k = 0; k < sz; ++k )
	{
		const BadgeE & bdg = badge_chain[k];
		if ( !bdg.empty() )
        {
            auto temp = chacont.GetConstChain( bdg );
            busy_chains.insert( temp );
        }
	}

	//перекладываем для удобства
	list<vector<BadgeE>> busy_badges;
	for ( auto it = busy_chains.cbegin(); it != busy_chains.cend(); ++it )
	{
		vector<BadgeE> temp_vec;
		const auto & badge_line = ( *it )->GetBadges();
		for ( auto bit = badge_line.cbegin(); bit != badge_line.cend(); ++bit )
			temp_vec.push_back( *bit );
		busy_badges.push_back( temp_vec );
	}

	//проверяем
	list<vector<BadgeE>> chains_to_check;
	vector<BadgeE> temp_vec;
	for ( unsigned int k = 0; k < sz; ++k )
	{
		if ( badge_chain[k].empty() )
		{
			chains_to_check.push_back( temp_vec );
			temp_vec.clear();
		}
		else
			temp_vec.push_back( badge_chain[k] );
	}
	CPPUNIT_ASSERT( busy_badges.size() == chains_to_check.size() );

	//нормируем цепочки (разворачиваем так, чтобы начинались с меньшего элемента)
	for ( auto ccit = chains_to_check.begin(); ccit != chains_to_check.end(); ++ccit )
	{
		if ( ccit->front().str() > ccit->back().str() )
			reverse( ccit->begin(), ccit->end() );
	}
	for ( auto bcit = busy_badges.begin(); bcit != busy_badges.end(); ++bcit )
	{
		if ( bcit->front().str() > bcit->back().str() )
			reverse( bcit->begin(), bcit->end() );

		//порядок цепочек не важен
		CPPUNIT_ASSERT( find( chains_to_check.begin(), chains_to_check.end(), *bcit ) != chains_to_check.end() );
	}
}

void TC_TrackerChains::IncorrectChainsMergingOnForcedLocation()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	LocateUnityByForce( nullptr, list<wstring>( 1, L"A" ) );
	LocateUnityByForce( nullptr, list<wstring>( 1, L"D" ) );
	const auto & chacont = tracker->GetChainContainer();
	auto chainA = chacont.GetConstChain( TI.Get( L"A" ).bdg );
    auto chainD = chacont.GetConstChain( TI.Get( L"D" ).bdg );
    CPPUNIT_ASSERT( nullptr != chainA ); //ЦЗ есть
    CPPUNIT_ASSERT( nullptr != chainD ); //ЦЗ есть
    CPPUNIT_ASSERT( chainD != chainA ); //ЦЗ не сливаются
}