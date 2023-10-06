#include "stdafx.h"
#include "TC_PlukDll.h"
#include <vector>
#include "../Pluk/pg_factory.h"
#include "../Pluk/PlukDll.h"
#include "TC_PortablePgSQL.h"

#include "../helpful/Attic.h"
#include "UtHemHelper.h"
#include "../Hem/HemHelpful.h"
#include "../Hem/Hem.h"
#include "../helpful/DuePath.h"
#include "../helpful/Pilgrim.h"
#include "../helpful/Log.h"

using namespace std;
using namespace connection_pool;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PlukDll );

const int min_connect = 2;
const int max_connect = 10;

#ifdef _WIN64
	const bool x64 = true;
#else
	const bool x64 = false;
#endif

typedef std::shared_ptr<connection_pool::pool<pg_connection>> pg_pool;

std::wstring GetDllPath() {
	auto selfExe = Pilgrim::instance()->ExeFileName();
	return DuePath_firstExisted( DuePath( selfExe, x64, L"Pluk.dll" ) );
}

void TC_PlukDll::InitDll()
{
	PlukDll pluk( GetDllPath());
	CPPUNIT_ASSERT_NO_THROW(pluk.CreateConnect(LocalPostgresInstance::GetStringConnect()));
}

void TC_PlukDll::WriteChange()
{
	const std::wstring xml_changes = L"<?xml version=\"1.0\"?><SoxPostUpdChange increment=\"Y\" presently=\"20170117T073648Z\"><AutoChanges><HappenLayer><Trio><Body create_time=\"20170117T073648Z\" name=\"Form\" Bdg=\"P3P_SK[09370:09380]\" waynum=\"1\"/><Body create_time=\"20170117T073648Z\" name=\"Span_move\" Bdg=\"P5P_SK[09370:09380]\" waynum=\"1\"/></Trio><Trio><Body create_time=\"20170117T073657Z\" name=\"Form\" Bdg=\"N38P[11420:11760]\" waynum=\"2\"/><Body create_time=\"20170117T073657Z\" name=\"Span_move\" Bdg=\"N40P[11420:11760]\" waynum=\"2\"/></Trio><Trio><Body create_time=\"20170117T073659Z\" name=\"Form\" Bdg=\"N9P_KOK[09360:11760]\" waynum=\"1\"/><Body create_time=\"20170117T073659Z\" name=\"Span_move\" Bdg=\"N9P_KOK[09360:11760]\" waynum=\"1\"/><Body create_time=\"20170117T073659Z\" name=\"Span_move\" Bdg=\"N7P_KOK[09360:11760]\" waynum=\"1\"/></Trio><Trio><Body create_time=\"20170117T073700Z\" name=\"Form\" Bdg=\"N5P_JER[11430:11431]\" waynum=\"1\"/><Body create_time=\"20170117T073700Z\" name=\"Span_move\" Bdg=\"N3P_JER[11430:11431]\" waynum=\"1\"/><Body create_time=\"20170117T073700Z\" name=\"Span_move\" Bdg=\"N5P_JER[11430:11431]\" waynum=\"1\"/><Body create_time=\"20170117T073700Z\" name=\"Span_move\" Bdg=\"N7P_JER[11430:11431]\" waynum=\"1\"/></Trio><Trio><Pre create_time=\"20170117T073700Z\" name=\"Span_move\" Bdg=\"N7P_JER[11430:11431]\" waynum=\"1\"/><Post create_time=\"20170117T073700Z\" name=\"Span_move\" Bdg=\"N7P_JER[11430:11431]\" waynum=\"1\" cover=\"Y\"/><Body create_time=\"20170117T073715Z\" name=\"Span_move\" Bdg=\"N9P_JER[11430:11431]\" waynum=\"1\"/></Trio><Trio><Pre create_time=\"20170117T073657Z\" name=\"Span_move\" Bdg=\"N40P[11420:11760]\" waynum=\"2\"/><Post create_time=\"20170117T073657Z\" name=\"Span_move\" Bdg=\"N40P[11420:11760]\" waynum=\"2\" cover=\"Y\"/><Body create_time=\"20170117T073716Z\" name=\"Span_move\" Bdg=\"N42P[11420:11760]\" waynum=\"2\"/></Trio><Trio><Pre create_time=\"20170117T073659Z\" name=\"Span_move\" Bdg=\"N7P_KOK[09360:11760]\" waynum=\"1\"/><Post create_time=\"20170117T073659Z\" name=\"Span_move\" Bdg=\"N7P_KOK[09360:11760]\" waynum=\"1\" cover=\"Y\"/><Body create_time=\"20170117T073717Z\" name=\"Span_move\" Bdg=\"N5P_KOK[09360:11760]\" waynum=\"1\"/></Trio><Trio><Pre create_time=\"20170117T073648Z\" name=\"Span_move\" Bdg=\"P5P_SK[09370:09380]\" waynum=\"1\"/><Post create_time=\"20170117T073648Z\" name=\"Span_move\" Bdg=\"P5P_SK[09370:09380]\" waynum=\"1\" cover=\"Y\"/><Body create_time=\"20170117T073718Z\" name=\"Span_move\" Bdg=\"P7P_SK[09370:09380]\" waynum=\"1\"/></Trio><Trio><Body create_time=\"20170117T073859Z\" name=\"Form\" Bdg=\"P5P[09000:09420]\" waynum=\"1\"/><Body create_time=\"20170117T073859Z\" name=\"Span_move\" Bdg=\"P7P[09000:09420]\" waynum=\"1\"/></Trio><Trio><Pre create_time=\"20170117T073859Z\" name=\"Span_move\" Bdg=\"P7P[09000:09420]\" waynum=\"1\"/><Post create_time=\"20170117T073859Z\" name=\"Span_move\" Bdg=\"P7P[09000:09420]\" waynum=\"1\" cover=\"Y\"/><Body create_time=\"20170117T073913Z\" name=\"Span_move\" Bdg=\"N29P[09000:09420]\" waynum=\"1\"/></Trio></HappenLayer><AsoupLayer><Trio layer=\"asoup\"><Body create_time=\"20170117T073600Z\" name=\"Arrival\" Bdg=\"ASOUP 1042[11760]\" index=\"1116-078-1176\" num=\"8501\" length=\"4\" weight=\"124\" util=\"Y\" waynum=\"5\" parknum=\"1\" dirFrom=\"11750\" adjFrom=\"11750\"><![CDATA[(:1042 909/000+11760 8501 1116 078 1176 01 11750 17 01 09 36 01/05 1 0/00 00 0 2 0 00 00 00 00 0000 0 0 004 00124 00000 012 19185362 19151257 003 000 00 000 000 12 00 19185362 1 000 11310 42100 7755 005 00 00 00 00 00 0537 50 10 0580 04 130 11760 11420 00000 02 11310 43 00000 0000 000 0 0000 80/25  128 000 00000000 12 00 19151240 251237127247 000035005561050      12 00 19151257 255255255255 :)]]></Body></Trio></AsoupLayer></AutoChanges><UserChanges><HappenLayer><Trio><Body create_time=\"20170117T073648Z\" name=\"Form\" Bdg=\"P3P_SK[09370:09380]\" waynum=\"1\"/><Body create_time=\"20170117T073648Z\" name=\"Span_move\" Bdg=\"P5P_SK[09370:09380]\" waynum=\"1\"/></Trio><Trio><Body create_time=\"20170117T073657Z\" name=\"Form\" Bdg=\"N38P[11420:11760]\" waynum=\"2\"/><Body create_time=\"20170117T073657Z\" name=\"Span_move\" Bdg=\"N40P[11420:11760]\" waynum=\"2\"/></Trio><Trio><Body create_time=\"20170117T073659Z\" name=\"Form\" Bdg=\"P5P[09000:09420]\" waynum=\"1\"/><Body create_time=\"20170117T073659Z\" name=\"Span_move\" Bdg=\"P7P[09000:09420]\" waynum=\"1\"/></Trio><Trio><Body create_time=\"20170117T073659Z\" name=\"Form\" Bdg=\"N9P_KOK[09360:11760]\" waynum=\"1\"/><Body create_time=\"20170117T073659Z\" name=\"Span_move\" Bdg=\"N7P_KOK[09360:11760]\" waynum=\"1\"/></Trio><Trio><Pre create_time=\"20170117T073659Z\" name=\"Span_move\" Bdg=\"P7P[09000:09420]\" waynum=\"1\" cover=\"Y\"/><Post create_time=\"20170117T073659Z\" name=\"Span_move\" Bdg=\"P7P[09000:09420]\" waynum=\"1\" cover=\"Y\"/><Body create_time=\"20170117T073713Z\" name=\"Span_move\" Bdg=\"N29P[09000:09420]\" waynum=\"1\"/></Trio><Trio><Pre create_time=\"20170117T073657Z\" name=\"Span_move\" Bdg=\"N40P[11420:11760]\" waynum=\"2\" cover=\"Y\"/><Post create_time=\"20170117T073657Z\" name=\"Span_move\" Bdg=\"N40P[11420:11760]\" waynum=\"2\" cover=\"Y\"/><Body create_time=\"20170117T073716Z\" name=\"Span_move\" Bdg=\"N42P[11420:11760]\" waynum=\"2\"/></Trio><Trio><Pre create_time=\"20170117T073659Z\" name=\"Span_move\" Bdg=\"N7P_KOK[09360:11760]\" waynum=\"1\" cover=\"Y\"/><Post create_time=\"20170117T073659Z\" name=\"Span_move\" Bdg=\"N7P_KOK[09360:11760]\" waynum=\"1\" cover=\"Y\"/><Body create_time=\"20170117T073717Z\" name=\"Span_move\" Bdg=\"N5P_KOK[09360:11760]\" waynum=\"1\"/></Trio><Trio><Pre create_time=\"20170117T073648Z\" name=\"Span_move\" Bdg=\"P5P_SK[09370:09380]\" waynum=\"1\" cover=\"Y\"/><Post create_time=\"20170117T073648Z\" name=\"Span_move\" Bdg=\"P5P_SK[09370:09380]\" waynum=\"1\" cover=\"Y\"/><Body create_time=\"20170117T073718Z\" name=\"Span_move\" Bdg=\"P7P_SK[09370:09380]\" waynum=\"1\"/></Trio></HappenLayer><AsoupLayer><Trio layer=\"asoup\"><Body create_time=\"20170117T073600Z\" name=\"Arrival\" Bdg=\"ASOUP 1042[11760]\" index=\"1116-078-1176\" num=\"8501\" length=\"4\" weight=\"124\" util=\"Y\" waynum=\"5\" parknum=\"1\" dirFrom=\"11750\" adjFrom=\"11750\"><![CDATA[(:1042 909/000+11760 8501 1116 078 1176 01 11750 17 01 09 36 01/05 1 0/00 00 0 2 0 00 00 00 00 0000 0 0 004 00124 00000 012 19185362 19151257 003 000 00 000 000 12 00 19185362 1 000 11310 42100 7755 005 00 00 00 00 00 0537 50 10 0580 04 130 11760 11420 00000 02 11310 43 00000 0000 000 0 0000 80/25  128 000 00000000 12 00 19151240 251237127247 000035005561050      12 00 19151257 255255255255 :)]]></Body></Trio></AsoupLayer></UserChanges><TerraChanges><BusyLayer><Trio layer=\"busy\"><Body create_time=\"20170117T073647Z\" name=\"WayOccupy\" Bdg=\"1C[09190]\" waynum=\"1\" parknum=\"1\" /></Trio><Trio layer=\"busy\"><Pre create_time=\"20170117T073647Z\" name=\"WayOccupy\" Bdg=\"1C[09190]\" waynum=\"1\" parknum=\"1\" /><Post create_time=\"20170117T073647Z\" name=\"WayOccupy\" Bdg=\"1C[09190]\" waynum=\"1\" parknum=\"1\" cover=\"Y\" /><Body create_time=\"20170117T073702Z\" name=\"WayEmpty\" Bdg=\"1C[09190]\" waynum=\"1\" parknum=\"1\" /></Trio></BusyLayer></TerraChanges></SoxPostUpdChange>";

	attic::a_document change_doc;
	change_doc.load_wide(xml_changes);
	PlukDll pluk( GetDllPath());
	pluk.CreateConnect(LocalPostgresInstance::GetStringConnect());
	pluk.WriteChange(change_doc.to_str());

	const time_t from(1484611200);	// 2017-01-17 00:00
	const time_t to(1484697600);	// 2017-01-18 00:00
	std::string result = pluk.GetChart(from, to);
	ReleaseLog(result);
	attic::a_document resultXml;
	resultXml.load_utf8(result);
	attic::a_node autoLayer = resultXml.child("HappenLayer");
	UtLayer<HappenLayer> hemPathLayer;
	for (attic::a_node hemPath: autoLayer.children("HemPath"))
	{
		hemPathLayer.createPath( FromUtf8(hemPath.to_str()));
	}
	attic::a_document hemXml("hl");
	hemPathLayer.Serialize(hemXml.document_element());
	std::string hlStr = hemXml.document_element().child("HappenLayer").to_str();
	ReleaseLog(hlStr);
	ReleaseLog(autoLayer.to_str());
	CPPUNIT_ASSERT(autoLayer.to_str() == hlStr);
}


void TC_PlukDll::setUp()
{
	LocalPostgresInstance::Start();
}

void TC_PlukDll::tearDown() 
{
	LocalPostgresInstance::Stop();
}