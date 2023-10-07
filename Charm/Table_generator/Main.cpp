#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <iomanip>

#include "Attic.h"
#include "Utf8.h"

#pragma warning(disable: 26451)

const std::string DIVIDINGLINE(115, '-');
const std::string SIDELINE(ToUtf8(L"|        |                               |                |                |                |                     |\n"));

struct Data
{
    std::wstring DK;    // обозначение
    std::string name;   // наименование
    int bit;            // смещение
    bool note;          // есть ли примечание

    Data(int bit, bool note) : bit(bit), note(note) {}
};

std::wstring MakeDesignation(std::string& section_name, std::string& first_attribute_name, bool waynum, int type)
{
    std::wstring designation_DK;

    if (section_name == "SEMALEGS")
    {
        if (type == 0) designation_DK += L"Поездн. светофор ";
        else if (type == 1) designation_DK += L"Маневр. светофор ";
        else if (type == 4) designation_DK += L"Пригл. светофор ";

        if (first_attribute_name == "allow") designation_DK += L"открыт";
        else if (first_attribute_name == "restrict") designation_DK += L"закрыт";
        else return {};
    }
    else if (section_name == "FIGURES")
    {
        designation_DK += L"Лампа";
    }
    else if (section_name == "STRIPS")
    {
        if (waynum) designation_DK += L"Путь ";
        else designation_DK += L"ИУ ";

        if (first_attribute_name == "used") designation_DK += L"занят";
        else if (first_attribute_name == "lock") designation_DK += L"в маршруте";
        else return {};
    }
    else if (section_name == "SWITCHES")
    {
        if (first_attribute_name == "positiv") designation_DK += L"Стрелка (плюс)";
        else if (first_attribute_name == "negativ") designation_DK += L"Стрелка (минус)";
    }

    return designation_DK;
}

void GetTs(std::vector<std::pair<std::pair<int, std::vector<std::string>>, Data>>& table, std::string& section_name, pugi::xml_node& element)
{
    for (auto ts : element.children("ts"))
    {
        std::string first_attribute_name = ts.first_attribute().name();
        int type = element.attribute("type").as_int();

        std::wstring designation_DK = MakeDesignation(section_name, first_attribute_name, element.attribute("waynum") != nullptr, type);

        // можно использовать std::optional или исключения
        if (designation_DK.size())
        {
            int ab = ts.attribute("ab").as_int();
            std::string signal_name = ts.first_attribute().as_string();

            // иду по всем данным
            for (auto& line : table)
            {
                bool used = false;

                // и по всем именам для LA
                for (auto& name : line.first.second)
                {
                    // если номер и имя совпадают, то к этому LA добавляю имя объекта и обозначение(DK)
                    if (line.first.first == ab && name == signal_name)
                    {
                        line.second.name = element.attribute("name").as_string();
                        line.second.DK = designation_DK;
                        used = true;
                        break;
                    }
                }
                if (used) break;
            }
        }
    }
}

void ReadingABC(std::vector<std::pair<std::pair<int, std::vector<std::string>>, Data>>& table, attic::a_document& abc_doc)
{
    attic::a_node abc_table = abc_doc.document_element();

    // нужен ли цикл по станциям, или в .аbc всегда 1 станция
    for (auto section : abc_table.child("Station").children())
    {
        for (auto kind : section.children("Kind"))
        {
            for (auto LTS : kind.children("LTS"))
            {
                // проверяю, есть ли такой LA, если есть, добавляю новое имя
                bool used = false;

                int LA = LTS.attribute("LA").as_int();

                for (auto& line : table)
                {
                    if (LA == line.second.bit)
                    {
                        used = true;
                        line.first.second.push_back(LTS.attribute("Name").as_string());
                        break;
                    }
                }

                // если нет - добавляю новый элемент без описания
                if (!used)
                {
                    table.push_back({ { section.attribute("Abonent").as_int(), { LTS.attribute("Name").as_string() } },
                        { LA, false } });
                }
            }
        }

        for (auto exp : section.children("Exp"))
        {
            // иду только по импульсам, тк у LTS в Exp нет LA
            for (auto impuls : exp.children("Impuls"))
            {
                bool used = false;

                for (auto& line : table)
                {
                    // если уже есть, то добавляю описание
                    if (impuls.attribute("LA").as_int() == line.second.bit)
                    {
                        line.second.note = true;
                        used = true;
                    }
                }

                // если нет - добавляю новый элемент c описанием
                if (!used)
                {
                    table.push_back({ { section.attribute("Abonent").as_int(), {} },
                        {impuls.attribute("LA").as_int(), true } });
                }
            }
        }
    }
}

void ReadingOEC(std::vector<std::pair<std::pair<int, std::vector<std::string>>, Data>>& table, attic::a_document& oec_doc)
{
    attic::a_node collection = oec_doc.document_element();

    for (auto section : collection.children())
    {
        std::string section_name = section.name();

        for (auto element : section.children("element"))
        {
            // если section_name == "SEMALEGS", сначала необходимо зайти в head
            if (section_name == "SEMALEGS")
            {
                for (auto head : element.children("head"))
                {
                    GetTs(table, section_name, head);
                }
            }
            // иначе сразу в ts
            else
                GetTs(table, section_name, element);
        }
    }
}

// подсчет однобайтовых символов в строке
int CountSingleByte(std::string& str)
{
    std::string pattern = "1234567890_-()./ ";

    int count = std::count_if(str.begin(), str.end(), [&pattern](char c)
        {
            return pattern.find(c) != std::string::npos;
        }
    );

    return count;
}

void Formatting(std::vector<std::pair<std::pair<int, std::vector<std::string>>, Data>>& table, std::ofstream& re_doc)
{
    re_doc << DIVIDINGLINE << '\n';
    re_doc << ToUtf8(L"|   N    |      Обозначение сигналов     |  Наименование  |   Состояние	   |    Смещение    |                     |\n");
    re_doc << ToUtf8(L"|  п/п   |          в системе ДК         |    объекта     |    объекта     |    в битовом   |      Примечание     |\n");
    re_doc << ToUtf8(L"|        |                               |                |                |     массиве    |                     |\n");
    re_doc << DIVIDINGLINE << '\n' << SIDELINE;

    size_t count = 1;

    // не нашел способа лучше средствами стандартной библиотеки
    for (auto& [first, second] : table)
    {
        re_doc << "| " << std::left << std::setw(7) << count
            << "| " << std::setw(30 + (ToUtf8(second.DK).size() - CountSingleByte(ToUtf8(second.DK))) / 2) << ToUtf8(second.DK)
            << "| " << std::setw(15 + (second.name.size() - CountSingleByte(second.name)) / 2) << second.name
            << "| " << std::setw(15) << ' '
            << "| " << std::setw(15) << second.bit << "| ";
        if (second.note) re_doc << std::setw(30) << ToUtf8(L"исп.в лог.обработке") << " |\n";
        else re_doc << std::string(19, ' ') << " |\n";

        re_doc << SIDELINE << DIVIDINGLINE << '\n';

        if (count < table.size())
            re_doc << SIDELINE;

        ++count;
    }

    re_doc.close();
}

int main(int argc, char* argv[])
{
    // с одним .abc могут быть связаны несколько файлов .oec
    if (argc < 4)
    {
        std::cerr << "Enter the path to one .abc file and one or more .oec files. Lastly, enter the path of the output(.re) file." << std::endl;
        return 1;
    }

    // пара по которой сопоставляем - число(Abonent), имена сигнала; данные - обозначение, наименование, бит, примечание(есть, нет)
    std::vector<std::pair<std::pair<int, std::vector<std::string>>, Data>> table;

    attic::a_document abc_doc;

    // На Windows - From1251()
    if (!abc_doc.load_file(FromUtf8(argv[1])))
    {
        std::cerr << "Failed to load hemalda.abc" << std::endl;
        return 1;
    }

    ReadingABC(table, abc_doc);

    attic::a_document oec_doc;

    for (int i = 2; i < argc - 1; ++i)
    {
        if (!oec_doc.load_file(FromUtf8(argv[i])))
        {
            std::cerr << "Failed to load hemalda.oec" << std::endl;
            return 1;
        }

        ReadingOEC(table, oec_doc);
    }

    // сортировка по битам
    std::sort(table.begin(), table.end(), [](std::pair<std::pair<int, std::vector<std::string>>, Data>& line1, std::pair<std::pair<int, std::vector<std::string>>, Data>& line2)
        {
            return line1.second.bit < line2.second.bit;
        }
    );

    std::ofstream re_doc(FromUtf8(argv[argc - 1]));

    if (!re_doc.is_open())
    {
        std::cerr << "Failed to create hemalda.re" << std::endl;
        return 1;
    }

    Formatting(table, re_doc);

    std::cout << "Success!" << std::endl;

    return 0;
}