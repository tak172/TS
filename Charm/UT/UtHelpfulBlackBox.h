#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../BlackBox/bbx_BlackBox.h"
#include "../Fund/BbxConst.h"
#include "TC_Bbx.h"

const std::string CONTENT_ATTR = "forTest";
const std::string CONTENT_SUP = "+++";
const std::string CONTENT_INC = "-?-";
const std::string CONTENT_PACKAGE_IN = "pin";
const std::string CONTENT_PACKAGE_OUT = "out";

const int c_BbxBegin = 100;
const int c_LacunaAft = 200; // лакуна ПОСЛЕ этого момента (Begin=201) --> Aft=200
const int c_LacunaEnd = 400; // лакуна ПЕРЕД этим моментом (End=400)     --> Bef=400 
const int c_BbxEnd = 500;


/*
вспомогательные классы для создания ЧЯ
*/
// запись опорная
class Sup
{
public:
    Sup(time_t _mom, int _special = 0) 
        : mom(_mom), special(_special)
    {}
    time_t getTime() const
    {
        return mom;
    }
    int getSpecial() const
    {
        return special;
    }
private:
    time_t mom;
    int special;
};

// запись инкрементная
class Inc : public Sup 
{
public:
    Inc(time_t _mom, int _special = 0)
        : Sup(_mom, _special)
    {};
};

class PackageIn : public Sup
{
public:
    PackageIn(time_t _mom, int _special = 0)
        : Sup(_mom, _special)
    {};
};

class PackageOut : public Sup
{
public:
    PackageOut(time_t _mom, int _special = 0)
        : Sup(_mom, _special)
    {};
};

Bbx::Writer& operator<<(Bbx::Writer& bbx, const Sup& rec);
Bbx::Writer& operator<<(Bbx::Writer& bbx, const Inc& rec);
Bbx::Writer& operator<<(Bbx::Writer& bbx, const PackageIn& rec);
Bbx::Writer& operator<<(Bbx::Writer& bbx, const PackageOut& rec);

void DeleteBlackBox(const Bbx::Location& bbxLocation);

time_t PrepareBlackBox(Bbx::Location& bbxLocation, std::tuple<unsigned, unsigned, unsigned, unsigned> *count = nullptr);