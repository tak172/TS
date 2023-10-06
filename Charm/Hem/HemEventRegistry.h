#pragma once

#include "../helpful/RT_Singleton.h"
#include "../helpful/X_translate.h"

struct HemEventDescr;

enum struct HCode
{
    FORM = 0,	   ///< возникновение поезда
    EXPLICIT_FORM, ///< явное формирование идентифицированного поезда
    DISFORM,	 ///< расформирование поезда
    DEATH,		 ///<исчезновение из системы отслеживания (по сигналам занятости)
    ARRIVAL,     ///< прибытие
    DEPARTURE,  ///< отправление
    TRANSITION, ///< проследование
    SPAN_MOVE,  ///< движение по перегону
    INFO_CHANGING, ///< изменение базовой информации (номер, индекс, ФИО)
    ENTRANCE_STOPPING, ///< остановка перед входным светофором
    SPAN_STOPPING_BEGIN, ///< начало стоянки на перегоне
    SPAN_STOPPING_END, ///< окончание стоянки на перегоне
    WRONG_SPANWAY, ///< выход на неправильный путь перегона
    TOKEN_SPANWAY, ///< сведения о пути перегона
    POCKET_ENTRY, ///< вход в "карман"
    POCKET_EXIT, ///< выход из "кармана"
    STATION_ENTRY, ///< вход на станцию (создается при необходимости)
    BANNED_ENTRY, ///< вход на станцию по запрещающему сигналу
    STATION_EXIT, ///< выход со станции (создается при необходимости)
    THROW, ///< бросание поезда
    RAISE, ///< поднятие поезда
    WAY_RED,     ///< поступил сигнал "занятость"   приемо-отправочного пути
    WAY_BLACK,   ///< поступил сигнал "свободность" приемо-отправочного пути
    CUTOFF, ///< Потеря связи со станцией
    CUTIN, ///< Возобновление связи со станцией
    ASOUP_UNKNOWN, ///< Сообщение АСОУП без точной привязки к месту (парк/путь)
    LIMIT, ///< предупреждение
    FORBID  ///< окно
};

class HemEventRegistry
{
    DECLARE_SINGLETON(HemEventRegistry);

public:
    enum Allowably
    {
        ALW_NONE = 0,         // никаких разрешений
        ALW_AUTOCHART  = 0x1, // разрешение для раздела AutoChart
        ALW_USERCHART  = 0x2, // разрешение для раздела UserChart
        ALW_TERRACHART = 0x4, // разрешение для раздела TerraChart
        ALW_ALL = ALW_AUTOCHART|ALW_USERCHART|ALW_TERRACHART, // разрешение для всех разделов
    };

    bool CanBeStarted( HCode new_code ) const; //может ли нить начинаться с этого события
    bool CanBeFollowed( HCode last_code, HCode new_code ) const; //может ли новое событие следовать за этим последним
    bool CodeIsValid( HCode ) const;
    std::shared_ptr<HCode> GetEventCode( const std::string& xml_name ) const;
    bool GetEventCodeEx( const char* xml_name, HCode& code ) const;
    const std::string& GetXMLName( HCode ) const;
    std::wstring getUserName( HCode ) const;
    //
    static bool MustBeOnStation( HCode code );
    static bool MustBeOnSpan( HCode code );
    static bool IsSpecialClosedCode( HCode code );
    static bool isDepartureCode( const HCode code );
    static bool isArrivalCode( const HCode code );
    static bool isFormCode( const HCode code );

private:
    std::map <HCode, HemEventDescr> registry;
    typedef std::pair<HCode,HCode> SequenceKey;
    typedef std::map < SequenceKey, Allowably > PermissionTable;
    PermissionTable seqtable;
    void CreateSequenceTable();
};
