#pragma once
#include "../Docker/dk_docker.h"
#include "../helpful/Dictum.h"

// сообщения нижнего уровня (АСОУП, Биск, ...)
// базовый класс
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
    std::string m_content; // текст сообщения
    boost::posix_time::ptime m_time; // момент создания  (или приема) сообщения
    // NVI
    virtual bool putImp() =0;
    virtual bool getImp( boost::posix_time::time_duration _duration ) = 0;
};

class MsgHardFile : public MsgHard
{
public:
    MsgHardFile( boost::string_ref _text, std::wstring _folder, std::wstring _fname  ); // для записи
    MsgHardFile( std::wstring _folder, std::wstring _fmask ); // заготовка для чтения
    std::wstring fileNameOnly() const;
    std::wstring fileNameFull() const;
private:
    std::wstring m_fname; // имя файла
    std::wstring m_fmask; // маска файла
    std::wstring m_folder; // папка

    bool putImp() override;
    bool getImp( boost::posix_time::time_duration _duration ) override;
};

class MsgHardXml : public MsgHardFile
{
public:
    MsgHardXml( boost::string_ref _text, std::wstring _folder, std::wstring _fname  ); // для записи
    MsgHardXml( std::wstring _folder, std::wstring _fmask ); // заготовка для чтения
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

// Сообщения верхнего уровня (между Fund и Haron)
class MsgSoft
{
public:
    MsgSoft( Network::Docker& _docker ); // для чтения из докера
    MsgSoft( boost::string_ref _data, Network::Docker& _docker ); // для записи в докер
    bool empty() const;
    bool put();
    bool get( boost::posix_time::time_duration _duration );
    // инфо о первом фрагменте
    DICTUM_KIND::VALUE_TYPE kind() const;
    boost::string_ref content() const;
    boost::posix_time::ptime time() const;
    // отбросить первый фрагмент
    void pop();
    // получить сообщение произвольной структуры
    std::vector<char> get_raw( boost::posix_time::time_duration _duration );
    // отправить сообщение произвольной структуры
    bool put_raw( boost::string_ref _data );

public:
    std::unique_ptr<RichDictum> m_rd; // созданное вручную или принятое из докера сообщение
    bool m_manual; // это сообщение создано вручную (т.е. типа нет)
private:
    Network::Docker& m_docker;
    boost::posix_time::ptime m_time; // момент создания  (или приема) сообщения
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
    std::string contentInside( int coding ) const;    // содержимое для отдачи во вне
    std::wstring filenameInside() const; // название файла для записи
};
