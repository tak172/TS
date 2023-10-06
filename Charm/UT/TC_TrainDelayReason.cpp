#include "stdafx.h"

#include "../helpful/Attic.h"
#include "TC_TrainDelayReason.h"

#include "../Actor/HemDraw/TrainDelayReason.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrainDelayReason );

void TC_TrainDelayReason::Load()
{
	const std::wstring delays_txt = 
		L"<DelayList>"
		L"<GroupReason name='Задержки поездов по вине службы перевозок  (Д)'>"
		L"<DelayReason name='Несвоевременное формирование состава' />"
		L"<DelayReason name='Несвоевременное формирование поездных документов' />"
		L"<DelayReason name='Неприем станцией' />"
		L"<DelayReason name='Другие причины по вине (Д)' />"
		L"</GroupReason>"
		L"<GroupReason name='Задержки поездов по вине службы локомотивного хозяйства \"Т\"' >"
		L"<DelayReason name='Неисправность локомотива' />"
		L"<GroupReason name='Прочие причины по вине (Т)' >"
		L"<DelayReason name='Несвоевременная выдача локомотива' />"
		L"<DelayReason name='Срыв стоп-крана в электро- и дизель-поездах' />"
		L"</GroupReason>"
		L"<DelayReason name='Внеплановая отмена пригородных поездов' />"
		L"</GroupReason>"
		L"</DelayList>";

	attic::a_document x_doc;
	x_doc.load_wide( delays_txt );
	std::wstring str_src = FromUtf8(x_doc.document_element().to_str());

	TrainDelayList DelayList;
	x_doc.document_element() >> DelayList;

	CPPUNIT_ASSERT ( !DelayList.empty() );

	const DELAY_VECT& vDelay = DelayList.get_delays();
	CPPUNIT_ASSERT(vDelay.size()==3);
	
	if ( vDelay.size()>0 )
	{
		const TrainDelayReason& frst_it = vDelay[0];
		CPPUNIT_ASSERT( frst_it.get_name()==L"Задержки поездов по вине службы перевозок  (Д)");
		const DELAY_VECT& vDelayChild = frst_it.get_delays();
		CPPUNIT_ASSERT( vDelayChild.size()==4 );
		if ( vDelayChild.size()==4 )
			{
				const TrainDelayReason& it = vDelayChild[3];
				CPPUNIT_ASSERT( it.get_name()==L"Другие причины по вине (Д)" && !it.have_child() );
			}
	}

	if ( vDelay.size()>1 )
	{
		const TrainDelayReason& scnd_it = vDelay[1];
		CPPUNIT_ASSERT( scnd_it.get_name()==L"Задержки поездов по вине службы локомотивного хозяйства \"Т\"");
		const DELAY_VECT& vDelayChild = scnd_it.get_delays();
		CPPUNIT_ASSERT( vDelayChild.size()==3 );
		if ( vDelayChild.size()>1 )
		{
			const TrainDelayReason& it = vDelayChild[1];
			CPPUNIT_ASSERT( it.get_name()==L"Прочие причины по вине (Т)" && it.get_delays().size()==2 );
			if ( it.get_delays().size()==2 )
			{
				CPPUNIT_ASSERT( it.get_delays()[0].get_name()==L"Несвоевременная выдача локомотива" && 
					!it.get_delays()[0].have_child() );
				CPPUNIT_ASSERT( it.get_delays()[1].get_name()==L"Срыв стоп-крана в электро- и дизель-поездах" && 
					!it.get_delays()[1].have_child() );
			}
		}
	}
}