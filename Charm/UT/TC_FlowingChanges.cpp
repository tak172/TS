#include "stdafx.h"

#include "TC_FlowingChanges.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_FlowingChanges );

TC_FlowingChanges::TC_FlowingChanges()
: 
data0( std::make_pair( time_t(00),BadgeE(L"0",EsrKit(12345)) ) ),
data1( std::make_pair( time_t(11),BadgeE(L"a",EsrKit(23456)) ) ),
data2( std::make_pair( time_t(22),BadgeE(L"b",EsrKit(34567)) ) ),
data3( std::make_pair( time_t(33),BadgeE(L"c",EsrKit(45678)) ) )
{

}

void TC_FlowingChanges::setUp()
{
    // подготовить данные
    out.push_back(data0);
    out.push_back(data1);
    out.push_back(data2);

    attic::a_node rut = xdoc.set_document_element("test_root");
    ref = rut.append_child("ref");
    before = rut.append_child("before");
    after = rut.append_child("after");

}
void TC_FlowingChanges::tearDown()
{
    out.clear();
    xdoc.reset();
}

void TC_FlowingChanges::changes()
{
    // создаем управление изменениями
    FlowingChangesProducer<COLL_TYPE> flow(out);
    // сначала изменений нет
    CPPUNIT_ASSERT( !flow.hasChanges() );
    std::swap(out[0],out[1]);
    CPPUNIT_ASSERT( flow.hasChanges() );
    std::swap(out[0],out[1]);
    // добавление - это изменение
    out.push_back( data3 );
    CPPUNIT_ASSERT( flow.hasChanges() );
    out.pop_back();
    CPPUNIT_ASSERT( !flow.hasChanges() );
    // удаление - это тоже изменение 
    out.pop_back();
    CPPUNIT_ASSERT( flow.hasChanges() );
    out.push_back( data2 );
    CPPUNIT_ASSERT( !flow.hasChanges() );
}

void TC_FlowingChanges::ref_out_in()
{
    // первый референс
    FlowingChangesProducer<COLL_TYPE> flow(out);
    flow.writeReference(ref);
    // ввод
    COLLECTION in;
    CPPUNIT_ASSERT( true==intakeChangesRef(ref,in) );
    CPPUNIT_ASSERT( out==in );
}

void TC_FlowingChanges::inc_head()
{
    // первый референс
    FlowingChangesProducer<COLL_TYPE> flow(out);
    flow.writeReference(ref);

    COLLECTION in;
    CPPUNIT_ASSERT( true == intakeChangesRef(ref,in) );
    CPPUNIT_ASSERT( out==in );

    // изменения (вперед)
    const COLLECTION out_ref = out;
    out.erase(out.begin());
    flow.writeIncrement(before,after);
    CPPUNIT_ASSERT( true==intakeChangesInc(after,in) );
    CPPUNIT_ASSERT( out==in );

    // изменения (назад)
    CPPUNIT_ASSERT( true == intakeChangesInc(before,in) );
    CPPUNIT_ASSERT( out_ref==in );
}

void TC_FlowingChanges::inc_tail()
{
    // первый референс
    FlowingChangesProducer<COLL_TYPE> flow(out);
    flow.writeReference(ref);

    COLLECTION in;
    CPPUNIT_ASSERT( true == intakeChangesRef(ref,in) );
    CPPUNIT_ASSERT( out==in );

    // изменения (вперед)
    const COLLECTION out_ref = out;
    out.push_back(data3);
    flow.writeIncrement(before,after);
    CPPUNIT_ASSERT( true==intakeChangesInc(after,in) );
    CPPUNIT_ASSERT( out==in );

    // изменения (назад)
    CPPUNIT_ASSERT( true == intakeChangesInc(before,in) );
    CPPUNIT_ASSERT( out_ref==in );
}

void TC_FlowingChanges::inc_swap()
{
    // первый референс
    FlowingChangesProducer<COLL_TYPE> flow(out);
    flow.writeReference(ref);

    COLLECTION in;
    CPPUNIT_ASSERT( true == intakeChangesRef(ref,in) );
    CPPUNIT_ASSERT( out==in );

    // изменения (вперед)
    const COLLECTION out_ref = out;
    std::swap(out[1],out[2]);
    flow.writeIncrement(before,after);
    CPPUNIT_ASSERT( true==intakeChangesInc(after,in) );
    CPPUNIT_ASSERT( out==in );

    // изменения (назад)
    CPPUNIT_ASSERT( true == intakeChangesInc(before,in) );
    CPPUNIT_ASSERT( out_ref==in );
}
