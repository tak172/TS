#ifndef TC_HEM_BILL_H
#define TC_HEM_BILL_H

#include <cppunit/extensions/HelperMacros.h>

// Тест класса Bill
class TC_HemBill : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_HemBill );
    CPPUNIT_TEST( LoadCut );
    CPPUNIT_TEST( LoadAttach );
    CPPUNIT_TEST( LoadAttach_AsoupHappen );
    CPPUNIT_TEST( LoadAttach_HappenAsoup );
    CPPUNIT_TEST( LoadAttach_AsoupAsoup );
    CPPUNIT_TEST( LoadCreate );
    CPPUNIT_TEST( LoadRemove );
    CPPUNIT_TEST( LoadIdentifyRequests );
    CPPUNIT_TEST( CheckTailAndHead );
    CPPUNIT_TEST( FutureMoveOrder );
//	CPPUNIT_TEST( FutureMoveNonstop );
    CPPUNIT_TEST( FutureMoveDefault );
	CPPUNIT_TEST_SUITE_END();

protected:
    void LoadCut();
    void LoadAttach();
    void LoadAttach_AsoupHappen();
    void LoadAttach_HappenAsoup();
    void LoadAttach_AsoupAsoup();
    void LoadCreate();
    void LoadRemove();
    void CheckTailAndHead();
    void LoadIdentifyRequests();
    void FutureMoveOrder();
	void FutureMoveNonstop();
    void FutureMoveDefault();
};

#endif // TC_HEM_BILL_H