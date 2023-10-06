#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//��������� ����� �������� ����������

class TC_TrackerInfoSetting : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerInfoSetting );
	CPPUNIT_TEST( PlaceChangings );
	CPPUNIT_TEST( NoTrainInfoSetting );
	CPPUNIT_TEST( ArrivedNoDepartured );
	CPPUNIT_TEST( ArrivedThenDepartured );
	CPPUNIT_TEST( InfoSettingBeforeArriving );
	CPPUNIT_TEST( FormedNoDepartured );
	CPPUNIT_TEST( FormedThenDepartured );
	CPPUNIT_TEST( FullyInSpan );
	CPPUNIT_TEST( HouseHold );
	CPPUNIT_TEST( ArrivalFactExpiration );
	CPPUNIT_TEST( ArrivalExpirationWhileLPFDelay );
	CPPUNIT_TEST( NotrainToTrain );
	CPPUNIT_TEST( DuplicateInfoMustBeErased );
	CPPUNIT_TEST( DuplicateInfoForDifferentUnities );
	CPPUNIT_TEST( DuplicateInfoForDifferentUnities2 );
	CPPUNIT_TEST( AddingIncompleteInfo );
	CPPUNIT_TEST( UndefOddnessAlsoApplyable );
	CPPUNIT_TEST( NearNoTrainInfoSettingMakesMoving );
	CPPUNIT_TEST( FormAfterDelayedForm );
	CPPUNIT_TEST( SetInfoOnTechnodeWithOddnessIgnore );
	CPPUNIT_TEST( SetInfoOnTechnodeWOOddnessIgnore );
	CPPUNIT_TEST( TransitionShouldCleanDelayedInfo );
	CPPUNIT_TEST( HittedExtDisformSuccess );
	CPPUNIT_TEST( HittedExtDisformReject );
	CPPUNIT_TEST( HittedExtDisformThruMoving );
	CPPUNIT_TEST( HittedExtDisformThruMoving2 );
	CPPUNIT_TEST( HittedExtDisformThruMoving3 );
	CPPUNIT_TEST( MissedExtDisformUnsuccess );
	CPPUNIT_TEST( MissedExtDisformExpired );
	CPPUNIT_TEST( MissedExtDisformAlienNumber );
	CPPUNIT_TEST( MissedExtDisformArrivedWithDisform );
	CPPUNIT_TEST( MissedExtDisformArrivedWODisform );
	CPPUNIT_TEST( OneMomentDisformAndForm );
	//CPPUNIT_TEST( NoNumberDisformForbidTransition );
	//CPPUNIT_TEST( NoNumberOverlappedDisformForbidTransition );
	CPPUNIT_TEST( HittedNoNumberDisform );
	CPPUNIT_TEST( MissedNoNumberDisform );
	CPPUNIT_TEST( DelayedInfoReplacement );
	CPPUNIT_TEST( TrainsMigrationAfterTrainInfoDuplication );
	CPPUNIT_TEST( TrainsMigrationAfterTrainInfoDuplication2 );
	CPPUNIT_TEST( BunchOfIdenticalNumbers );
	CPPUNIT_TEST_SUITE_END();

	void PlaceChangings(); //�������� �� ��������� ��������� ����, ������������ �� Fund

	//���������� ���������� ��� ����������
	void NoTrainInfoSetting();
	//����� ������ �� ��-����, �� �� ����������
	void ArrivedNoDepartured();
	//����� ������ �� ��-���� � ����������
	void ArrivedThenDepartured();
	//���������� ���������� �� �������� ������
	void InfoSettingBeforeArriving();
	//����� ������������� �� ��-����, �� ��������, �� �� ����������
	void FormedNoDepartured();
	//����� ������������� �� ��-���� � ����������
	void FormedThenDepartured();
	//�� ��������� �� ��������
	void FullyInSpan();
	//��������� ���� ��� ������������� �������
	void HouseHold();
	//������, ����� ����� ��� � �� ����� ������� �� ��-����, ������� ��������� ��� ��������
	void ArrivalFactExpiration();
	//�������� ���������� ��� �������� � �������� �������� �������� �������
	void ArrivalExpirationWhileLPFDelay();
	//�������� ������ �� ����������
	void NotrainToTrain();
	//����������� ���������� �� ������ ����������������
	void DuplicateInfoMustBeErased();
	//������������ ������ � ���� �� ������ �� ������ ��
	void DuplicateInfoForDifferentUnities();
	void DuplicateInfoForDifferentUnities2();
	//��������� �� ������� ���������� �� ������ ������� ��� ������������ ����������
	void AddingIncompleteInfo();
	//�������������������� ����� � ����������� ��������� �� ��������� ��������� ����������� �� ��������
	void UndefOddnessAlsoApplyable();
	//���������� ���������� �� ���������� ����� � �������, ��� �� �������, �������� �� ������������ ������ ������, � � �������� �������������
	void NearNoTrainInfoSettingMakesMoving();
	//������ ������������ ������, �������������� ���������� �� ������������ ����������
	void FormAfterDelayedForm();
	//��������� ������ �� ����� �� ���.���� � ��������� ������������� ��������
	void SetInfoOnTechnodeWithOddnessIgnore();
	//��������� ������ �� ����� �� ���.���� ��� �������� ������������� ��������
	void SetInfoOnTechnodeWOOddnessIgnore();
	//������������� ������ ����� ����, �� ������� �������� ��������� ��� �������� ������ ������� ������ ��������� �� ������� �������� ������������
	void TransitionShouldCleanDelayedInfo();
	//������������� ��������������� � ������������ ������ ������ (�� ������� �������)
	void OneMomentDisformAndForm();

	void HittedExtDisformSuccess(); //���������� ��������� ��������������� �� ������� ������� (�������)
	void HittedExtDisformReject(); //���������� ��������� ��������������� �� ������� ������� (�����)
	void HittedExtDisformThruMoving(); //���������� ��������� ��������������� �� ������� ������� �� �����, ������� ���������� ����� ���������� ��-����
	void HittedExtDisformThruMoving2(); //�������� �� ����������� ������� �����������
	void HittedExtDisformThruMoving3(); //������ �������� ������
	void MissedExtDisformUnsuccess(); //������ ���������� ��������������� �� ������� ������� (���������� ����������)
	void MissedExtDisformExpired(); //������ ���������� ��������������� �� ������� ������� (��������� ��������� �� ���������� �������)
	void MissedExtDisformAlienNumber(); //������ ���������� ��������������� �� ������� ������� (��������� �� ����������� ��-�� ����������� �������)
	void MissedExtDisformArrivedWithDisform(); //������ ���������� ��������������� �� ������� ������� (�������� �������� ������ � ����������� ����������������)
	void MissedExtDisformArrivedWODisform(); //������ ���������� ��������������� �� ������� ������� (�������� ������ ��� ���������������)
	void NoNumberDisformForbidTransition(); //��������� ��������������� ��� ������ ��������� �������������
	void NoNumberOverlappedDisformForbidTransition(); //��������� ��������������� ��� ������ ��������� ������������� (������ ��������� ������� ����������)
	void HittedNoNumberDisform(); //��������� ��������������� ��� ������ �������� � ����� (�����������)
	void MissedNoNumberDisform(); //��������� ��������������� ��� ������ �� �������� � ������ �� �� (�� �����������)
	void DelayedInfoReplacement(); //� ������ ������� ���������� ����������� �� ���� � ��� �� ���� �� ������������ ������� �� ��� ������� ��������� ������ ���������
	void TrainsMigrationAfterTrainInfoDuplication(); //��� ���������� ���������� � ������������ �����/�������� ����������� ����� ��� ����������� �������� ����������� �� ����� �����
	void TrainsMigrationAfterTrainInfoDuplication2();
	void BunchOfIdenticalNumbers(); //������������� ���������� ������ ����������� � ����������� ��������

	//��������������� �������
	bool CheckPermitted( unsigned int /*prevIden*/, const TrainDescr & /*newDescr*/ );
	bool CheckForbidden( unsigned int /*prevIden*/, ConstTrainDescrPtr /*prevDescrPtr*/, const TrainDescr & /*newDescr*/ );
	bool CheckRecreated( unsigned int /*prevIden*/, ConstTrainDescrPtr /*prevDescrPtr*/, const TrainDescr & /*newDescr*/ );
	bool CheckForbiddenThenPermittedAfterArrive( unsigned int /*prevIden*/, ConstTrainDescrPtr /*prevDescrPtr*/, const TrainDescr & /*newDescr*/ );
	bool CheckForbiddenThenRecreationAfterArrive( unsigned int /*prevIden*/, ConstTrainDescrPtr /*prevDescrPtr*/, const TrainDescr & /*newDescr*/ );
	bool CheckHouseHold( unsigned int /*prevIden*/, ConstTrainDescrPtr /*prevDescrPtr*/, const TrainDescr & /*newDescr*/ );
};