#include "stdafx.h"

#include "TC_Hem_aeTrimNote.h"
#include "UtHemHelper.h"
#include "../Hem/NoteLayer.h"
#include "../Hem/Appliers/aeTrimNote.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeTrimNote );

void TC_Hem_aeTrimNote::test()
{
    UtLayer<Note::Layer> note_layer;
    {
        Note::Event evSource = createEvent<Note::Event>(
            L"<MarkEvent Id='G25' esr='09715:09730' waynum='2' create_time='20151019T223024Z' print='Y' Comment='Остановка поезда  перед входным светофором.' inner_name='srvMoving'>"
            L"    <picketing picketing1_val='22~1000' picketing1_comm='Рига-Тукумс' />"
            L"</MarkEvent>"
            );
        note_layer.Push(evSource);
        evSource = createEvent<Note::Event>(
            L"<MarkEvent Id='i33' esr='11267:11270' waynum='1' create_time='20151023T223024Z' print='Y' Comment='Остановка поезда  перед входным светофором.' inner_name='srvMoving'>"
            L"    <picketing picketing1_val='429~150' picketing1_comm='Россия' />"
            L"</MarkEvent>"
            );
        note_layer.Push(evSource);
    }

    CPPUNIT_ASSERT( note_layer.path_count() == 2 );
    CPPUNIT_ASSERT( note_layer.exist_event("@Id='R25'") );
    CPPUNIT_ASSERT( note_layer.exist_event("@Id='i33'") );

    namespace bg = boost::gregorian;
    namespace bt = boost::posix_time;
    bt::ptime pt   ( bg::date(2015,10,20), bt::hours(0) );
    bt::ptime epoch = bt::from_time_t(0);
    time_t const seconds = (pt-epoch).total_seconds();
    Hem::aeTrimNote ntrim( note_layer, seconds );
    ntrim.Action();

    note_layer.ForEach([seconds](const Note::Event& event) {
        CPPUNIT_ASSERT(!event.isLeft(seconds));
    });

    CPPUNIT_ASSERT( note_layer.path_count() == 1 );
    CPPUNIT_ASSERT( note_layer.exist_event("@Id='i33'") );
}
