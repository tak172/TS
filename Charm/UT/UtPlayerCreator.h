#pragma once

//#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/IPlayerPanel.h"
class CTimeWarper;
namespace Bbx {
    class Reader;
};

class UtPlayerPanel : public IPlayerPanel
{
public:
    UtPlayerPanel( CTimeWarper& timeWarper, Bbx::Reader& bbxReader );
    bool showFirst() override; // ���������� ������ (�.�. ��� ������ ���������)
    void stopWarper() override; // ���������� �������� �������
    void displayTimeAsCurrent( time_t moment ) override; // ���������� � ���� �������� �������
    static IPlayerPanel* construct( CTimeWarper& timeWarper, Bbx::Reader& bbxReader );

private:
    CTimeWarper& m_timeWarper;
    Bbx::Reader& m_bbxReader;
};
