#ifndef TC_BBX_H
#define TC_BBX_H
#include <atomic>
#include <cppunit/extensions/HelperMacros.h>
#include "../BlackBox/bbx_BlackBox.h"
/* 
 * ����� ��� �������� ������ ������� BbxIn � BbxOut
 */
namespace Bbx { class Location; };

class TC_Bbx : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Bbx );

  CPPUNIT_TEST(Create);                  /* �������� ���� ������� �������� � ������������ */
  CPPUNIT_TEST(Search);                  /* �������� ����������� ������ ������� ������ */
  CPPUNIT_TEST(RemoveOldFiles);          /* �������� �������� ���������� ������ */
  CPPUNIT_TEST(ConstrainSpace);          /* ���������� ����������� ����� �� ����� */
  CPPUNIT_TEST(ConstrainTime);           /* ���������� ����������� �� ��������� �������� */
  CPPUNIT_TEST(ConstrainValue);          /* ������ ����������� */
  CPPUNIT_TEST(ContinuousTruncation);    /* � ����� 60 �������� �� 0.5 ������� */
  CPPUNIT_TEST(Records);                 /* �������� ������ ������ ������� ������� */
  CPPUNIT_TEST(Silence);
  CPPUNIT_TEST(AsyncStressTest);         /* ����� �������� ������ ��� ������� ����� ��������� */
  CPPUNIT_TEST(Lacuna);                  /* �������� ����������� ����� */
  CPPUNIT_TEST(ReadBorder);              /* �������� ������ ������ */
  CPPUNIT_TEST(Req321_SupportStartFile); /* �������� ������ ����� ������ � ������� ������ */
  CPPUNIT_TEST(DisRemoveOldFiles);       /* �������� ���������� ������ ������ ��� ������ */ 
  CPPUNIT_TEST(SizeParameters);          // ���������� ������, ������ ������� ��������� ������ ����� 
  CPPUNIT_TEST( FileNames_recreate );    // �������� ��������� ������ � ���������� ������� 
  CPPUNIT_TEST( FileNames_continuous );  // �������� ��������� ������ � ���������� ������� 
  CPPUNIT_TEST(ReaderSeeWriter);         /* �������� ������ �������� � ������� �������� */
  CPPUNIT_TEST(SmallRecordsSupport);     /* �������� �� ��������� ������������ �������� ���� ��������� */
  CPPUNIT_TEST(SmallRecordsReading);     /* �������� �� ��������� ������ ���������� ��������� ��������� */
  CPPUNIT_TEST(SmallIncrementRecordsSlicing); /* ����������� ������� � ������ ���������� ������������ ������� */
  CPPUNIT_TEST(IdentifierArifmetics);    /* �������� �������������� */
  CPPUNIT_TEST(PushStressTest);
  CPPUNIT_TEST(Compatible_NameLess);
  CPPUNIT_TEST(StoreTimeZone);
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();

    static size_t DeleteBlackBoxFiles(const Bbx::Location& location);

protected:
	void Create();
    void Records();
    void Search();
    void Silence();
    void AsyncStressTest();
    void FileNames_recreate();   // ��������� ������� ������
    void FileNames_continuous(); // ����������� ������ � ����� ������
    void SizeParameters();
	void RemoveOldFiles();// ������� ���������� ������
    void ConstrainSpace(); // ����������� ����� �� �����
    void ConstrainTime();  // ����������� ������� ��������
    void ConstrainValue(); // ������ ���������

    void fill_sample_with_time( std::string &body, time_t t, size_t i );
    void ContinuousTruncation();
	void DisRemoveOldFiles();// ������������� ������� ���������� ������ ��� ������ �� ���
	void ReadBorder();// ������� ������
    void Req321_SupportStartFile();
    void Lacuna();
    void ReaderSeeWriter();
	void SmallRecordsSupport(); // ����������� ������� � ������� ����� ���������
	void SmallRecordsReading(); // ����������� ������� � ������� ��������� ����������� �����
    void SmallIncrementRecordsSlicing(); // ����������� ������� � ������ ���������� ������������ �������
    void IdentifierArifmetics();
    void PushStressTest();
    void Compatible_NameLess(); // ������������� ��������� ���� ������ �� ������ �������
    void StoreTimeZone();   // ���������� ��������� ���� � ��������� �����
private:
    static time_t fixTm();

    void write_sup( Bbx::Writer& bOut, time_t t, const Bbx::char_vec& support ) const;
    void write_inc( Bbx::Writer& bOut, Bbx::char_vec*, time_t t, const Bbx::char_vec& before, const Bbx::char_vec& after_ ) const;
    static Bbx::char_vec minus( const Bbx::char_vec& total, const Bbx::char_vec &del );
    static Bbx::char_vec plus( const Bbx::char_vec& total, const Bbx::char_vec& add );
    Bbx::char_vec Req25_write_helper( const Bbx::Location& loc, const Bbx::char_vec& absolute_b );
    void Req25_read_helper( const Bbx::Location& loc, const bool direction,
                            const Bbx::char_vec& relat_b, const Bbx::char_vec& relat_e );
    void search_pause( Bbx::Writer &out_bbx );
    void search_addSupport( int shift, Bbx::Writer &out_bbx, std::vector<int> &supp );
    void addSupport( time_t moment, Bbx::Writer &out_bbx );
    std::vector<int> search_make_checkpoint( const std::vector<int>& supp );


private:
    time_t fix_moment;
    static const size_t BBX_COUNT = 3; 
    Bbx::Location BbxLocation[BBX_COUNT];
    Bbx::Identifier defaultId;
    boost::posix_time::time_duration restore_delay;
};

#endif // TC_BBX_H