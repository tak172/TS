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
    bool showFirst() override; // отобразить панель (т.е. это первое обращение)
    void stopWarper() override; // остановить движение времени
    void displayTimeAsCurrent( time_t moment ) override; // отобразить в поле текущего момента
    static IPlayerPanel* construct( CTimeWarper& timeWarper, Bbx::Reader& bbxReader );

private:
    CTimeWarper& m_timeWarper;
    Bbx::Reader& m_bbxReader;
};
