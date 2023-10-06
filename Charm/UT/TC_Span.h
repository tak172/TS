#ifndef TC_SPAN_H
#define TC_SPAN_H

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/EsrGuide.h"

/// \brief ���� ��� Mission::StationControl_Base
class TC_Span : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Span );
  CPPUNIT_TEST( way_normal );
  CPPUNIT_TEST( reverse_sema_parity );
  CPPUNIT_TEST( unknown_sema_parity );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
	virtual void tearDown();

protected:
	void way_normal();          // ���������� �������
    void reverse_sema_parity(); // ��������� ��������� ����� �������� �� ����� �����
    void unknown_sema_parity(); // �������������� �������� ���������� � ��������

private:
    EsrGuide esrGuide;
    std::vector<BadgeE> chain;

    static EsrKit TERM1, TERM2, SPAN12;
    static std::vector<BadgeE> make_chain( std::string items, EsrKit esr, std::wstring ruler );
};


#endif // TC_SPAN_H