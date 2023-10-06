#pragma once
#include "../helpful/Attic.h"
#include "../helpful/TrainDescr.h"

inline TrainDescr BuildTrain( unsigned int number, std::wstring index, std::wstring typeinfo, unsigned int length,
				unsigned grossweight, unsigned netweight, std::wstring delaytime )
{
	attic::a_document train_doc( "root" );
	auto root_node = train_doc.document_element();
	root_node.append_attribute( number_xAttr ).set_value( number );
	root_node.append_attribute( index_xAttr ).set_value( index );
	root_node.append_attribute( length_xAttr ).set_value( length );
	root_node.append_attribute( gross_weight_xAttr ).set_value( grossweight );
    root_node.append_attribute( net_weight_xAttr ).set_value( netweight );
	auto ftxt_node = root_node.append_child( feat_texts );
	const auto & tcharacts = TrainCharacteristics::instanceCRef();
	const auto & delfeatinfo = tcharacts.GetTrainFeatureInfo( TrainCharacteristics::TrainFeature::Delay );
	ftxt_node.append_attribute( delfeatinfo.xml_name ).set_value( delaytime );
	ftxt_node.append_attribute( "typeinfo" ).set_value( typeinfo );
	return TrainDescr( root_node );
}
