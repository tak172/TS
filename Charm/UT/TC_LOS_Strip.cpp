#include "stdafx.h"

#include <sstream>
#include "TC_LOS_Strip.h"
#include "../Actor/LOSBase.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LOS_Strip );

void TC_LOS_Strip::signals()
{
    boost::scoped_ptr<LOS_base> way( LOS_base::fabric_from_object(STRIP) );

    way->setUsed(   StateLTS::PASSIVE);    
    way->setArtUsed(StateLTS::PASSIVE); 
    way->setLock(   StateLTS::PASSIVE);
    way->setArtLock(StateLTS::PASSIVE);
    CPPUNIT_ASSERT( !way->isStableUsed() );
    CPPUNIT_ASSERT( !way->isDirectUsed() );
    CPPUNIT_ASSERT( !way->isArtUsed()    );
    CPPUNIT_ASSERT( !way->isLock()       );
    CPPUNIT_ASSERT( !way->isArtLock()    );

    way->setUsed(   StateLTS::ACTIVE);
    way->setArtUsed(StateLTS::PASSIVE);
    CPPUNIT_ASSERT( way->isStableUsed()  );
    CPPUNIT_ASSERT( way->isDirectUsed() );
    CPPUNIT_ASSERT( !way->isArtUsed()    );

    way->setUsed(   StateLTS::ACTIVE);
    way->setArtUsed(StateLTS::ACTIVE);
    CPPUNIT_ASSERT( !way->isStableUsed()  );
    CPPUNIT_ASSERT( way->isDirectUsed() );
    CPPUNIT_ASSERT( way->isArtUsed()    );

    way->setUsed(   StateLTS::PASSIVE);
    way->setArtUsed(StateLTS::ACTIVE);
    CPPUNIT_ASSERT( !way->isStableUsed()  );
    CPPUNIT_ASSERT( !way->isDirectUsed() );
    CPPUNIT_ASSERT( way->isArtUsed()    );

    // добавка из-за Шарьи - разделка занятого = постоянная занятость + мигающее замыкание
    way->setUsed(   StateLTS::ACTIVE );
    way->setArtUsed(StateLTS::PASSIVE);
    way->setArtLock(StateLTS::ACTIVE );
    CPPUNIT_ASSERT( way->isStableUsed()  );
    CPPUNIT_ASSERT( way->isDirectUsed() );
    CPPUNIT_ASSERT( way->isArtUsed()    );
}
