#pragma once

#include <vector>

#include "../Hem/HemHelpful.h"
#include "../Hem/EventTrio.h"
#include "../Hem/TrackedChart.h"
#include "../Guess/Msg.h"

class TC_ChartImpl
{
public:
    typedef HemHelpful::SpotEventPtr SpotPtr;
    typedef HemHelpful::AsoupEventPtr AsoupPtr;

protected:
    // Создает нитку на основании событий "от трекера", 
    // возвращает true если все события были приняты чартом
    // сохраняет изменения в членах класса
    template <unsigned const count>
    bool CreatePath(Hem::TrackedChart& chart, const SpotPtr (&events)[count], unsigned trainID);
    bool CreatePath(Hem::TrackedChart& chart, const std::vector<SpotPtr>& events, unsigned trainID);

    // Вводит в чарт АСОУП сообщение извне, возвращает true если оно было принято чартом
    // сохраняет изменения в членах класса
    bool PassAsoup( Hem::TrackedChart& chart, const AsoupPtr& asoup, time_t currentTime=0 );

    bool PassAsoupRawText(Hem::TrackedChart& chart, const std::wstring& asoup, const 
		boost::gregorian::date& asoupDate = boost::gregorian::date(2015, 10, 10), const boost::posix_time::time_duration& asoupTime = boost::posix_time::time_duration() );
	bool PassGuiRawText(Hem::TrackedChart& chart, const std::wstring& asoup);

    void ProcessChanges(Hem::TrackedChart& chart);

    attic::a_node GetChangesNode() const;
    attic::a_node GetAnalysisNode() const;

    size_t GetHappenTrioCount() const;
    TrioSpot GetHappenTrio(const size_t index) const;

    size_t GetAsoupTrioCount() const;
    TrioAsoup GetAsoupTrio(const size_t index) const;

	size_t GetGuiTrioCount() const;

	size_t GetTrainDescrCount() const;
    std::shared_ptr<const TrainDescr> GetTrainDescr(const size_t index) const;

    attic::a_document m_changesDoc;
    attic::a_document m_analysisDoc;

	time_t FindEventInfo( const std::list < HemHelpful::SpotEventPtr > & listToCheck,
		const std::pair <HCode, BadgeE> & seekedInfo ) const;
	bool FindTrainDescr( const std::list <std::pair <HemHelpful::SpotEventPtr, std::shared_ptr<const TrainDescr> > > & listToCheck,
		const TrainDescr &, std::pair <time_t, time_t> & result_interval ) const;

private:
    void InitDocuments();
};

template <unsigned const count>
inline bool TC_ChartImpl::CreatePath(Hem::TrackedChart& chart, const SpotPtr (&events)[count], unsigned trainID)
{
    try
    {
        std::vector<SpotPtr> temp( std::begin(events), std::end(events) );
        return CreatePath( chart, temp, trainID );
    }
    catch (const std::exception& )
    {
        return false;
    }
}

inline bool TC_ChartImpl::PassAsoup(Hem::TrackedChart& chart, const AsoupPtr& asoup, time_t currentTime)
{
    bool const result = chart.processNewAsoup(asoup, currentTime, nullptr);
	chart.updateTime(currentTime, nullptr);
    ProcessChanges(chart);

    return result;
}