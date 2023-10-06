#ifndef TC_XML_LOS_H
#define TC_XML_LOS_H

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/Attic.h"

class LOS_base;

class TC_xml_LOS : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_xml_LOS );

  CPPUNIT_TEST( fields_Board );
  CPPUNIT_TEST( fields_Landscape );
  CPPUNIT_TEST( fields_Commutator );
  CPPUNIT_TEST( fields_Figure );
  CPPUNIT_TEST( fields_Head );
  CPPUNIT_TEST( fields_HtmlText );
  CPPUNIT_TEST( fields_Strip );
  CPPUNIT_TEST( fields_Switch );
  CPPUNIT_TEST( fields_Text );
  CPPUNIT_TEST( fields_Void );
  CPPUNIT_TEST( fields_WinText );

  CPPUNIT_TEST( signals_Board );
  CPPUNIT_TEST( signals_Landscape );
  CPPUNIT_TEST( signals_Commutator );
  CPPUNIT_TEST( signals_Figure );
  CPPUNIT_TEST( signals_Head );
  CPPUNIT_TEST( signals_HtmlText );
  CPPUNIT_TEST( signals_Strip );
  CPPUNIT_TEST( signals_Switch );
  CPPUNIT_TEST( signals_Text );
  CPPUNIT_TEST( signals_Void );
  CPPUNIT_TEST( signals_WinText );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();
protected:
    void fields_Board();
    void fields_Landscape();
    void fields_Commutator();
    void fields_Figure();
    void fields_Head();
    void fields_HtmlText();
    void fields_Strip();
    void fields_Switch();
    void fields_Text();
    void fields_Void();
    void fields_WinText();

    void signals_Board();
    void signals_Landscape();
    void signals_Commutator();
    void signals_Figure();
    void signals_Head();
    void signals_HtmlText();
    void signals_Strip();
    void signals_Switch();
    void signals_Void();
    void signals_Text();
    void signals_WinText();

private:
    bool chk( boost::scoped_ptr<LOS_base>& one, boost::scoped_ptr<LOS_base>& two );

    attic::a_document doc;
    attic::a_node     node;
};


#endif // TC_XML_LOS_H