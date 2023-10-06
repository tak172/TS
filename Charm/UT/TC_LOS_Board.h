#ifndef TC_LOS_BOARD_H
#define TC_LOS_BOARD_H

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/TrainDescr.h"

/* 
 * Проверка класса LOS_Board
 */
class LOS_base;

class TC_LOS_Board : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_LOS_Board );
  CPPUNIT_TEST( item );
  CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
protected:
    void item();
private:
    struct testData
    {
        TrainDescr traits;
        std::wstring evenCapacity;
		std::wstring oddCapacity;
        bool used;
        bool lock;
        bool passenger;
    };
    std::vector<testData> d;
	std::vector <std::vector<const LOS_base*> > ways;
	std::map <const LOS_base *, TrainDescr> losToTDescr;
};


#endif // TC_LOS_BOARD_H