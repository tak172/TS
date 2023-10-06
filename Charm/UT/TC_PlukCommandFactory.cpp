#include "stdafx.h"
#include "TC_PlukCommandFactory.h"
#include <string>
#include <time.h>
#include <vector>
#include <map>
#include "../Pluk/PlukCommandFactory.h"
#include "../Pluk/PlukMessage.h"
#include "../Pluk/PlukCommand.h"
#include "../Pluk/PlukConst.h"
#include "../helpful/Attic.h"
#include "../helpful/Serialization.h"

using namespace std;
using namespace attic;
using namespace PlukConst;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PlukCommandFactory );

void TC_PlukCommandFactory::CommandCreate()
{
	string bodyText = "BodyText";
	time_t fromTime = 1505865600; //20.09.2017 00:00:00 
	time_t toTime = 1505865600;
	time_t atTime = 1505865600;
	time_t createTime = 1505865600;
	string layerA = "layerA";
	string layerB = "layerB";
	vector<string> layerParam;
	layerParam.push_back(layerA);
	layerParam.push_back(layerB);
	

	PlukCommandFactory commandFactory;

	auto message = commandFactory.WriteChange(bodyText);
	CPPUNIT_ASSERT(message.getCommand() == PlukCommand::write_change);
	CPPUNIT_ASSERT(message.getBody() == bodyText);

	message = commandFactory.WriteSchedule(bodyText);
	CPPUNIT_ASSERT(message.getCommand() == PlukCommand::write_schedule);
	CPPUNIT_ASSERT(message.getBody() == bodyText);

	message = commandFactory.GetStatutorySchedule(fromTime, toTime);
	CPPUNIT_ASSERT(message.getCommand() == PlukCommand::get_statutory);
	auto head = message.getHead();

	message = commandFactory.GetSchedule(atTime, createTime);
	CPPUNIT_ASSERT(message.getCommand() == PlukCommand::get_schedule);
	head = message.getHead();
	
	message = commandFactory.GetScheduleList(fromTime, toTime);
	CPPUNIT_ASSERT(message.getCommand() == PlukCommand::get_schedule_list);
	head = message.getHead();
	
	message = commandFactory.GetUserChart(fromTime, toTime, layerParam);
	CPPUNIT_ASSERT(message.getCommand() == PlukCommand::get_user_chart);
	head = message.getHead();
	
    message = commandFactory.GetDraftVgdp(fromTime, toTime, layerParam);
    CPPUNIT_ASSERT(message.getCommand() == PlukCommand::get_draft_vgdp);
    head = message.getHead();

	a_document xml;
	CPPUNIT_ASSERT(xml.load_utf8(message.getBody()));
	a_node chartNode = xml.child(PlukConst::CHART_PARAM_NODE);
	CPPUNIT_ASSERT(chartNode != NULL);
	CPPUNIT_ASSERT(chartNode.child(layerA) != NULL);
	CPPUNIT_ASSERT(chartNode.child(layerB) != NULL);


    message = commandFactory.GetFinalVgdp(fromTime, toTime);
    CPPUNIT_ASSERT(message.getCommand() == PlukCommand::get_final_vgdp);
    head = message.getHead();

	message = commandFactory.GetAutoChart(fromTime, toTime, layerParam);
	CPPUNIT_ASSERT(message.getCommand() == PlukCommand::get_auto_chart);
	head = message.getHead();
	CPPUNIT_ASSERT(xml.load_utf8(message.getBody()));
	chartNode = xml.child(PlukConst::CHART_PARAM_NODE);
	CPPUNIT_ASSERT(chartNode != NULL);
	CPPUNIT_ASSERT(chartNode.child(layerA) != NULL);
	CPPUNIT_ASSERT(chartNode.child(layerB) != NULL);

	message = commandFactory.GetPatch(fromTime, toTime);
	CPPUNIT_ASSERT(message.getCommand() == PlukCommand::get_patch);
	head = message.getHead();
}
