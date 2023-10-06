#include "../Hem/AutoChart.h"

class AutoChartTest : public AutoChart
{
public:
    AutoChartTest();
 	using AutoChart::GetPaths_OnlyEvents;
    using AutoChart::GetPaths_OnlyFrags;
    using AutoChart::GetPaths_EventAndDescr;
	using AutoChart::getAsoupLayer;
	using AutoChart::getHappenLayer;
};

inline AutoChartTest::AutoChartTest()
{
    startHookTrainChanges();
}

// свободные функции
inline attic::a_node operator<< ( attic::a_node parent_node, const AutoChartTest& act )
{
    const AutoChart& ac = act;
    parent_node << ac;
    return parent_node;
}

inline const attic::a_node operator>> ( attic::a_node parent_node, AutoChartTest& act )
{
    AutoChart& ac = act;
    parent_node >> ac;
    return parent_node;
}
