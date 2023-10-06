#pragma once
#include "../Docker/dk_docker.h"
#include "../helpful/Dictum.h"

// ��������� ������� ������ (�����, ����, ...)
// ������� �����
class MsgHard
{
public:
    MsgHard( boost::string_ref _str );
    void set( boost::string_ref _str );
    bool empty() const;
    bool put();
    bool get( boost::posix_time::time_duration _duration );
    const std::string& content() const;
    boost::posix_time::ptime time() const;
private:
    std::string m_content; // ����� ���������
    boost::posix_time::ptime m_time; // ������ ��������  (��� ������) ���������
    // NVI
    virtual bool putImp() =0;
    virtual bool getImp( boost::posix_time::time_duration _duration ) = 0;
};

class MsgHardFile : public MsgHard
{
public:
    MsgHardFile( boost::string_ref _text, std::wstring _folder, std::wstring _fname  ); // ��� ������
    MsgHardFile( std::wstring _folder, std::wstring _fmask ); // ��������� ��� ������
    std::wstring fileNameOnly() const;
    std::wstring fileNameFull() const;
private:
    std::wstring m_fname; // ��� �����
    std::wstring m_fmask; // ����� �����
    std::wstring m_folder; // �����

    bool putImp() override;
    bool getImp( boost::posix_time::time_duration _duration ) override;
};

class MsgHardXml : public MsgHardFile
{
public:
    MsgHardXml( boost::string_ref _text, std::wstring _folder, std::wstring _fname  ); // ��� ������
    MsgHardXml( std::wstring _folder, std::wstring _fmask ); // ��������� ��� ������
    std::string contentEx() const;
private:
    bool putImp() override;
};

#ifndef LINUX
class UtDatagram;
class MsgHardUdp : public MsgHard
{
public:
	MsgHardUdp(UtDatagram& _dg);
private:
	UtDatagram& m_dg;
	bool getImp(boost::posix_time::time_duration _duration) override;
	bool putImp() override;
};
#endif // !LINUX

// ��������� �������� ������ (����� Fund � Haron)
class MsgSoft
{
public:
    MsgSoft( Network::Docker& _docker ); // ��� ������ �� ������
    MsgSoft( boost::string_ref _data, Network::Docker& _docker ); // ��� ������ � �����
    bool empty() const;
    bool put();
    bool get( boost::posix_time::time_duration _duration );
    // ���� � ������ ���������
    DICTUM_KIND::VALUE_TYPE kind() const;
    boost::string_ref content() const;
    boost::posix_time::ptime time() const;
    // ��������� ������ ��������
    void pop();
    // �������� ��������� ������������ ���������
    std::vector<char> get_raw( boost::posix_time::time_duration _duration );
    // ��������� ��������� ������������ ���������
    bool put_raw( boost::string_ref _data );

public:
    std::unique_ptr<RichDictum> m_rd; // ��������� ������� ��� �������� �� ������ ���������
    bool m_manual; // ��� ��������� ������� ������� (�.�. ���� ���)
private:
    Network::Docker& m_docker;
    boost::posix_time::ptime m_time; // ������ ��������  (��� ������) ���������
    virtual bool putImp() =0;
    virtual bool getImp( boost::posix_time::time_duration _duration ) =0;
};

class MsgSoftXml : public MsgSoft
{
public:
    MsgSoftXml( Network::Docker& _docker );
    MsgSoftXml( boost::string_ref _data, Network::Docker& _docker, std::wstring _fname );
private:
    bool putImp() override;
    bool getImp( boost::posix_time::time_duration _duration ) override;
};

class MsgSoftGui : public MsgSoftXml
{
public:
    MsgSoftGui( Network::Docker& _docker );
    MsgSoftGui( boost::string_ref _data, Network::Docker& _docker, std::wstring _fname );
    std::string contentInside( int coding ) const;    // ���������� ��� ������ �� ���
    std::wstring filenameInside() const; // �������� ����� ��� ������
};
