#include "stdafx.h"
#include "TC_ChartImpl.h"
#include "../Hem/SpotEvent.h"
#include "../Hem/AsoupEvent.h"
#include "../Hem/AutoChart.h"
#include "../Hem/AsoupServedData.h"
#include "../Guess/Msg.h"
#include "UtHelpfulDateTime.h"

using namespace std;
using namespace HemHelpful;

time_t TC_ChartImpl::FindEventInfo( const std::list < HemHelpful::SpotEventPtr > & listToCheck,
						const std::pair <HCode, BadgeE> & seekedInfo ) const
{
	time_t eventTime = 0;
	for( auto& CI : listToCheck )
	{
		if ( CI->GetCode() == seekedInfo.first && CI->GetBadge() == seekedInfo.second )
		{
			eventTime = CI->GetTime();
			break;
		}
	}
	return eventTime;
}

bool TC_ChartImpl::FindTrainDescr( const list <pair <HemHelpful::SpotEventPtr, ConstTrainDescrPtr> > & listToCheck, const TrainDescr & tdescr, 
						pair <time_t, time_t> & result_interval ) const
{
	bool found = false;
	for( auto CIt = listToCheck.cbegin(); CIt != listToCheck.cend(); ++CIt )
	{
        if (CIt->second)
        {
            if (*CIt->second == tdescr)
            {
                if (!found)
                {
                    result_interval.first = CIt->first->GetTime();
                    found = true;
                }
                result_interval.second = CIt->first->GetTime();
            }
            else
            {
                if (found)
                    break;
            }
        }
        else
        {
            if (found)
                result_interval.second = CIt->first->GetTime();
        }
	}
	return found;
}

attic::a_node TC_ChartImpl::GetChangesNode() const
{
    return m_changesDoc.document_element();
}

attic::a_node TC_ChartImpl::GetAnalysisNode() const
{
    return m_analysisDoc.document_element();
}

size_t GetTrioCount(const attic::a_node& changesNode, const std::string& layerName)
{
    size_t count = 0;

    attic::a_node const layerNode = changesNode.child(layerName);
    for (const attic::a_node& trioNode : layerNode)
    {
        if (trioNode.name_is(Trio_xTag))
        {
            ++count;
        }
    }

    return count;
}

attic::a_node GetTrioNode(const attic::a_node& changesNode, size_t index, const std::string& layerName)
{
    size_t count = 0;

    attic::a_node const layerNode = changesNode.child(layerName);
    for (const attic::a_node& trioNode : layerNode)
    {
        if (trioNode.name_is(Trio_xTag))
        {
            if (count++ == index)
            {
                return trioNode;
            }
        }
    }

    throw std::runtime_error("Trio count is less than expected");
}

size_t TC_ChartImpl::GetHappenTrioCount() const
{
    return GetTrioCount(GetChangesNode(), HappenLayer_xAttr);
}

size_t TC_ChartImpl::GetAsoupTrioCount() const
{
    return GetTrioCount(GetChangesNode(), AsoupLayer_xAttr);
}

size_t TC_ChartImpl::GetGuiTrioCount() const
{
	return GetTrioCount(GetChangesNode(), GuiLayer_xAttr);
}

TrioSpot TC_ChartImpl::GetHappenTrio(const size_t index) const
{
    attic::a_node trioNode = GetTrioNode(GetChangesNode(), index, HappenLayer_xAttr);
    TrioSpot trio;
    trioNode >> trio;
    return trio;
}

TrioAsoup TC_ChartImpl::GetAsoupTrio(const size_t index) const
{
    attic::a_node trioNode = GetTrioNode(GetChangesNode(), index, AsoupLayer_xAttr);
    TrioAsoup trio;
    trioNode >> trio;
    return trio;
}

size_t TC_ChartImpl::GetTrainDescrCount() const
{
    size_t count = 0;

    attic::a_node const analysisNode = GetAnalysisNode().child(TrainDescr_xAttr);
    for (const attic::a_node& childNode : analysisNode)
    {
        if (childNode.name_is(info_xAttr))
        {
            ++count;
        }
    }

    return count;
}

std::shared_ptr<const TrainDescr> TC_ChartImpl::GetTrainDescr(const size_t index) const
{
    std::shared_ptr<const TrainDescr> result;
    size_t count = 0;

    attic::a_node const analysisNode = GetAnalysisNode().child(TrainDescr_xAttr);
    for (const attic::a_node& childNode : analysisNode)
    {
        if (childNode.name_is(info_xAttr))
        {
            if (count++ == index)
            {
                result.reset(new TrainDescr(childNode));
                if (result->empty())
                    result.reset();
            }
        }
    }

    return result;
}

bool TC_ChartImpl::PassAsoupRawText(Hem::TrackedChart& chart, const std::wstring& messageText, const boost::gregorian::date& asoupDate, const boost::posix_time::time_duration& asoupTime)
{
    time_t asoupMessageReceivedTime = timeForDate(asoupDate.year(), asoupDate.month(), asoupDate.day());
	
	chart.updateTime(asoupMessageReceivedTime, nullptr);
    bool const result = chart.processNewAsoupText(messageText, asoupDate, nullptr, 0);
	if ( auto delta = asoupTime.total_milliseconds() )
		chart.updateTime(asoupMessageReceivedTime + delta, nullptr);
    ProcessChanges(chart);
    return result;
}

bool TC_ChartImpl::PassGuiRawText(Hem::TrackedChart& chart, const std::wstring& messageText)
{
	boost::gregorian::date const guiDate(2022, 01, 11);
	time_t guiMessageReceivedTime = timeForDate(guiDate.year(), guiDate.month(), guiDate.day());

	chart.updateTime(guiMessageReceivedTime, nullptr);
	bool const result = chart.processNewGuiMarkText(messageText, guiMessageReceivedTime);
	ProcessChanges(chart);
	return result;
}

void TC_ChartImpl::InitDocuments()
{
    m_changesDoc.reset();
    m_analysisDoc.reset();
    m_changesDoc.set_document_element("ChangesRoot");
    m_analysisDoc.set_document_element("AnalysisRoot");

    if (m_changesDoc.document_element().first_child() || m_analysisDoc.document_element().first_child())
        throw std::runtime_error("Expected nodes to be empty");
}

void TC_ChartImpl::ProcessChanges(Hem::TrackedChart& chart)
{
    InitDocuments();
    chart.takeChanges(m_changesDoc.document_element(), m_analysisDoc.document_element(), 0);
}

bool TC_ChartImpl::CreatePath(Hem::TrackedChart& chart, const std::vector<SpotPtr>& events, unsigned trainID)
{
    try
    {
        bool allSucceeded = true;
        for (const SpotPtr& spot : events)
        {
            if (!chart.acceptTrackerEvent(spot, trainID, TrainCharacteristics::Source::Guess))
                allSucceeded = false;
        }
        ProcessChanges(chart);
        return allSucceeded;
    }
    catch (const std::exception& )
    {
        return false;
    }
}
