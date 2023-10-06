#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TC_TrackerChangings : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();
	virtual void tearDown();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerChangings );
	CPPUNIT_TEST( StandardPassing );
	CPPUNIT_TEST( ElementaryMove );
	CPPUNIT_TEST( OneEndBlinking );
	CPPUNIT_TEST( UTurn );
	CPPUNIT_TEST( LengthyLiberationWithFiltering );
	CPPUNIT_TEST( UnknownUnityADWayExitNGoSpan );
	CPPUNIT_TEST( UnknownUnityADWayExitNGoThroughSema );
	CPPUNIT_TEST( HeadMotionsOnSetTrainInfo );
	CPPUNIT_TEST( HeadMotionsOnUnsetTrainInfo );
	CPPUNIT_TEST( HeadMotionsOnChangeInfoForUnmovingTrain );
	CPPUNIT_TEST( DisappearWithRecovery );
	CPPUNIT_TEST( DisappearWithoutRecovery );
	CPPUNIT_TEST( ForceDisappearAfterRouteSet );
	CPPUNIT_TEST( InfoExtensionNoCausesExternalTrainChanges );
	CPPUNIT_TEST_SUITE_END();

	void StandardPassing(); //����������� ������ ������ ����� �/� ���� �������
	std::wstring StandardPassingInput(); //������� - ����� ������
	void StandardPassingInputCheck( std::wstring trainNum );
	void StandardPassingOutput();
	void StandardPassingOutputCheck( std::wstring trainNum );
	void ElementaryMove(); //��������� ������ � ����������� �� ���� �������
	void OneEndBlinking(); //������� ������� �� ����� ����� ������
	void UTurn(); //�������� ������
	void LengthyLiberationWithFiltering(); //������������ ���������� �������� (��� ������� ������ ����������)
	void UnknownUnityADWayExitNGoSpan(); //����� � �/� ���� �� ���������� ������ ��������� ��������� � ����� �� �������
	void UnknownUnityADWayExitNGoThroughSema(); //����� � �/� ���� �� ���������� ������ ��������� ��������� � ����������� ������ ����� �������� �������� ��������
	void HeadMotionsOnSetTrainInfo(); //����������� �� ���������� ������ ��� ��������� �����������
	void HeadMotionsOnUnsetTrainInfo(); //����������� �� ���������� ������ ��� ������ �����������
	void HeadMotionsOnChangeInfoForUnmovingTrain(); //����������� �� ���������� ������ ��� ����� ����������� (����� �� ���������)
	void DisappearWithRecovery(); //������������ ������ � ��������� ���������������
	void DisappearWithoutRecovery(); //������������ ������ ��� ���������� ��������������
	void ForceDisappearAfterRouteSet(); //����������� �� ������������ ����� ������� �������� ������ ������������ ������
	void InfoExtensionNoCausesExternalTrainChanges(); //��������� ���������� �� ������ ��������� � �������������� ������������ �� ������� �������
};