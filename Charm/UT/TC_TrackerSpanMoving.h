#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//����� �� ���������� ��������

class TestTracker;
class TC_TrackerSpanMoving : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerSpanMoving );
	CPPUNIT_TEST( PushForwardOnDisappear );
	CPPUNIT_TEST( Jump );
	CPPUNIT_TEST_SUITE_END();

	void PushForwardOnDisappear(); //������������� ������� ������� ������ ������ ��� ������������ ������� �����
	void Jump(); //������ ����� �������
};