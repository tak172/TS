#include "stdafx.h"

#include "../helpful/Attic.h"
#include "TC_ShowCategory.h"
#include "../Actor/HemDraw/CategoryTree.h"
#include "../helpful/TrainDescr.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_ShowCategory );

void TC_ShowCategory::LoadSave()
{
    const std::wstring pe_num_text = 
        L"<category version='2.0' pattern='1111111111111111' width='2'>"
         L"<train name='�������������������� ������'>"
         L"<screen color='0x00FFFFFF' slave_color='0x00FFFFF0' width='1' />"
         L"<print color='0x00000000' slave_color='0x00000000' width='1' />"
         L"<predict color='0x00000000' slave_color='0x00000000' width='1' />"
         L"</train>"
         L"<train name='�������������� ������'>"
         L"<screen color='0x00FF0000' slave_color='0x005C5AFF' />"
         L"<print color='0x00000000' slave_color='0x00000000' width='1' />"
         L"<predict color='0x00000000' slave_color='0x00000000' width='1' />"
         L"</train>"
        L"<train name='��������� ������'>"
        L"<screen color='0x00010000' slave_color='0x00040000' width='1' />"
        L"<print color='0x00020000' slave_color='0x00050000' width='1' />"
        L"<predict color='0x00030000' slave_color='0x00060000' width='1' />"
        L"</train>"
         L"<train num_diapason='1-150' name='������ ��������������'>"
         L"<screen color='0x000000FF' slave_color='0x005C5A42' />"
         L"<print color='0x000000FF' slave_color='0x005C5A42' />"
         L"<predict color='0x000000F0' slave_color='0x00000000' width='1' />"
         L"</train>"
         L"<train num_diapason='2001-2198' name='��������, � �.�. �� ���������� ������ �����������'>"
         L"<screen color='0x00FF0000' slave_color='0x005C5A42' />"
         L"<print color='0x00FF0000' slave_color='0x005C5A42' />"
         L"<predict color='0x00F00000' slave_color='0x005C5A42' width='1'/>"
         L"</train>"
         L"<train num_diapason='3401-3468' name='�������'>"
         L"<screen color='0x00000000' slave_color='0x005C5A42' pattern='1111110100111111' width='3' />"
         L"<print color='0x00000000' slave_color='0x005C5A42' pattern='1111110100111111' width='3' />"
         L"<predict color='0x00000000' slave_color='0x005C5A42' pattern='1111110100111111' width='1' />"
         L"</train>"
         L"<train num_diapason='9751-9798' name='����� 9000 � � �����'>"
         L"<screen color='0x0000FF00' slave_color='0x005C5A42' />"
         L"<print color='0x0000FF00' slave_color='0x005C5A42' />"
         L"<predict color='0x0000FF00' slave_color='0x005C5A42' />"
         L"</train>"
         L"<asoup name='���� �����'>"
         L"<screen color='0x00E1E4FF' slave_color='0x005C5A42' width='1'/>"
         L"<print color='0x00000000' slave_color='0x005C5A42' width='1'/>"
         L"</asoup>"
         L"<limit name='��������������'>"
         L"<screen color='0x000EFFFF' slave_color='0x005C5A42' />"
         L"<print color='0x000EFFFF' slave_color='0x005C5A42' />"
         L"</limit>"
         L"<window name='����'>"
         L"<screen color='0x000000FF' slave_color='0x005C5A42' width='1'/>"
         L"<print color='0x000000FF' slave_color='0x005C5A42' width='1'/>"
         L"</window>"
         L"<silence name='���������\\���������� �������' >"
         L"<screen color='0x000EFFFF' slave_color='0x005C5A42' />"
         L"<print color='0x000EFFFF' slave_color='0x005C5A42' />"
         L"</silence>"
         L"<busy name='��������� �����-����������� �����'>"
         L"<screen color='0x002068FF' slave_color='0x005C5A42' width='1'/>"
         L"<print color='0x002068FF' slave_color='0x005C5A42' width='1'/>"
         L"</busy>"
        L"</category>";

    attic::a_document x_doc;
    x_doc.load_wide( pe_num_text );
    std::wstring str_src = FromUtf8(x_doc.document_element().to_str());

    ShowCategoryTable sct( x_doc );

    attic::a_document y_doc;
    sct.SaveToXML( y_doc );
    std::wstring str_dst = FromUtf8(y_doc.document_element().to_str());
    auto mismresult = std::mismatch( str_dst.cbegin(), str_dst.cend(), str_src.cbegin(), str_src.cend() );

    CPPUNIT_ASSERT( str_src == str_dst );
}

void TC_ShowCategory::GetCategory()
{
    const std::wstring pe_num_text = 
        L"<category version='2.0' pattern='1111111111111111' width='2'>"
        L"<train name='�������������������� ������'>"
        L"<screen color='0x00FFFFFF' slave_color='0x00FFFFF0' width='1' />"
        L"<print color='0x00000000' slave_color='0x00000000' width='1' />"
        L"</train>"

        L"<train name='�������������� ������'>"
        L"<screen color='0x00FF0000' slave_color='0x005C5AFF' />"
        L"<print color='0x00000000' slave_color='0x00000000' width='1' />"
        L"</train>"
        L"<train name='��������� ������'>"
        L"<screen color='0x000F0000' slave_color='0x005C5AF0' width='1'/>"
        L"<print color='0x0000000F' slave_color='0x0000000F' width='1' />"
        L"</train>"
        L"<train num_diapason='1-150' name='������ ��������������'>"
        L"<screen color='0x000000FF' slave_color='0x005C5A42' />"
        L"<print color='0x000000FF' slave_color='0x005C5A42' />"
        L"</train>"
        L"<train num_diapason='2001-2198' name='��������, � �.�. �� ���������� ������ �����������'>"
        L"<screen color='0x00FF0000' slave_color='0x005C5A42' />"
        L"<print color='0x00FF0000' slave_color='0x005C5A42' />"
        L"</train>"
        L"<train num_diapason='3401-3468' name='�������'>"
        L"<screen color='0x00000000' slave_color='0x005C5A42' pattern='1111110100111111' width='3' />"
        L"<print color='0x00000000' slave_color='0x005C5A42' pattern='1111110100111111' width='3' />"
        L"</train>"
        L"<train num_diapason='9751-9798' name='����� 9000 � � �����'>"
        L"<screen color='0x0000FF00' slave_color='0x005C5A42' />"
        L"<print color='0x0000FF00' slave_color='0x005C5A42' />"
        L"</train>"
        L"<silence name='���������\\���������� �������' >"
        L"<screen color='0x000EFFFF' slave_color='0x005C5A42' />"
        L"<print color='0x000EFFFF' slave_color='0x005C5A42' />"
        L"</silence>"
        L"<busy name='��������� �����-����������� �����'>"
        L"<screen color='0x002068FF' slave_color='0x005C5A42' width='1'/>"
        L"<print color='0x002068F0' slave_color='0x005C5A42' width='1'/>"
        L"</busy>"
        L"<asoup name='���� �����'>"
        L"<screen color='0x00E1E4FF' slave_color='0x005C5A42' width='1'/>"
        L"<print color='0x00000000' slave_color='0x005C5A42' width='1'/>"
        L"</asoup>"
        L"<limit name='��������������'>"
        L"<screen color='0x000EFFFF' slave_color='0x005C5A42' />"
        L"<print color='0x000EFFFF' slave_color='0x005C5A42' />"
        L"</limit>"
        L"<window name='����'>"
        L"<screen color='0x000000F0' slave_color='0x005C5A41' width='1'/>"
        L"<print color='0x000000FF' slave_color='0x005C5A42' width='1'/>"
        L"</window>"
        L"</category>";

    attic::a_document x_doc;
    x_doc.load_wide( pe_num_text );

    ShowCategoryTable sct( x_doc );

    //����� ��� ������
    ShowCategory shc;
    //�������������������� ������
    HemOpt hemOpt;
    std::set<TrainCharacteristics::TrainFeature> tfeatures;
    CPPUNIT_ASSERT( sct.GetTrainCategory(LRT_HAPPEN, 0, false, tfeatures, shc) );
    ShowParams opt = shc.GetParam(LayerOpt(true, true), hemOpt, nullptr);
    CPPUNIT_ASSERT( opt.GetColor().ToCOLORREF()==0x00FFFFFF && 
        opt.GetWidth()==1 && 
        opt.GetPattern().to_str()=="1111111111111111");
    opt = shc.GetParam(LayerOpt(true, false), hemOpt, nullptr);
    CPPUNIT_ASSERT( opt.GetColor().ToCOLORREF()==0x00FFFFF0 && 
        opt.GetWidth()==1 && 
        opt.GetPattern().to_str()=="1111111111111111");
    //�������������� ������
    CPPUNIT_ASSERT( sct.GetTrainCategory(LRT_HAPPEN, 0, true, tfeatures, shc) );
    opt = shc.GetParam(LayerOpt(true, true), hemOpt, nullptr);
    CPPUNIT_ASSERT( opt.GetColor().ToCOLORREF()==0x00FF0000 && 
        opt.GetWidth()==2 && 
        opt.GetPattern().to_str()=="1111111111111111");
    opt = shc.GetParam(LayerOpt(true, false), hemOpt, nullptr);
    CPPUNIT_ASSERT( opt.GetColor().ToCOLORREF()==0x005C5AFF && 
        opt.GetWidth()==1 && 
        opt.GetPattern().to_str()=="1111111111111111");

    //��������� ������
    tfeatures.insert(TrainCharacteristics::TrainFeature::Thrown);
    CPPUNIT_ASSERT( sct.GetTrainCategory(LRT_HAPPEN, 0, true, tfeatures, shc) );
    opt = shc.GetParam(LayerOpt(true, true), hemOpt, nullptr);
    CPPUNIT_ASSERT( opt.GetColor().ToCOLORREF()==0x000F0000 && 
        opt.GetWidth()==1 && 
        opt.GetPattern().to_str()=="1111111111111111");
    opt = shc.GetParam(LayerOpt(true, false), hemOpt, nullptr);
    CPPUNIT_ASSERT( opt.GetColor().ToCOLORREF()==0x005C5AF0 && 
        opt.GetWidth()==1 && 
        opt.GetPattern().to_str()=="1111111111111111");

    tfeatures.clear();
    CPPUNIT_ASSERT( sct.GetTrainCategory(LRT_HAPPEN, 74, false, tfeatures, shc) );
    opt = shc.GetParam(LayerOpt(true, true), hemOpt, nullptr);
    CPPUNIT_ASSERT( opt.GetColor().ToCOLORREF()==0xFF && 
        opt.GetWidth()==2 && 
        opt.GetPattern().to_str()=="1111111111111111");
    opt = shc.GetParam(LayerOpt(true, false), hemOpt, nullptr);
    CPPUNIT_ASSERT( opt.GetColor().ToCOLORREF()==0x005C5A42 && 
        opt.GetPattern().to_str()=="1111111111111111");
    
    //����� ��� ������
    sct.prepare_category(true); 
    CPPUNIT_ASSERT( sct.GetTrainCategory(LRT_HAPPEN, 3450, false, tfeatures, shc) );
    opt = shc.GetParam(LayerOpt(true, true), hemOpt, nullptr);
    CPPUNIT_ASSERT( opt.GetColor().ToCOLORREF()==0x00000000 && 
        opt.GetWidth()==3 && 
        opt.GetPattern().to_str()=="1111110100111111");
    opt = shc.GetParam(LayerOpt(true, false), hemOpt, nullptr);
    CPPUNIT_ASSERT( opt.GetColor().ToCOLORREF()==0x005C5A42 && 
        opt.GetPattern().to_str()=="1111110100111111");
    CPPUNIT_ASSERT( sct.GetTrainCategory(LRT_HAPPEN, 799, false, tfeatures, shc)==false );

    //����� ������ ��� �����
    CPPUNIT_ASSERT( sct.GetLayerCategory(LRT_BUSY, LayerOpt(), shc) );
    opt = shc.GetParam(LayerOpt(true, true), hemOpt, nullptr);
    CPPUNIT_ASSERT( opt.GetColor().ToCOLORREF()==0x002068F0 && 
        opt.GetWidth()==1 && 
        opt.GetPattern().to_str()=="1111111111111111");
    opt = shc.GetParam(LayerOpt(true, false), hemOpt, nullptr);
    CPPUNIT_ASSERT( opt.GetColor().ToCOLORREF()==0x005C5A42 && 
        opt.GetPattern().to_str()=="1111111111111111");

    //����� ������ ��� �����
    sct.prepare_category(false);
    CPPUNIT_ASSERT( sct.GetLayerCategory(LRT_FORBID, LayerOpt(), shc) );
    opt = shc.GetParam(LayerOpt(true, true), hemOpt, nullptr);
    CPPUNIT_ASSERT( opt.GetColor().ToCOLORREF()==0xF0 && 
        opt.GetWidth()==1 && 
        opt.GetPattern().to_str()=="1111111111111111");
    opt = shc.GetParam(LayerOpt(true, false), hemOpt, nullptr);
    CPPUNIT_ASSERT( opt.GetColor().ToCOLORREF()==0x005C5A41 && 
        opt.GetPattern().to_str()=="1111111111111111");

}

void TC_ShowCategory::setUp()
{
}

void TC_ShowCategory::tearDown()
{
}
