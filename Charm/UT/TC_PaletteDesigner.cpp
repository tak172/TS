#include "stdafx.h"

#include <sstream>
#include "TC_PaletteDesigner.h"
#include "../helpful/Attic.h"
#include "../Actor/PaletteDesigner.h"
#include "../StatBuilder/ColorGamut.h"
#include "../Actor/Blinker.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PaletteDesigner );

using namespace std;
using namespace boost::posix_time;


void TC_PaletteDesigner::Load()
{
     const wstring palette_txt = 
         L"<Test>"
         L"<PaletteConfig>"
         L"<Color srv_main='FF000000h' />"
         L"<Color srv_bkgr='FFBFBFBFh' />"
         L"<Color comm_arrow_asgn_train='FF00FF00h' />"
         L"<Color comm_arrow_asgn_mnvr='FFFFFF00h' />"
         L"<Color comm_arrow_used='FFFF0000h' />"
         L"</PaletteConfig>"
         L"</Test>";
 
     attic::a_document doc;
     attic::a_parse_result res = doc.load_wide( palette_txt );
     CPPUNIT_ASSERT( res==true );
     const std::wstring before = doc.pretty_wstr();
 
     PaletteDesigner pd( color_gamut );
     pd.Load( doc.child("Test") );
 
     Gdiplus::Color color;

     CPPUNIT_ASSERT( color_gamut->GetServiceColor( ColorDescr::BASE, color) &&
                    color.GetValue() == Gdiplus::Color(0xFF, 0, 0, 0).GetValue() );

     CPPUNIT_ASSERT( color_gamut->GetServiceColor( ColorDescr::BKGR, color) &&
         color.GetValue() == Gdiplus::Color(0xFF, 0xBF, 0xBF, 0xBF).GetValue() );

     color = color_gamut->GetCommArrowColor( false, true );
     CPPUNIT_ASSERT( color.GetValue() == Gdiplus::Color(0xFF, 0x00, 0xFF, 0x00).GetValue() );
    
     color = color_gamut->GetCommArrowColor( false, false );
     CPPUNIT_ASSERT( color.GetValue() == Gdiplus::Color(0xFF, 0xFF, 0xFF, 0x00).GetValue() );

     color = color_gamut->GetCommArrowColor( true, false );
     CPPUNIT_ASSERT( color.GetValue() == Gdiplus::Color(0xFF, 0xFF, 0x00, 0x00).GetValue() );

     doc.reset();

      const wstring add_txt = 
          L"<Test>"
          L"<PaletteConfig>"
          L"<Color srv_main='FFF00000h' />"
          L"<Color comm_arrow_asgn_train='FF00F000h' />"
          L"<Color trn_0_998='FFFF8080h' />"
          L"</PaletteConfig>"
          L"</Test>";
 
      res = doc.load_wide( add_txt );
      CPPUNIT_ASSERT( res==true );
      pd.Load( doc.child("Test") );

      CPPUNIT_ASSERT( color_gamut->GetServiceColor( ColorDescr::BASE, color) &&
          color.GetValue() == Gdiplus::Color(0xFF, 0xF0, 0, 0).GetValue() );

      CPPUNIT_ASSERT( color_gamut->GetServiceColor( ColorDescr::BKGR, color) &&
          color.GetValue() == Gdiplus::Color(0xFF, 0xBF, 0xBF, 0xBF).GetValue() );

      color = color_gamut->GetCommArrowColor( false, true );
      CPPUNIT_ASSERT( color.GetValue() == Gdiplus::Color(0xFF, 0x00, 0xF0, 0x00).GetValue() );

      color = color_gamut->GetCommArrowColor( false, false );
      CPPUNIT_ASSERT( color.GetValue() == Gdiplus::Color(0xFF, 0xFF, 0xFF, 0x00).GetValue() );

      color = color_gamut->GetCommArrowColor( true, false );
      CPPUNIT_ASSERT( color.GetValue() == Gdiplus::Color(0xFF, 0xFF, 0x00, 0x00).GetValue() );

      color = color_gamut->GetTrainNumColor( 756 );
      CPPUNIT_ASSERT( color.GetValue() == Gdiplus::Color(0xFF, 0xFF, 0x80, 0x80).GetValue() );
    
}
    
void TC_PaletteDesigner::setUp()
{
    color_gamut = new ColorGamut( *Blinker::instance() );
}

void TC_PaletteDesigner::tearDown()
{
    if ( color_gamut )
        delete color_gamut;
    Blinker::Shutdowner();
}
