#include "stdafx.h"
#include "UtPlayerCreator.h"


UtPlayerPanel::UtPlayerPanel( CTimeWarper& timeWarper, Bbx::Reader& bbxReader ) : m_timeWarper( timeWarper ), m_bbxReader( bbxReader )
{

}

bool UtPlayerPanel::showFirst()
{
    return true;
}

void UtPlayerPanel::stopWarper()
{

}

void UtPlayerPanel::displayTimeAsCurrent( time_t /*moment*/ )
{

}

IPlayerPanel* UtPlayerPanel::construct( CTimeWarper& timeWarper, Bbx::Reader& bbxReader )
{
    IPlayerPanel* pp = new UtPlayerPanel( timeWarper, bbxReader );
    return pp;
}
