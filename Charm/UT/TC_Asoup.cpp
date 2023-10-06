#include "stdafx.h"

#include "TC_Asoup.h"
#include "UtHelpfulDateTime.h"

#include "../Asoup/M1042.h"
#include "../Asoup/M904.h"
#include "../Asoup/M333.h"
#include "../helpful/Utf8.h"
#include "../Asoup/MsgPassengerInfo.h"
#include "../helpful/Attic.h"
#include "../helpful/ExceptionTracer.h"
#include "../Hem/AsoupEvent.h"
#include "../Hem/XlatEsrParkWay.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TC_Asoup);

CPPUNIT_NS_BEGIN

// Метода не хватает в CppUnit
void CPPUNIT_API assertDoubleEquals( double expected,
    double actual,
    double delta,
    SourceLine sourceLine, 
    const std::string &message )
{
    if ( std::abs(expected - actual) > std::abs(delta) )
    {
        Asserter::failNotEqual( assertion_traits<double>::toString(expected),
            assertion_traits<double>::toString(actual),
            sourceLine,
            message );
    }
}

CPPUNIT_NS_END

template<>
std::string CppUnit::assertion_traits<std::wstring>::toString( const std::wstring& x )
{
    std::string text = '"' + ToUtf8(x) + '"';    // adds quote around the string to see whitespace
    OStringStream ost;
    ost << text;
    return ost.str();
}

const float c_floatDelta = 0.0001f;

const wchar_t *c_904 = L"(:904 0705 3811 0700 51 0779 1 27 01 08 35 062 05442 6 0000 0 0\n"
    L" 01 58315946 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0228 0099 0710\n"
    L" 02 53347902 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0224 0106 0710\n"
    L" 03 53321444 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0226 0106 0710\n"
    L" 04 53355236 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0225 0106 0710\n"
    L" 05 54946876 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0227 0106 0710\n"
    L" 06 58325044 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0227 0106 0710\n"
    L" 07 58306267 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0223 0106 0710\n"
    L" 08 53331443 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0227 0106 0710\n"
    L" 09 58306523 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0223 0106 0710\n"
    L" 10 53353777 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0223 0106 0710\n"
    L" 11 59839639 0202 064 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0224 0095 0640\n"
    L" 12 53355921 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0225 0106 0710\n"
    L" 13 53356226 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0225 0106 0710\n"
    L" 14 59866749 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0235 0099 0705\n"
    L" 15 59250225 0202 064 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0220 0095 0640\n"
    L" 16 59236877 0202 064 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0220 0095 0640\n"
    L" 17 59250274 0202 064 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0220 0095 0640\n"
    L" 18 59838706 0202 064 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0218 0095 0640\n"
    L" 19 59040980 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0232 0095 0700\n"
    L" 20 59254979 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0225 0095 0700\n"
    L" 21 59255141 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0225 0095 0700\n"
    L" 22 59254953 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0225 0095 0700\n"
    L" 23 59264325 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0223 0095 0700\n"
    L" 24 59245589 0202 064 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0214 0095 0640\n"
    L" 25 53371456 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0225 0106 0710\n"
    L" 26 59035030 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0224 0095 0700\n"
    L" 27 59253260 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0227 0095 0700\n"
    L" 28 59035022 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0223 0095 0700\n"
    L" 29 58317207 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0228 0099 0710\n"
    L" 30 59570069 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0225 0095 0700\n"
    L" 31 58316233 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0228 0099 0710\n"
    L" 32 58785304 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0225 0095 0700\n"
    L" 33 53353504 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0225 0106 0710\n"
    L" 34 53348546 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0225 0106 0710\n"
    L" 35 53347910 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0225 0106 0710\n"
    L" 36 59263715 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0223 0095 0700\n"
    L" 37 58316266 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0228 0099 0710\n"
    L" 38 58785312 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0223 0095 0700\n"
    L" 39 58787425 0202 064 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0220 0095 0640\n"
    L" 40 59584649 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0223 0095 0700\n"
    L" 41 59263962 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0223 0095 0700\n"
    L" 42 59569855 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0225 0095 0700\n"
    L" 43 59251264 0202 070 12526  43103 5291 0 0 0 0 00/00 00000 000 00000  0230 0095 0700\n"
    L" 44 57340911 0222 063 09665  47114 3477 5 8 0 1 00/00 00000 000 COБCTB 0260 0086 0660\n"
    L" 45 50501865 0252 060 09665  47114 3477 5 8 0 1 00/00 00000 000 COБCTB 0269 0086 0630\n"
    L" 46 70722210 0262 062 09665  47114 3477 5 8 0 1 00/00 00000 000 COБCTB 0261 0086 0660\n"
    L" 47 50701804 0262 059 09665  47114 3477 5 8 0 2 00/00 00000 000 COБCTB 0290 0086 0645\n"
    L" 48 50702174 0262 061 09665  47114 3477 5 8 0 2 00/00 00000 000 XBHECT 0290 0086 0645\n"
    L" 49 50501774 0252 057 09665  47114 3477 5 8 0 1 00/00 00000 000 COБCTB 0271 0086 0630\n"
    L" 50 58787946 0202 069 09069  43307 7846 3 6 0 1 00/00 00000 000 273608 0231 0095 0700\n"
    L" 51 59051524 0202 062 09069  43307 7846 3 6 0 1 00/00 00000 000 273608 0220 0095 0640\n"
    L" 52 58899808 0202 069 09069  43307 7846 3 6 0 2 00/00 00000 000 273608 0224 0095 0700\n"
    L" 53 59254201 0202 069 09069  43307 7846 3 6 0 2 00/00 00000 000 273608 0223 0095 0700\n"
    L" 54 59067355 0202 069 09069  43307 7846 3 6 0 2 00/00 00000 000 273608 0224 0095 0700\n"
    L" 55 59041517 0202 069 09069  43307 7846 3 6 0 2 00/00 00000 000 273608 0224 0095 0700\n"
    L" 56 58653387 0202 069 09069  43307 7846 3 6 0 2 00/00 00000 000 273608 0225 0095 0700\n"
    L" 57 53820668 0202 069 09069  43307 7846 3 6 0 2 00/00 00000 000 273608 0226 0095 0700\n"
    L" 58 58886862 0202 069 09069  43307 7846 3 6 0 2 00/00 00000 000 273608 0224 0095 0700\n"
    L" 59 65324220 0202 043 09836  17108 6302 0 0 0 0 00/00 00000 000 19337  0232 0100 0690\n"
    L" 60 93503688 0253 000 09322  28104 3411 0 0 0 0 00/00 00000 000 000000 0182 0086 0670\n"
    L" 61 94526340 0252 006 09069  00300 0012 0 0 5 0 00/02 00000 000 07003- 0249 0141 0690\n"
    L" 62 94521408 0252 006 09069  00300 0012 0 0 5 0 00/02 00000 000 07003- 0248 0141 0690:)";

const wchar_t *c_904_2 = L"(:904 1129 2632 0900 207 1800 1 15 04 01 05 056 02592 6 0000 0 0\n"
    L" 01 59429860 0202 000 86492  42100 1574 4 0 0 0 00/00 11020 000 09069- 0235 0100 0700\n"
    L" 02 55844906 0202 000 86492  42100 1574 4 0 0 0 00/00 11020 000 09069- 0235 0100 0700\n"
    L" 03 61319307 0202 000 86492  42100 1574 4 0 0 0 00/00 11020 000 APEHДA 0240 0100 0700\n"
    L" 04 61973061 0202 000 86492  42100 1574 4 0 0 0 00/00 11020 000 APEHДA 0243 0100 0695\n"
    L" 05 52934874 0202 000 86492  42100 1574 4 0 0 0 00/00 11288 000 09069- 0234 0100 0700\n"
    L" 06 55197651 0202 000 86492  42100 1574 4 0 0 0 00/00 11020 000 09069- 0236 0100 0700\n"
    L" 07 60621927 0202 000 86492  42100 1574 4 0 0 0 00/00 11020 000 APEHДA 0240 0100 0700\n"
    L" 08 55760292 0202 000 86355  42100 3006 3 0 0 0 00/00 11020 000 09069- 0233 0100 0700\n"
    L" 09 53072344 0202 000 86355  42100 3006 3 0 0 0 00/00 11020 000 09069- 0241 0100 0695\n"
    L" 10 55685812 0202 000 86355  42100 3006 3 0 0 0 00/00 11020 000 09069- 0230 0100 0700\n"
    L" 11 61992780 0202 000 30260  42100 8383 5 0 0 0 00/00 11288 000 APEHДA 0243 0100 0695\n"
    L" 12 61993028 0202 000 30260  42100 8383 5 0 0 0 00/00 11288 000 APEHДA 0243 0100 0695\n"
    L" 13 24041931 0253 062 70010  58100 6799 0 0 0 0 00/00 11288 000 000000 0240 0106 0680\n"
    L" 14 94510088 0253 004 66785  00300 0012 0 0 5 0 00/02 11288 000 APEHДA 0220 0141 0600\n"
    L" 15 94709342 0253 004 66785  00300 0012 0 0 5 0 00/02 11288 000 APEHДA 0216 0141 0600\n"
    L" 16 54958095 0202 067 23604  00300 0012 0 0 5 0 02/00 11288 000 000000 0220 0105 0700\n"
    L" 17 94525946 0252 067 23604  00300 0012 0 0 5 0 02/00 11288 000 000000 0248 0141 0690\n"
    L" 18 94521614 0252 067 23604  00300 0012 0 0 5 0 02/00 11288 000 000000 0249 0141 0690\n"
    L" 19 94523560 0252 067 23604  00300 0012 0 0 5 0 02/00 11288 000 000000 0249 0141 0690\n"
    L" 20 94525508 0252 067 23604  00300 0012 0 0 5 0 02/00 11288 000 000000 0248 0141 0690\n"
    L" 21 94964327 0253 017 25029  00300 0012 0 8 5 0 02/00 11288 000 000000 0227 0183 0712\n"
    L" 22 60512209 0252 029 69020  00300 0012 0 0 0 0 01/00 11288 000 APEHДA 0233 0100 0700\n"
    L" 23 60511326 0252 029 69020  00300 0012 0 0 0 0 01/00 11288 000 APEHДA 0232 0100 0700\n"
    L" 24 60510500 0252 029 69020  00300 0012 0 0 0 0 01/00 11288 000 APEHДA 0232 0100 0700\n"
    L" 25 61709036 0252 029 69020  00300 0012 0 0 0 0 01/00 11288 000 APEHДA 0232 0100 0700\n"
    L" 26 60512126 0252 029 69020  00300 0012 0 0 0 0 01/00 11288 000 APEHДA 0233 0100 0700\n"
    L" 27 94524055 0252 041 24863  00300 0012 0 4 5 0 02/00 11288 000 APEHДA 0248 0141 0690\n"
    L" 28 94516317 0253 048 78032  00300 0012 0 0 5 0 02/00 11288 000 000000 0213 0141 0720\n"
    L" 29 21515887 0253 024 83140  51500 7076 0 0 0 4 00/00 11288 000 000000 0235 0106 0660\n"
    L" 30 21529375 0253 061 54688  54100 9999 0 0 0 4 00/00 11288 000 000000 0235 0106 0660\n"
    L" 31 54533310 0202 037 64822  00300 0012 0 6 0 0 02/00 11288 000 000000 0220 0105 0720\n"
    L" 32 94259132 0242 054 60143  00300 0012 0 4 0 0 02/00 11288 000 APEHДA 0200 0105 0710\n"
    L" 33 54392493 0202 054 18294  00300 0012 0 0 5 0 02/00 11288 000 000000 0250 0181 0690\n"
    L" 34 54281159 0202 017 82001  00300 0012 0 0 5 0 01/00 11288 000 000000 0236 0184 0700\n"
    L" 35 94354404 0242 008 61042  00300 0012 0 6 7 0 00/02 11288 000 APEHДA 0200 0105 0660\n"
    L" 36 94522828 0252 051 23707  00300 0012 0 6 5 0 02/00 11288 000 APEHДA 0249 0141 0690\n"
    L" 37 94546835 0253 018 93717  00300 0012 0 0 5 0 01/00 11288 000 000000 0247 0141 0690\n"
    L" 38 94546561 0253 023 93717  00300 0012 0 0 5 0 01/00 11288 000 000000 0247 0141 0690\n"
    L" 39 61193710 0202 008 70007  00300 0012 0 0 0 0 01/00 11288 000 APEHДA 0240 0100 0700\n"
    L" 40 60512506 0252 029 69020  00300 0012 0 0 0 0 01/00 11288 000 APEHДA 0233 0100 0700\n"
    L" 41 60718848 0252 029 69020  00300 0012 0 0 0 0 01/00 11288 000 APEHДA 0232 0100 0700\n"
    L" 42 61717369 0252 029 69020  00300 0012 0 0 0 0 01/00 11288 000 APEHДA 0233 0100 0700\n"
    L" 43 23566623 0253 060 73198  74200 2083 0 4 0 6 00/00 11288 000 000000 0222 0106 0640\n"
    L" 44 21519723 0253 060 73198  74200 2083 0 4 0 6 00/00 11288 000 000000 0235 0106 0660\n"
    L" 45 21513221 0253 060 73198  74200 2083 0 4 0 6 00/00 11288 000 000000 0235 0106 0660\n"
    L" 46 23602527 0253 060 73198  74200 2083 0 4 0 6 00/00 11288 000 000000 0235 0106 0660\n"
    L" 47 21515440 0253 060 73198  74200 2083 0 4 0 6 00/00 11288 000 000000 0235 0106 0660\n"
    L" 48 24107930 0253 068 54701  57300 8466 0 0 0 6 00/00 11288 000 000000 0240 0106 0680:)";

const wchar_t *c_1042 = L"(:1042 909/000+11310 2419 1131 039 0950 03 11420 27 01 08 30 00/00 0 0/00 00 0\n"
    L"Ю2 0 00 00 00 00 0000 0 0 042 02429 01647 136 56069321 52600855 000 000 00 028 006.20 001 000 000 60 004 000 000 90 023 006 000 95 012 000 000 96 011 006 000\n"
    L"Ю3 539 00012331 1 06 45 0000 00000 VOLKOVS     \n"
    L"Ю3 539 00012332 9:)";

const wchar_t *c_1042_2 = L"(:1042 909/000+09000 9999 0900 238 1800 07 11760 27 01 10 35 00/00 0 0/00 00 0\n"
    L"Ю2 0 00 00 00 00 0000 0 0 057 02450 01406 176 57797482 94817681 000 000 00 044 000.20 011 000 000 60 007 000 000 90 026 000 000 96 026 000 000\n"
    L"Ю4 11290 2 63 008 000.20 008 000 \n"
    L"Ю4 11290 2 68 032 000.20 002 000 60 007 000 90 023 000 96 023 000 \n"
    L"Ю4 11290 2 73 001 000.20 001 000 \n"
    L"Ю4 11290 2 17 002 000.90 002 000 96 002 000 \n"
    L"Ю4 11290 2 24 001 000.90 001 000 96 001 000 \n"
    L"Ю12 00 57797482 1 028 70580 00300 0012 000 00 00 00 01 00 0240 20 60 5600 04 100 18000 00000 11290 68 09000 42 00000 0000 020 1 1280 0      132 000 00000000\n"
    L"Ю12 00 52718194 191239255255 0200242\n"
    L"Ю12 00 55566657 191239255255 0280240\n"
    L"Ю12 00 58379694 255255255255 \n"
    L"Ю12 00 61617759 255237255247 02350600APEHДA\n"
    L"Ю12 00 52716982 191237255247 010024256000     \n"
    L"Ю12 00 54907589 191239255255 0270243\n"
    L"Ю12 00 24383432 133169125191 049700004170099990600023820020610616025\n"
    L"Ю12 00 29336997 139237125183 04770020474000050271021113543024APEHДA\n"
    L"Ю12 00 94359353 129169117191 06226260003000012040000201909609421052442025\n"
    L"Ю12 00 94521564 155237119255 049236000050249094614117\n"
    L"Ю12 00 52673787 129169117183 061648204510099990000600025820520612263180200     \n"
    L"Ю12 00 52387438 255239255255 0257\n"
    L"Ю12 00 52113354 255255255255 \n"
    L"Ю12 00 52671534 255255255255 \n"
    L"Ю12 00 28080430 255237255247 02590208APEHДA\n"
    L"Ю12 00 28082337 255255255255 \n"
    L"Ю12 00 52564101 255237255247 025852060     \n"
    L"Ю12 00 52394400 255239255255 0267\n"
    L"Ю12 00 24094468 143237119191 04272350461000240020610673025\n"
    L"Ю12 00 94796521 129217117247 0046957000300001200500029609461416842APEHДA\n"
    L"Ю12 00 94772407 191255255255 005\n"
    L"Ю12 00 94796554 255255255255 \n"
    L"Ю12 00 94772621 191255255255 004\n"
    L"Ю12 00 94729019 191239255255 0050245\n"
    L"Ю12 00 94772514 255239255255 0240\n"
    L"Ю12 00 94644655 255239255255 0216\n"
    L"Ю12 00 94545357 191239255255 0040247\n"
    L"Ю12 00 94516507 255239255255 0220\n"
    L"Ю12 00 94511102 255255255255 \n"
    L"Ю12 00 94516549 255255255255 \n"
    L"Ю12 00 94505351 255239255255 0230\n"
    L"Ю12 00 94512613 255239255255 0220\n"
    L"Ю12 00 94753175 255239255255 0240\n"
    L"Ю12 00 94511201 155143255191 0486617006502000220027\n"
    L"Ю12 00 94848934 255239255255 0230\n"
    L"Ю12 00 94509007 191239255255 0470220\n"
    L"Ю12 00 94752680 255239255255 0230\n"
    L"Ю12 00 94510765 191239255255 0480220\n"
    L"Ю12 00 94817210 255239255255 0240\n"
    L"Ю12 00 94817376 255255255255 \n"
    L"Ю12 00 94866365 255239255255 0200\n"
    L"Ю12 00 94867033 255255255255 \n"
    L"Ю12 00 94817681 155175247183 00919720005010240170250     :)";

const wchar_t *c_1042_3 = L"(:1042 909/000+09000 2413 1131 038 0862 10 11760 27 01 12 00 00/00 0 0/00 00 0\n"
    L"Ю1 2413 1131 038 0900 01\n"
    L"Ю2 0  T 00 00 00 0000 0 0 053 04895 03344 232 51425726 95699344 000 000 00 058 000.40 004 000 000 70 045 000 000 73 003 000 000 90 009 000 000 95 006 000 000 96 003 000 000:)";

const wchar_t *c_333 = L"(:333 1042 909/000+11290 2609 1800 541 1131 04 11310 27 01 12 45 00/00 0 0/00 00 0\n"
    L"Ю2 0 00 00 00 00 0000 0 0 057 04713 03417 220 75085555 95316873 000 000 00 055 000.60 001 000 000 70 009 000 000 73 002 000 000 90 045 000 000 95 045 000 000:)";

const wchar_t *c_wrongCode = L"(:1333 1042 909/000+11290 2609 1800 541 1131 04 11310 27 01 12 45 00/00 0 0/00 00 0\n"
    L"Ю2 0 00 00 00 00 0000 0 0 057 04713 03417 220 75085555 95316873 000 000 00 055 000.60 001 000 000 70 009 000 000 73 002 000 000 90 045 000 000 95 045 000 000:)";

const wchar_t *c_wrongInformationOriginCodeTooLong = L"(:1042 909/0220+11310 2419 1131 039 0950 03 11420 27 01 08 30 00/00 0 0/00 00 0\n"
    L"Ю2 0 00 00 00 00 0000 0 0 042 02429 01647 136 56069321 52600855 000 000 00 028 006.20 001 000 000 60 004 000 000 90 023 006 000 95 012 000 000 96 011 006 000\n"
    L"Ю3 539 00012331 1 06 45 0000 00000 VOLKOVS     \n"
    L"Ю3 539 00012332 9:)";

const wchar_t *c_wrongInformationOriginCodeTooShort = L"(:1042 909/00+11310 2419 1131 039 0950 03 11420 27 01 08 30 00/00 0 0/00 00 0\n"
    L"Ю2 0 00 00 00 00 0000 0 0 042 02429 01647 136 56069321 52600855 000 000 00 028 006.20 001 000 000 60 004 000 000 90 023 006 000 95 012 000 000 96 011 006 000\n"
    L"Ю3 539 00012331 1 06 45 0000 00000 VOLKOVS     \n"
    L"Ю3 539 00012332 9:)";

const wchar_t *c_1042_withoutMachinist = L"(:1042 909/000+11310 2419 1131 039 0950 03 11420 27 01 08 30 00/00 0 0/00 00 0\n"
    L"Ю2 0 00 00 00 00 0000 0 0 042 02429 01647 136 56069321 52600855 000 000 00 028 006.20 001 000 000 60 004 000 000 90 023 006 000 95 012 000 000 96 011 006 000\n"
    L"Ю3 539 00012331 1 00 00 0000 00000 0           \n"
    L"Ю3 539 00012332 9:)";


const wchar_t *c_PassagirMsgInfo = L"<TrainAssignment operation='1' date='12-02-2019 15:26' esr='12345' location='0,0' number='1235'>"
    L"<Unit number='123432' driver='Иванов' driverTime='12-02-2019 00:00' weight='234' length='24' specialConditions='M' carrier='22'>"
    L"<Wagon number='1231' axisCount='4' weight='12' length='1' type='1' owner='22'/>"
    L"<Wagon number='1234' axisCount='4' weight='12' length='1' type='2' owner='22'/>"
    L"<Wagon number='213143' axisCount='4' weight='23' length='1' type='3' owner='23'/>"
    L"<Wagon number='21345' axisCount='4' weight='23' length='1' type='3' owner='23'/>"
    L"</Unit>"
    L"<Unit number='312354м' driver='Петров' driverTime='12-02-2019 00:00' weight='235' length='34' specialConditions='' carrier='34234'>"
    L"<Wagon number='12387' axisCount='4' weight='23' length='1' type='1' owner='6'/>"
    L"<Wagon number='23454' axisCount='4' weight='24' length='2' type='2' owner='6'/>"
    L"<Wagon number='2342125' axisCount='4' weight='30' length='2' type='3' owner='34'/>"
    L"</Unit>"
    L"</TrainAssignment>";

void TC_Asoup::setUp()
{
    m_receivedDate = boost::gregorian::date(2015, 6, 6);
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
}
void TC_Asoup::tearDown()
{
    set_TimeZone("");
}

template< typename messageType, typename enumType >
void TC_Asoup::verify(const messageType& message, enumType attribute, const wchar_t *value) const
{
    {
        std::stringstream messageStream;
        messageStream << "Отсутствует аттрибут " << std::to_string((int)attribute) << ", ожидалось значение \"" << ToUtf8(value) << "\"";
        CPPUNIT_ASSERT_MESSAGE(messageStream.str(), message.containsAttribute(attribute));
    }
    {
        std::stringstream messageStream;
        messageStream << "Значение аттрибута " << std::to_string((int)attribute) << " \"" 
            << ToUtf8(message.getAttribute(attribute)) << "\" не соответствует \"" << ToUtf8(value) << "\"";
        CPPUNIT_ASSERT_EQUAL_MESSAGE(messageStream.str(), 0, message.getAttribute(attribute).compare(value));
    }
}

void TC_Asoup::verify(const Asoup::Message& message, Asoup::AsoupMsgAttribute attribute, unsigned long value) const
{
    CPPUNIT_ASSERT(message.containsAttribute(attribute));
    CPPUNIT_ASSERT_EQUAL(value, message.getAttributeUlong(attribute));
}

void TC_Asoup::verify(const Asoup::Message& message, Asoup::AsoupMsgAttribute attribute, Asoup::OperationCodeEnum value) const
{
    CPPUNIT_ASSERT(message.containsAttribute(attribute));
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned>(value), static_cast<unsigned>(message.getOperationCode(Asoup::AsoupOperationMode::LDZ, attribute)));
}

void TC_Asoup::verifyDateTime(const Asoup::Message& message, Asoup::AsoupMsgAttribute attribute, time_t value) const
{
    CPPUNIT_ASSERT(message.containsAttribute(attribute));
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned>(value), static_cast<unsigned>(message.getAttributeDateTime(attribute)));
}

namespace std
{
    ostream& operator <<(ostream& stream, const Trindex& trainIndex)
    {
        stream << ToUtf8(trainIndex.str());
        return stream;
    }
}

void TC_Asoup::verify(const Asoup::Message& message, const Trindex& value) const
{
    CPPUNIT_ASSERT(message.containsAttribute(Asoup::AsoupMsgAttribute::TrainIndex));
    Trindex messageValue = message.getTrainIndex();
    CPPUNIT_ASSERT_EQUAL(value, messageValue);
}

void TC_Asoup::verifyNoAttr(const Asoup::Message& message, Asoup::AsoupMsgAttribute attribute) const
{
    CPPUNIT_ASSERT(!message.containsAttribute(attribute));
}

void TC_Asoup::tc_Creation()
{
    CPPUNIT_ASSERT(!Asoup::Message::parse(L"(:", m_receivedDate, Asoup::AsoupOperationMode::LDZ, false));

    CPPUNIT_ASSERT(!Asoup::Message::parse(L"(::]", m_receivedDate, Asoup::AsoupOperationMode::LDZ, false));
    CPPUNIT_ASSERT(!Asoup::Message::parse(L"(:)", m_receivedDate, Asoup::AsoupOperationMode::LDZ, false));
    CPPUNIT_ASSERT(!Asoup::Message::parse(L":::)", m_receivedDate, Asoup::AsoupOperationMode::LDZ, false));

    CPPUNIT_ASSERT(!Asoup::Message::parse(c_wrongCode, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false));
    CPPUNIT_ASSERT(!Asoup::Message::parse(c_wrongInformationOriginCodeTooLong, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false));
    CPPUNIT_ASSERT(!Asoup::Message::parse(c_wrongInformationOriginCodeTooShort, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false));
}

void TC_Asoup::tc_Readers()
{
    std::wstring values = L"12325АРЕНДА";
    std::wistringstream stream(values);
    unsigned test1, test2;
    std::wstring note;
    Asoup::Impl::Reader::readpart<3>(test1, stream);
    CPPUNIT_ASSERT(123u == test1);
    Asoup::Impl::Reader::readpart<2>(test2, stream);
    CPPUNIT_ASSERT(25u == test2);
    Asoup::Impl::Reader::readpart<6>(note, stream);
    CPPUNIT_ASSERT(note == std::wstring(L"АРЕНДА"));

    values = L"123 2345.02 222";
    std::wistringstream stream2(values);
    Asoup::Impl::Reader::read<2, 4>(test1, stream2);
    CPPUNIT_ASSERT(123 == test1);

    unsigned ignorethat = 0u;
    CPPUNIT_ASSERT(!(Asoup::Impl::Reader::tryRead<4>(ignorethat, stream2, 13u)));
    CPPUNIT_ASSERT(13 == ignorethat);

    Asoup::Impl::Reader::read<1, 6>(test2, stream2, '.');
    CPPUNIT_ASSERT(2345 == test2);

    CPPUNIT_ASSERT('.' == stream2.peek());
    stream2.get();

    unsigned test3, test4;
    Asoup::Impl::Reader::read<2>(test3, stream2);
    CPPUNIT_ASSERT(02 == test3);
    Asoup::Impl::Reader::read<3>(test4, stream2);
    CPPUNIT_ASSERT(222 == test4);
}

void TC_Asoup::tc_Parse904()
{
    auto ptr = Asoup::Message::parse(c_904, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false);
    CPPUNIT_ASSERT(ptr);
    const Asoup::Message& message = *ptr;
    // 0705 3811 0700 51 0779 1 27 01 08 35 062 05442 6 0000 0 0
    verify(message, Asoup::AsoupMsgAttribute::MessageCode, 904u);
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::EsrOfSenderStation, L"07050");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::TrainNumber, L"3811");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::TrainIndex, L"0700-051-0779");
    verify(message, Trindex(700, 51, 779));
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::ResolvingTrainSign, L"1");
    {
        verify(message, Asoup::AsoupMsgAttribute::FormationDateTime, L"20150127T083500");

        verifyDateTime(message, Asoup::AsoupMsgAttribute::FormationDateTime, 
            timeForLocalDateTime(2015u, 1u, 27u, 8u, 35u));
    }
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::UnitLength, L"062");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::BruttoWeight, L"05442");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::SpecialClosingCode, L"6");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::SpecialOversizeIndex, L"0000");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::SpecialPoultry, L"0");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::SpecialRoute, L"0");

    const Asoup::M904& m904 = *message.get904Info();
    CPPUNIT_ASSERT_MESSAGE("Число информационных блоков должно быть 62", 62u == m904.blocks.size());
    {
        const Asoup::M904::CarInformationBlock& block = m904.blocks[0];
        // L" 01 58315946 0202 071 12526  43103 5291 0 0 0 0 00/00 00000 000 073476 0228 0099 0710"

        CPPUNIT_ASSERT_EQUAL(1u, block.consecutiveCarNumber);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"58315946"), block.machineNumber);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"0202"), block.ownerCodeAndRollerBearingSign);
        CPPUNIT_ASSERT_EQUAL(71u, block.cargoWeight);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"12526"), block.destinationStationCode);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"43103"), block.cargoCode);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"5291"), block.cargoReceiverCode);
        CPPUNIT_ASSERT_EQUAL(L'0', block.specialNodes.routeParkGroupCoupling);
        CPPUNIT_ASSERT_EQUAL(L'0', block.specialNodes.coveringCode);
        CPPUNIT_ASSERT_EQUAL(L'0', block.specialNodes.specialSign);
        CPPUNIT_ASSERT_EQUAL(0u, block.sealsCount);
        CPPUNIT_ASSERT_EQUAL(0u, block.containerNumber.first);
        CPPUNIT_ASSERT_EQUAL(0u, block.containerNumber.second);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"00000"), block.outputBorderStationEsrCode);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"000"), block.someField4);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"073476"), block.note);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(22.8f, block.tareWeight, c_floatDelta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.99f, block.conventionalLength, c_floatDelta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(71.f, block.capacity, c_floatDelta);
    }

    {
        const Asoup::M904::CarInformationBlock& block = m904.blocks[48];
        // L" 49 50501774 0252 057 09665  47114 3477 5 8 0 1 00/00 00000 000 COБCTB 0271 0086 0630\n"

        CPPUNIT_ASSERT_EQUAL(49u, block.consecutiveCarNumber);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"50501774"), block.machineNumber);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"0252"), block.ownerCodeAndRollerBearingSign);
        CPPUNIT_ASSERT_EQUAL(57u, block.cargoWeight);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"09665"), block.destinationStationCode);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"47114"), block.cargoCode);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"3477"), block.cargoReceiverCode);
        CPPUNIT_ASSERT_EQUAL(L'5', block.specialNodes.routeParkGroupCoupling);
        CPPUNIT_ASSERT_EQUAL(L'8', block.specialNodes.coveringCode);
        CPPUNIT_ASSERT_EQUAL(L'0', block.specialNodes.specialSign);
        CPPUNIT_ASSERT_EQUAL(1u, block.sealsCount);
        CPPUNIT_ASSERT_EQUAL(0u, block.containerNumber.first);
        CPPUNIT_ASSERT_EQUAL(0u, block.containerNumber.second);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"00000"), block.outputBorderStationEsrCode);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"000"), block.someField4);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"COБCTB"), block.note);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(27.1f, block.tareWeight, c_floatDelta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.86f, block.conventionalLength, c_floatDelta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(63.f, block.capacity, c_floatDelta);
    }
}

void TC_Asoup::tc_Parse904_2()
{
    auto ptr = Asoup::Message::parse(c_904_2, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false);
    CPPUNIT_ASSERT(ptr);
    const Asoup::Message& message = *ptr;
    // 1129 2632 0900 207 1800 1 15 04 01 05 056 02592 6 0000 0 0
    verify(message, Asoup::AsoupMsgAttribute::MessageCode, 904u);
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::EsrOfSenderStation, L"11290");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::TrainNumber, L"2632");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::TrainIndex, L"0900-207-1800");
    verify(message, Trindex(900, 207, 1800));
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::ResolvingTrainSign, L"1");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::FormationDateTime, L"20150415T010500");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::UnitLength, L"056");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::BruttoWeight, L"02592");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::SpecialClosingCode, L"6");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::SpecialOversizeIndex, L"0000");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::SpecialPoultry, L"0");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::SpecialRoute, L"0");
}

void TC_Asoup::tc_Parse1042()
{
    auto ptr = Asoup::Message::parse(c_1042, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false);
    CPPUNIT_ASSERT(ptr);
    const Asoup::Message& message = *ptr;
    // 909/000+11310 2419 1131 039 0950 03 11420 27 01 08 30 00/00 0 0/00 00 0
    verify(message, Asoup::AsoupMsgAttribute::MessageCode, 1042u);
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::EsrOfSenderStation, L"11310");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::InformationOriginPointCode, L"909/000");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::TrainNumber, L"2419");
    verify(message, Asoup::AsoupMsgAttribute::TrainNumber, 2419u);
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::TrainIndex, L"1131-039-0950");
    verify(message, Trindex(1131, 39, 950));
    verify(message, Asoup::AsoupMsgAttribute::OperationCode, Asoup::OperationCodeEnum::DepartureOfTheTrainFromTheStation);
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::DirectionTo, L"11420");
    CPPUNIT_ASSERT(!message.containsAttribute(Asoup::AsoupMsgAttribute::DirectionFrom));
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::OperationDateTime, L"20150127T083000");
    verifyDateTime(message, Asoup::AsoupMsgAttribute::OperationDateTime, 
        timeForLocalDateTime(2015u, 1u, 27u, 8u, 30u));
    CPPUNIT_ASSERT_EQUAL(false, message.containsAttribute(Asoup::AsoupMsgAttribute::ParkAndWay));
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::WayOfOvertake, L"0");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::FullDaysOfDelayAndReason, L"0/00");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::ConnectionCodeOrTrowCode, L"00");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::TrainCategory, L"0");

    const Asoup::M1042& m1042 = *message.get1042Info();

    CPPUNIT_ASSERT_MESSAGE("Число блоков Ю1 должно быть равно 0", 0u == m1042.u1.size());

    CPPUNIT_ASSERT_MESSAGE("Блок Ю2 должен сущестовать", nullptr != m1042.u2.get());
    Asoup::M1042::Yu2& u2 = *m1042.u2;
    // L"Ю2 0 00 00 00 00 0000 0 0 042 02429 01647 136 56069321 52600855 000 000 00 028 006.20 001 000 000 60 004 000 000 90 023 006 000 95 012 000 000 96 011 006 000\n"
    CPPUNIT_ASSERT_EQUAL(L'0', u2.specialCharacteristics.category);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"00"), u2.specialCharacteristics.highWeight);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"00"), u2.specialCharacteristics.longTrain);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"00"), u2.specialCharacteristics.highLoad);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"00"), u2.specialCharacteristics.connectedTrains);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"0000"), u2.specialCharacteristics.oversizeIndex);
    CPPUNIT_ASSERT_EQUAL(L'0', u2.specialCharacteristics.poultry);
    CPPUNIT_ASSERT_EQUAL(L'0', u2.specialCharacteristics.route);
    CPPUNIT_ASSERT_EQUAL(42u, u2.conventionalLength);
    CPPUNIT_ASSERT_EQUAL(2429u, u2.bruttoWeight);
    CPPUNIT_ASSERT_EQUAL(1647u, u2.nettoWeight);
    CPPUNIT_ASSERT_EQUAL(136u, u2.axisCount);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"56069321"), u2.headCarNumber);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"52600855"), u2.tailCarNumber);
    CPPUNIT_ASSERT_EQUAL(0u, u2.NDLMCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.NRPCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.passengerCarsCount);
    CPPUNIT_ASSERT_EQUAL(28u, u2.fullActiveParkCarsCount);
    CPPUNIT_ASSERT_EQUAL(6u, u2.emptyActiveParkCarsCount);

    CPPUNIT_ASSERT_MESSAGE("Число блоков разложения по НРП должно быть равно 5", 5u == u2.decomposition.size());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"20"), u2.decomposition[0].RPSCode);
    CPPUNIT_ASSERT_EQUAL(1u, u2.decomposition[0].fullCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.decomposition[0].emptyCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.decomposition[0].inactiveParkCarsCount);

    CPPUNIT_ASSERT_EQUAL(std::wstring(L"96"), u2.decomposition[4].RPSCode);
    CPPUNIT_ASSERT_EQUAL(11u, u2.decomposition[4].fullCarsCount);
    CPPUNIT_ASSERT_EQUAL(6u, u2.decomposition[4].emptyCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.decomposition[4].inactiveParkCarsCount);

    CPPUNIT_ASSERT_MESSAGE("Число блоков Ю3 должно быть равно 1", 1u == m1042.u3.size());
    // L"Ю3 539 00012331 1 06 45 0000 00000 VOLKOVS     \n"
    // L"Ю3 539 00012332 9:)";
    CPPUNIT_ASSERT_EQUAL(539u, m1042.u3[0].seriesCode);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Секций в блоке должно быть две", 2u, m1042.u3[0].sectionsCount);
    CPPUNIT_ASSERT_EQUAL(0, m1042.u3[0].locomotiveNumber.compare(L"1233"));
    CPPUNIT_ASSERT(Asoup::M1042::Yu3::ConsecutionType::TrainHead == m1042.u3[0].consecutionTypeCode);
    CPPUNIT_ASSERT_EQUAL((unsigned short)6u, m1042.u3[0].appearanceTime.first);
    CPPUNIT_ASSERT_EQUAL((unsigned short)45u, m1042.u3[0].appearanceTime.second);
    CPPUNIT_ASSERT_EQUAL(0, m1042.u3[0].registryDepot.compare(L"0000"));
    CPPUNIT_ASSERT_EQUAL(0, m1042.u3[0].machinistPersonnelNumber.compare(L"00000"));
    CPPUNIT_ASSERT(m1042.u3[0].machinistSurname == L"VOLKOVS");
}

void TC_Asoup::tc_Parse1042_2()
{
    auto ptr = Asoup::Message::parse(c_1042_2, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false);
    CPPUNIT_ASSERT(ptr);
    const Asoup::Message& message = *ptr;
    // L"909/000+09000 9999 0900 238 1800 07 11760 27 01 10 35 00/00 0 0/00 00 0"
    verify(message, Asoup::AsoupMsgAttribute::MessageCode, 1042u);
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::EsrOfSenderStation, L"09000");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::InformationOriginPointCode, L"909/000");
    verifyNoAttr(message, Asoup::AsoupMsgAttribute::TrainNumber);
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::TrainIndex, L"0900-238-1800");
    verify(message, Trindex(900, 238, 1800));
    verify(message, Asoup::AsoupMsgAttribute::OperationCode, Asoup::OperationCodeEnum::FormationOfTrain);
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::DirectionTo, L"11760");
    CPPUNIT_ASSERT(!message.containsAttribute(Asoup::AsoupMsgAttribute::DirectionFrom));
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::OperationDateTime, L"20150127T103500");
    CPPUNIT_ASSERT_EQUAL(false, message.containsAttribute(Asoup::AsoupMsgAttribute::ParkAndWay));
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::WayOfOvertake, L"0");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::FullDaysOfDelayAndReason, L"0/00");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::ConnectionCodeOrTrowCode, L"00");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::TrainCategory, L"0");

    const Asoup::M1042& m1042 = *message.get1042Info();
    CPPUNIT_ASSERT_MESSAGE("Число блоков Ю1 должно быть равно 0", 0u == m1042.u1.size());

    CPPUNIT_ASSERT_MESSAGE("Блок Ю2 должен сущестовать", nullptr != m1042.u2.get());
    Asoup::M1042::Yu2& u2 = *m1042.u2;
    // L"Ю2 0 00 00 00 00 0000 0 0 057 02450 01406 176 57797482 94817681 000 000 00 044 000.20 011 000 000 60 007 000 000 90 026 000 000 96 026 000 000\n"
    CPPUNIT_ASSERT_EQUAL(L'0', u2.specialCharacteristics.category);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"00"), u2.specialCharacteristics.highWeight);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"00"), u2.specialCharacteristics.longTrain);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"00"), u2.specialCharacteristics.highLoad);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"00"), u2.specialCharacteristics.connectedTrains);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"0000"), u2.specialCharacteristics.oversizeIndex);
    CPPUNIT_ASSERT_EQUAL(L'0', u2.specialCharacteristics.poultry);
    CPPUNIT_ASSERT_EQUAL(L'0', u2.specialCharacteristics.route);
    CPPUNIT_ASSERT_EQUAL(57u, u2.conventionalLength);
    CPPUNIT_ASSERT_EQUAL(2450u, u2.bruttoWeight);
    CPPUNIT_ASSERT_EQUAL(1406u, u2.nettoWeight);
    CPPUNIT_ASSERT_EQUAL(176u, u2.axisCount);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"57797482"), u2.headCarNumber);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"94817681"), u2.tailCarNumber);
    CPPUNIT_ASSERT_EQUAL(0u, u2.NDLMCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.NRPCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.passengerCarsCount);
    CPPUNIT_ASSERT_EQUAL(44u, u2.fullActiveParkCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.emptyActiveParkCarsCount);

    CPPUNIT_ASSERT_MESSAGE("Число блоков разложения по НРП должно быть равно 4", 4u == u2.decomposition.size());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"20"), u2.decomposition[0].RPSCode);
    CPPUNIT_ASSERT_EQUAL(11u, u2.decomposition[0].fullCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.decomposition[0].emptyCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.decomposition[0].inactiveParkCarsCount);

    CPPUNIT_ASSERT_EQUAL(std::wstring(L"96"), u2.decomposition[3].RPSCode);
    CPPUNIT_ASSERT_EQUAL(26u, u2.decomposition[3].fullCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.decomposition[3].emptyCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.decomposition[3].inactiveParkCarsCount);

    CPPUNIT_ASSERT_MESSAGE("Блоков Ю3 не должно быть", 0u == m1042.u3.size());

    /*"Ю4 11290 2 63 008 000.20 008 000 \n"
    L"Ю4 11290 2 68 032 000.20 002 000 60 007 000 90 023 000 96 023 000 \n"
    L"Ю4 11290 2 73 001 000.20 001 000 \n"
    L"Ю4 11290 2 17 002 000.90 002 000 96 002 000 \n"
    L"Ю4 11290 2 24 001 000.90 001 000 96 001 000 \n"*/
    CPPUNIT_ASSERT_MESSAGE("Число блоков Ю4 должно быть равно 5", 5u == m1042.u4.size());
    const Asoup::M1042::Yu4& u4 = m1042.u4[1];

    CPPUNIT_ASSERT_EQUAL(0, u4.stationCode.compare(L"11290"));
    CPPUNIT_ASSERT(Asoup::M1042::Yu4::StationSign::Joint == u4.stationSign);
    CPPUNIT_ASSERT_EQUAL(0, u4.NODCode.compare(L"68"));
    CPPUNIT_ASSERT_EQUAL(32u, u4.fullCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u4.emptyCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u4.inactiveParkCarsCount);
    CPPUNIT_ASSERT_MESSAGE("Разложение по РПС должно включать в себя 4 единицы", 4u == u4.rollingStockLayout.size());

    CPPUNIT_ASSERT_EQUAL(0, u4.rollingStockLayout[3].RPSCode.compare(L"96"));
    CPPUNIT_ASSERT_EQUAL(23u, u4.rollingStockLayout[3].fullCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u4.rollingStockLayout[3].emptyCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u4.rollingStockLayout[3].inactiveParkCarsCount);

    CPPUNIT_ASSERT_MESSAGE("Блоков Ю12 должно быть 44", 44u == m1042.u12.size());

    {
        const Asoup::M1042::Yu12& u12 = m1042.u12[0];
        // L"Ю12 00 57797482 1 028 70580 00300 0012 000 00 00 00 01 00 0240 20 60 5600 04 100 18000 00000 11290 68 09000 42 00000 0000 020 1 1280 0      132 000 00000000\n"
        CPPUNIT_ASSERT(u12.adjustmentSign == std::wstring(L"00"));
        CPPUNIT_ASSERT(u12.carInventoryNumber == std::wstring(L"57797482"));
        CPPUNIT_ASSERT(u12.castorsFlags == '1');
        CPPUNIT_ASSERT(u12.cargoWeight == 28u);
        CPPUNIT_ASSERT(u12.carDestinationStation == std::wstring(L"70580"));
        CPPUNIT_ASSERT(u12.cargoCode == std::wstring(L"00300"));
        CPPUNIT_ASSERT(u12.cargoReceiverCode == std::wstring(L"0012"));
        CPPUNIT_ASSERT(u12.specialSigns == std::wstring(L"000"));
        CPPUNIT_ASSERT(u12.sealsCount == 0u);
        CPPUNIT_ASSERT(u12.midWeightFullContainersCount == 0u);
        CPPUNIT_ASSERT(u12.midWeightEmptyContainersCount == 0u);
        CPPUNIT_ASSERT(u12.bigWeightFullContainersCount == 1u);
        CPPUNIT_ASSERT(u12.bigWeightEmptyContainersCount == 0u);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(24.f, u12.tare, c_floatDelta);
        CPPUNIT_ASSERT(u12.parkType == std::wstring(L"20"));
        CPPUNIT_ASSERT(u12.accountingCarKind == std::wstring(L"60"));
        CPPUNIT_ASSERT(u12.conventionalCarType == std::wstring(L"5600"));
        CPPUNIT_ASSERT(u12.axisCount == 4u);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.f, u12.conventionalCarLength, c_floatDelta);
        CPPUNIT_ASSERT(u12.trainTerminus == std::wstring(L"18000"));
        CPPUNIT_ASSERT(u12.destinationByPF == std::wstring(L"00000"));
        CPPUNIT_ASSERT(u12.carDeliveryJoint == std::wstring(L"11290"));
        CPPUNIT_ASSERT(u12.destinationNOD == std::wstring(L"68"));
        CPPUNIT_ASSERT(u12.lastTechnicalProcessingStation == std::wstring(L"09000"));
        CPPUNIT_ASSERT(u12.cargoKindForOperationalNomenclature == std::wstring(L"42"));
        CPPUNIT_ASSERT(u12.loadAndUnloadStation == std::wstring(L"00000"));
        CPPUNIT_ASSERT(u12.cargoSenderCode == std::wstring(L"0000"));
        CPPUNIT_ASSERT(u12.carQualityFlag == Asoup::M1042::Yu12::QualitySign::NothingWrong);
        CPPUNIT_ASSERT(u12.carOwnerCode == std::wstring(L"20"));
        CPPUNIT_ASSERT(u12.outputBorderStation == std::wstring(L"11280"));
        CPPUNIT_ASSERT(u12.note == std::wstring(L"0"));
        CPPUNIT_ASSERT(u12.carSignsBytes == std::wstring(L"132"));
        CPPUNIT_ASSERT(u12.trackingSignsBytes == std::wstring(L"000"));
        CPPUNIT_ASSERT(u12.specialCarFeatures == std::wstring(L"00000000"));
    }


    {
        const Asoup::M1042::Yu12& u12 = m1042.u12[1];
        // L"Ю12 00 57797482 1 028 70580 00300 0012 000 00 00 00 01 00 0240 20 60 5600 04 100 18000 00000 11290 68 09000 42 00000 0000 020 1 1280 0      132 000 00000000\n"
        // L"Ю12 00 52718194 191239255255 0200242\n"

        // Совпадение с предыдущей записью 10111111 11101111 11111111 11111111
        CPPUNIT_ASSERT(u12.adjustmentSign == std::wstring(L"00"));
        CPPUNIT_ASSERT(u12.carInventoryNumber == std::wstring(L"52718194"));
        CPPUNIT_ASSERT(u12.castorsFlags == '1');

        // Было 028
        CPPUNIT_ASSERT(u12.cargoWeight == 20u);
        CPPUNIT_ASSERT(u12.carDestinationStation == std::wstring(L"70580"));
        CPPUNIT_ASSERT(u12.cargoCode == std::wstring(L"00300"));
        CPPUNIT_ASSERT(u12.cargoReceiverCode == std::wstring(L"0012"));
        CPPUNIT_ASSERT(u12.specialSigns == std::wstring(L"000"));
        CPPUNIT_ASSERT(u12.sealsCount == 0u);
        CPPUNIT_ASSERT(u12.midWeightFullContainersCount == 0u);
        CPPUNIT_ASSERT(u12.midWeightEmptyContainersCount == 0u);
        CPPUNIT_ASSERT(u12.bigWeightFullContainersCount == 1u);
        CPPUNIT_ASSERT(u12.bigWeightEmptyContainersCount == 0u);

        // Было 0240
        CPPUNIT_ASSERT_DOUBLES_EQUAL(24.2f, u12.tare, c_floatDelta);
        CPPUNIT_ASSERT(u12.parkType == std::wstring(L"20"));
        CPPUNIT_ASSERT(u12.accountingCarKind == std::wstring(L"60"));
        CPPUNIT_ASSERT(u12.conventionalCarType == std::wstring(L"5600"));
        CPPUNIT_ASSERT(u12.axisCount == 4u);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.f, u12.conventionalCarLength, c_floatDelta);
        CPPUNIT_ASSERT(u12.trainTerminus == std::wstring(L"18000"));
        CPPUNIT_ASSERT(u12.destinationByPF == std::wstring(L"00000"));
        CPPUNIT_ASSERT(u12.carDeliveryJoint == std::wstring(L"11290"));
        CPPUNIT_ASSERT(u12.destinationNOD == std::wstring(L"68"));
        CPPUNIT_ASSERT(u12.lastTechnicalProcessingStation == std::wstring(L"09000"));
        CPPUNIT_ASSERT(u12.cargoKindForOperationalNomenclature == std::wstring(L"42"));
        CPPUNIT_ASSERT(u12.loadAndUnloadStation == std::wstring(L"00000"));
        CPPUNIT_ASSERT(u12.cargoSenderCode == std::wstring(L"0000"));
        CPPUNIT_ASSERT(u12.carQualityFlag == Asoup::M1042::Yu12::QualitySign::NothingWrong);
        CPPUNIT_ASSERT(u12.carOwnerCode == std::wstring(L"20"));
        CPPUNIT_ASSERT(u12.outputBorderStation == std::wstring(L"11280"));
        CPPUNIT_ASSERT(u12.note == std::wstring(L"0"));
        CPPUNIT_ASSERT(u12.carSignsBytes == std::wstring(L"132"));
        CPPUNIT_ASSERT(u12.trackingSignsBytes == std::wstring(L"000"));
        CPPUNIT_ASSERT(u12.specialCarFeatures == std::wstring(L"00000000"));
    }

    // 5й блок Ю12: L"Ю12 00 61617759 255237255247 02350600APEHДA\n"
    {
        const Asoup::M1042::Yu12& u12 = m1042.u12[4];
        CPPUNIT_ASSERT(u12.adjustmentSign == std::wstring(L"00"));
        CPPUNIT_ASSERT(u12.carInventoryNumber == std::wstring(L"61617759"));
        CPPUNIT_ASSERT(u12.castorsFlags == '1');
        CPPUNIT_ASSERT(u12.cargoWeight == 28u);
        CPPUNIT_ASSERT(u12.carDestinationStation == std::wstring(L"70580"));
        CPPUNIT_ASSERT(u12.cargoCode == std::wstring(L"00300"));
        CPPUNIT_ASSERT(u12.cargoReceiverCode == std::wstring(L"0012"));
        CPPUNIT_ASSERT(u12.specialSigns == std::wstring(L"000"));
        CPPUNIT_ASSERT(u12.sealsCount == 0u);
        CPPUNIT_ASSERT(u12.midWeightFullContainersCount == 0u);
        CPPUNIT_ASSERT(u12.midWeightEmptyContainersCount == 0u);
        CPPUNIT_ASSERT(u12.bigWeightFullContainersCount == 1u);
        CPPUNIT_ASSERT(u12.bigWeightEmptyContainersCount == 0u);
        // 2 блок 3 бит
        CPPUNIT_ASSERT_DOUBLES_EQUAL(23.5f, u12.tare, c_floatDelta);
        CPPUNIT_ASSERT(u12.parkType == std::wstring(L"20"));
        CPPUNIT_ASSERT(u12.accountingCarKind == std::wstring(L"60"));
        // 2 блок 6 бит
        CPPUNIT_ASSERT(u12.conventionalCarType == std::wstring(L"0600"));
        CPPUNIT_ASSERT(u12.axisCount == 4u);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.f, u12.conventionalCarLength, c_floatDelta);
        CPPUNIT_ASSERT(u12.trainTerminus == std::wstring(L"18000"));
        CPPUNIT_ASSERT(u12.destinationByPF == std::wstring(L"00000"));
        CPPUNIT_ASSERT(u12.carDeliveryJoint == std::wstring(L"11290"));
        CPPUNIT_ASSERT(u12.destinationNOD == std::wstring(L"68"));
        CPPUNIT_ASSERT(u12.lastTechnicalProcessingStation == std::wstring(L"09000"));
        CPPUNIT_ASSERT(u12.cargoKindForOperationalNomenclature == std::wstring(L"42"));
        CPPUNIT_ASSERT(u12.loadAndUnloadStation == std::wstring(L"00000"));
        CPPUNIT_ASSERT(u12.cargoSenderCode == std::wstring(L"0000"));
        CPPUNIT_ASSERT(u12.carQualityFlag == Asoup::M1042::Yu12::QualitySign::NothingWrong);
        CPPUNIT_ASSERT(u12.carOwnerCode == std::wstring(L"20"));
        CPPUNIT_ASSERT(u12.outputBorderStation == std::wstring(L"11280"));
        // 4 блок 4 бит
        CPPUNIT_ASSERT(u12.note == std::wstring(L"APEHДA"));
        CPPUNIT_ASSERT(u12.carSignsBytes == std::wstring(L"132"));
        CPPUNIT_ASSERT(u12.trackingSignsBytes == std::wstring(L"000"));
        CPPUNIT_ASSERT(u12.specialCarFeatures == std::wstring(L"00000000"));
    }

    {
        const Asoup::M1042::Yu12& u12 = m1042.u12[11];
        // L"Ю12 00 52673787 129169117183 0616482045100999900006 000258205206 1226318  0200     \n"
        //                               1 группа               2 группа     3 группа 4 группа
        // Совпадения                    10000001               10101001     01110101 10110111
        CPPUNIT_ASSERT(u12.adjustmentSign == std::wstring(L"00"));
        CPPUNIT_ASSERT(u12.carInventoryNumber == std::wstring(L"52673787"));

        // 1 группа
        CPPUNIT_ASSERT(u12.cargoWeight == 61u);
        CPPUNIT_ASSERT(u12.carDestinationStation == std::wstring(L"64820"));
        CPPUNIT_ASSERT(u12.cargoCode == std::wstring(L"45100"));
        CPPUNIT_ASSERT(u12.cargoReceiverCode == std::wstring(L"9999"));
        CPPUNIT_ASSERT(u12.specialSigns == std::wstring(L"000"));
        CPPUNIT_ASSERT(u12.sealsCount == 6u);

        // 2 группа
        CPPUNIT_ASSERT(u12.bigWeightFullContainersCount == 0u);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(25.8f, u12.tare, c_floatDelta);
        CPPUNIT_ASSERT(u12.accountingCarKind == std::wstring(L"20"));
        CPPUNIT_ASSERT(u12.conventionalCarType == std::wstring(L"5206"));

        // 3 группа
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.22f, u12.conventionalCarLength, c_floatDelta);
        CPPUNIT_ASSERT(u12.destinationNOD == std::wstring(L"63"));
        CPPUNIT_ASSERT(u12.cargoKindForOperationalNomenclature == std::wstring(L"18"));

        // 4 группа
        CPPUNIT_ASSERT(u12.carQualityFlag == Asoup::M1042::Yu12::QualitySign::NothingWrong);
        CPPUNIT_ASSERT(u12.carOwnerCode == std::wstring(L"20"));
        CPPUNIT_ASSERT(u12.note == std::wstring(L"0     "));
        CPPUNIT_ASSERT(u12.specialCarFeatures == std::wstring(L"00000000"));
    }
}

void TC_Asoup::tc_Parse1042_3()
{
    auto ptr = Asoup::Message::parse(c_1042_3, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false);
    CPPUNIT_ASSERT(ptr);
    const Asoup::Message& message = *ptr;
    CPPUNIT_ASSERT_MESSAGE("Должен быть 1 блок Ю1", 1u == message.get1042Info()->u1.size());
    const Asoup::M1042::Yu1& u1 = message.get1042Info()->u1[0];

    // Ю1 2413 1131 038 0900 01
    CPPUNIT_ASSERT(u1.trainNumber == std::wstring(L"2413"));
    CPPUNIT_ASSERT(u1.trainIndex == std::wstring(L"1131-038-0900"));
    CPPUNIT_ASSERT(u1.operationCode == Asoup::OperationCodeEnum::ArrivingByTrainToTheStation);
}

void TC_Asoup::tc_Parse333()
{
    auto ptr = Asoup::Message::parse(c_333, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false);
    CPPUNIT_ASSERT(ptr);
    const Asoup::Message& message = *ptr;
    // 1042 909/000+11290 2609 1800 541 1131 04 11310 27 01 12 45 00/00 0 0/00 00 0
    //                                       ^ начиная с этой отметки, данных о формате нет
    //                                       | и информация просто игнорируется
    verify(message, Asoup::AsoupMsgAttribute::MessageCode, 333);
    verify(message, Asoup::AsoupMsgAttribute::CanceledMessageCode, 1042);
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::EsrOfSenderStation, L"11290");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::InformationOriginPointCode, L"909/000");
    verify(message, Asoup::AsoupMsgAttribute::TrainNumber, 2609);
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::TrainIndex, L"1800-541-1131");
    verify(message, Trindex(1800, 541, 1131));
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::OperationCode, L"04");
    verify(message, Asoup::AsoupMsgAttribute::OperationCode, Asoup::OperationCodeEnum::TravelWithoutStopping);

    // Проследование без остановки, но направление движения указано только одним кодом, как расшифровать - неизвестно
    // verify(message, Asoup::Message::DirectionFrom, "");

    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::OperationDateTime, L"20150127T124500");
    CPPUNIT_ASSERT(!message.containsAttribute(Asoup::AsoupMsgAttribute::ParkAndWay));

    CPPUNIT_ASSERT_MESSAGE("Должен быть 1 блок Ю2", message.get333Info()->u2);
    // L"Ю2 0 00 00 00 00 0000 0 0 057 04713 03417 220 75085555 95316873 000 000 00 055 000.60 001 000 000 70 009 000 000 73 002 000 000 90 045 000 000 95 045 000 000:)";
    CPPUNIT_ASSERT_MESSAGE("Блок Ю2 содержит 5 разбиений по РПС", 5u == message.get333Info()->u2->decomposition.size());
}

void TC_Asoup::tc_TrimMessage()
{
    CPPUNIT_ASSERT(Asoup::Message::parse(c_333, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false));

    {
        const std::wstring msg333 = std::wstring(L" ") + c_333 + L" ";
        CPPUNIT_ASSERT(Asoup::Message::parse(msg333, m_receivedDate, Asoup::AsoupOperationMode::LDZ, true));
    }

    {
        const std::wstring msg333 = std::wstring(L"\t\n\n  ") + c_333 + L"\t\n\n";
        CPPUNIT_ASSERT(Asoup::Message::parse(msg333, m_receivedDate, Asoup::AsoupOperationMode::LDZ, true));
    }
}

void TC_Asoup::tc_Parse904_3()
{
    const std::wstring message = 
        L"\n\n(:904 0963 3508 0900 553 0963 1 02 03 12 58 056 03274 6 0000 0 0\n"
        L" 01 59277137 0252 004 09593  00300 0012 0 0 5 0 00/01 00000 000        0220 0141 0600\n"
        L" 02 59933945 0252 004 09593  00300 0012 0 0 5 0 00/01 00000 000        0220 0105 0720\n"
        L" 03 59916346 0202 004 09593  00300 0012 0 0 5 0 00/01 00000 000        0193 0105 0720\n"
        L" 04 53704102 0252 000 09593  42103 7564 9 2 5 0 00/00 00000 000 054102 0530 0130 0000\n"
        L" 05 59277202 0252 004 09593  00300 0012 0 0 5 0 00/01 00000 000        0220 0141 0600\n"
        L" 06 59992545 0252 004 09593  00300 0012 0 0 5 0 00/01 00000 000        0225 0141 0715\n"
        L" 07 58414202 0252 004 09593  00300 0012 0 0 5 0 00/01 00000 000        0225 0141 0715\n"
        L" 08 94526134 0252 052 09593  00300 0012 0 4 7 0 02/00 00000 000 ЗУБP   0249 0141 0690\n"
        L" 09 94522463 0252 052 09593  00300 0012 0 4 7 0 02/00 00000 000 ЗУБP   0249 0141 0690\n"
        L" 10 94523339 0252 052 09593  00300 0012 0 4 7 0 02/00 00000 000 ЗУБP   0248 0141 0690\n"
        L" 11 60049525 0213 054 09593  08103 2421 0 0 0 0 00/00 00000 000 ГTД    0232 0100 0700\n"
        L" 12 61252607 0213 054 09593  08103 2421 0 0 0 0 00/00 00000 000 ГTД    0233 0100 0700\n"
        L" 13 95029716 0252 065 09593  10300 4137 3 0 0 2 00/00 00000 000 11051  0233 0106 0705\n"
        L" 14 95330833 0252 065 09593  10300 4137 3 0 0 2 00/00 00000 000 11051  0232 0106 0705\n"
        L" 15 61590733 0202 000 09593  42100 7742 0 0 0 0 00/00 00000 000 APEHДA 0239 0100 0695\n"
        L" 16 61573903 0202 000 09593  42100 7742 5 0 0 0 00/00 00000 000 APEHДA 0238 0100 0695\n"
        L" 17 61638813 0202 000 09593  42100 7742 5 0 0 0 00/00 00000 000 APEHДA 0239 0100 0695\n"
        L" 18 60895000 0202 000 09593  42100 7742 5 0 0 0 00/00 00000 000 APEHДA 0237 0100 0695\n"
        L" 19 60021375 0202 000 09593  42100 7742 5 0 0 0 00/00 00000 000 APEHДA 0236 0100 0700\n"
        L" 20 60260023 0202 000 09593  42100 5819 0 0 0 0 00/00 00000 000 09108  0246 0100 0750\n"
        L" 21 63732630 0213 043 09593  08104 2421 0 0 0 0 00/00 00000 000 0      0232 0100 0700\n"
        L" 22 59915561 0252 051 09593  00300 0032 0 8 0 0 02/00 00000 000 0      0191 0105 0720\n"
        L" 23 59914101 0252 050 09593  00300 0032 0 8 0 0 02/00 00000 000 0      0195 0105 0700\n"
        L" 24 59934059 0252 052 09593  00300 0032 0 8 0 0 02/00 00000 000 0      0220 0105 0720\n"
        L" 25 95988812 0252 068 09593  10300 4137 4 0 0 2 00/00 00000 000        0234 0106 0705\n"
        L" 26 95330510 0252 069 09593  10300 4137 4 0 0 2 00/00 00000 000        0232 0106 0705\n"
        L" 27 95331344 0252 069 09593  10300 4137 4 0 0 2 00/00 00000 000        0231 0106 0705\n"
        L" 28 95872016 0252 070 09593  10300 4137 4 0 0 6 00/00 00000 000        0232 0106 0705\n"
        L" 29 95330205 0252 069 09593  10300 4137 4 0 0 2 00/00 00000 000        0232 0106 0705\n"
        L" 30 95331757 0252 069 09593  10300 4137 4 0 0 2 00/00 00000 000        0231 0106 0705\n"
        L" 31 95331849 0252 068 09593  10300 4137 4 0 0 2 00/00 00000 000        0231 0106 0705\n"
        L" 32 95029013 0252 068 09593  10300 4137 4 0 0 2 00/00 00000 000        0232 0106 0705\n"
        L" 33 95330940 0252 068 09593  10300 4137 4 0 0 2 00/00 00000 000        0232 0106 0705\n"
        L" 34 95330015 0252 067 09593  10300 4137 4 0 0 2 00/00 00000 000        0231 0106 0705\n"
        L" 35 95872347 0252 067 09593  10300 4137 4 0 0 6 00/00 00000 000        0232 0106 0705\n"
        L" 36 52881356 0202 000 09593  42100 5819 0 0 0 0 00/00 00000 000 09836- 0235 0100 0700\n"
        L" 37 59501338 0252 000 09634  43307 6302 9 0 0 0 00/00 00000 000 ПEPECЛ 0230 0106 0700\n"
        L" 38 59507707 0252 000 09634  43307 6302 9 0 0 0 00/00 00000 000 ПEPECЛ 0230 0106 0700\n"
        L" 39 28081305 0202 054 09593  09407 4357 0 0 5 2 00/00 00000 000 MПФ2ДB 0259 0122 0680\n"
        L" 40 95330601 0252 064 09593  10300 4137 3 0 0 2 00/00 00000 000        0232 0106 0705\n"
        L" 41 95337242 0252 064 09593  10300 4137 3 0 0 2 00/00 00000 000        0232 0106 0705\n"
        L" 42 95872370 0252 065 09593  10300 4137 3 0 0 6 00/00 00000 000        0231 0106 0705\n"
        L" 43 95872081 0252 065 09593  10300 4137 3 0 0 6 00/00 00000 000        0232 0106 0705\n"
        L" 44 95336905 0252 066 09593  10300 4137 3 0 0 2 00/00 00000 000        0233 0106 0705\n"
        L" 45 94523917 0252 052 09593  00300 0012 0 4 7 0 02/00 00000 000 ЗУБP   0248 0141 0690\n"
        L" 46 94520921 0252 052 09593  00300 0012 0 4 7 0 02/00 00000 000 ЗУБP   0249 0141 0690\n"
        L" 47 57685620 0202 058 09593  21403 5862 4 6 0 1 00/00 00000 000 OXP    0240 0086 0600\n"
        L" 48 50093327 0202 059 09593  21403 5862 4 6 0 1 00/00 00000 000 OXP    0242 0086 0590\n"
        L" 49 51971653 0202 060 09593  21403 5862 4 6 0 1 00/00 00000 000 OXP    0240 0086 0600\n"
        L" 50 58295122 0202 059 09593  21403 5862 4 6 0 1 00/00 00000 000 OXP    0232 0086 0600:)";

    CPPUNIT_ASSERT(Asoup::Message::parse(message, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false));
}

void TC_Asoup::tc_Parse1042_4()
{
    const std::wstring message = L"(:1042 909/000+09010 8554 0901 029 0970 07 09000 02 03 11 45 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 002 00055 00000 004 00970061 00970061 000 000 01 000 000\n"
        L"Ю12 00 00970061 1 000 09700 42100 7755 025 00 00 00 00 00 0550 70 01 0040 04 175 09700 00000 00000 01 09010 43 00000 0000 025 0 0000 PASS   128 000 00000000:)";

    CPPUNIT_ASSERT(Asoup::Message::parse(message, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false));
}

void TC_Asoup::tc_Parse1042_5()
{
    const std::wstring message = L"(:1042 909/000+16169 0087 0000 028 0087 03 11000 28 07 03 53 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 021 00720 00000 048 00000000 00000000 000 000 12 000 000.\n"
        L"Ю3 510 00000070 1 21 00 0000 00000 ЗEЛEHKOB    :)";

    CPPUNIT_ASSERT(Asoup::Message::parse(message, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false));
}

void TC_Asoup::tc_SavingTheSource()
{
    const std::wstring source = L"(:1042 909/000+16169 0087 0000 028 0087 03 11000 28 07 03 53 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 021 00720 00000 048 00000000 00000000 000 000 12 000 000.\n"
        L"Ю3 510 00000070 1 21 00 0000 00000 ЗEЛEHKOB    :)";

    {
        // Когда сохранение изменённого исходного текста не требуется, он не должен быть сохранён
        auto message = Asoup::Message::parse(source, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false);
        CPPUNIT_ASSERT(message);
        CPPUNIT_ASSERT(message->getSourceText().empty());
    }

    {
        // В данном тесте нет лишних символов, поэтому текст не должен отличаться
        auto message = Asoup::Message::parse(source, m_receivedDate, Asoup::AsoupOperationMode::LDZ, true);
        CPPUNIT_ASSERT(message);
        CPPUNIT_ASSERT_EQUAL(source, message->getSourceText());
    }    
}

void TC_Asoup::tc_CleaningTheSource()
{
    const std::wstring source = L"\t\r\n(:1042 909/000+16169 0087 0000 028 0087 03 11000 28 07 03 53 00/00 0 0/00 00 0\r\n"
        L"Ю2 0 00 00 00 00 0000 0 0 021 00720 00000 048 00000000 00000000 000 000 12 000 000.\r\n"
        L"Ю3 510 00000070 1 21 00 0000 00000 ЗEЛEHKOB    :)\r\n\t";
    const std::wstring expectedSource = L"(:1042 909/000+16169 0087 0000 028 0087 03 11000 28 07 03 53 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 021 00720 00000 048 00000000 00000000 000 000 12 000 000.\n"
        L"Ю3 510 00000070 1 21 00 0000 00000 ЗEЛEHKOB    :)";

    {
        // Когда сохранение изменённого исходного текста не требуется, он не должен быть сохранён
        auto message = Asoup::Message::parse(source, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false);
        CPPUNIT_ASSERT(message);
        CPPUNIT_ASSERT(message->getSourceText().empty());
    }

    {
        // В данном тесте вставлены переносы строк и прочий "мусор", который должен быть уничтожен
        auto message = Asoup::Message::parse(source, m_receivedDate, Asoup::AsoupOperationMode::LDZ, true);
        CPPUNIT_ASSERT(message);
        CPPUNIT_ASSERT_EQUAL(expectedSource, message->getSourceText());

        // Сохранённый текст должен быть достаточен для повторного разбора сообщения
        auto nextIteration = Asoup::Message::parse(message->getSourceText(), m_receivedDate, Asoup::AsoupOperationMode::LDZ, true);
        CPPUNIT_ASSERT(nextIteration);
        CPPUNIT_ASSERT_EQUAL(message->getSourceText(), nextIteration->getSourceText());
    }    
}

void verifyYear(std::shared_ptr<Asoup::Message> asoup, boost::gregorian::greg_year expectedYear)
{
    CPPUNIT_ASSERT(asoup);

    time_t asoupTime = 0;
    if (asoup->containsAttribute(Asoup::AsoupMsgAttribute::FormationDateTime))
        asoupTime = asoup->getAttributeDateTime(Asoup::AsoupMsgAttribute::FormationDateTime);
    else if (asoup->containsAttribute(Asoup::AsoupMsgAttribute::OperationDateTime))
        asoupTime = asoup->getAttributeDateTime(Asoup::AsoupMsgAttribute::OperationDateTime);

    boost::gregorian::date asoupDate(boost::posix_time::from_time_t(asoupTime).date());
    CPPUNIT_ASSERT(!asoupDate.is_not_a_date());

    boost::gregorian::greg_year asoupYear = asoupDate.year();

    CPPUNIT_ASSERT_EQUAL(expectedYear, asoupYear);
}

void checkYearValue27Jan(unsigned short year, unsigned short month, unsigned short day, unsigned short asoupYear)
{
    boost::gregorian::date date(year, month, day);

    // Сообщение от 27 января
    auto asoup = Asoup::Message::parse(c_904, date, Asoup::AsoupOperationMode::LDZ, true);
    verifyYear(asoup, asoupYear);
}

void TC_Asoup::tc_CreationTime()
{
    checkYearValue27Jan(m_receivedDate.year(), m_receivedDate.month(), m_receivedDate.day(), m_receivedDate.year());
    checkYearValue27Jan(2015, 1, 1, 2015);
    checkYearValue27Jan(2015, 1, 2, 2015);
    checkYearValue27Jan(2015, 12, 31, 2016);
    checkYearValue27Jan(1987, 2, 28, 1987);
    checkYearValue27Jan(2004, 2, 29, 2004);
}

void TC_Asoup::tc_NearYearChange()
{
    // сообщение в самом начале года ( 1 января 03:17 - из-за московского часового пояса(+3)  )
    const wchar_t *c_beg = L"(:1042 909/000+11310 2419 1131 039 0950 03 11420 01 01 03 17 01/01 0 0/00 00 0:)";
    // сообщение в самом конце года ( 31 декабря 23:57 )
    const wchar_t *c_end = L"(:1042 909/000+11310 2419 1131 039 0950 03 11420 31 12 23 57 31/12 0 0/00 00 0:)";

    // Ожидаемое поведение: одинаковый прием сообщений как до НГ, так и после НГ
    verifyYear(Asoup::Message::parse(c_beg, boost::gregorian::date(2008, 12, 31), Asoup::AsoupOperationMode::LDZ, true), 2009);
    verifyYear(Asoup::Message::parse(c_beg, boost::gregorian::date(2009,  1,  1), Asoup::AsoupOperationMode::LDZ, true), 2009);

    verifyYear(Asoup::Message::parse(c_end, boost::gregorian::date(2017, 12, 31), Asoup::AsoupOperationMode::LDZ, true), 2017);
    verifyYear(Asoup::Message::parse(c_end, boost::gregorian::date(2018,  1,  1), Asoup::AsoupOperationMode::LDZ, true), 2017);
}

void TC_Asoup::tc_Parse1042_6()
{
    const wchar_t *c_longYu12 = L"(:1042 909/000+09820 2240 0982 055 1600 07 09750 23 01 16 05 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01475 00000 248 61667671 54114145 000 005 00 000 057.60 000 023 000 70 000 034 005 73 000 033 005\n"
        L"Ю4 00000 0 00 000 034.70 000 034 73 000 033 \n"
        L"Ю4 16169 2 13 000 023.60 000 023 \n"
        L"Ю12 00 61667671 1 000 16000 16112 6302 500 00 00 00 00 00 0244 12 60 0600 04 100 16000 00000 16169 13 09820 01 00000 0000 020 1 1020 09826  128 000 00000000\n"
        L"Ю12 00 52259991 255237255255 02365600\n"
        L"Ю12 00 55939821 255239255255 0237\n"
        L"Ю12 00 53796710 255239255255 0236\n"
        L"Ю12 00 58475401 255255255255 \n"
        L"Ю12 00 52243094 251239255255 4000237\n"
        L"Ю12 00 56890163 255253255255 5608\n"
        L"Ю12 00 59908756 255253255255 5600\n"
        L"Ю12 00 74754508 235225125183 2210698002704072079208603021ПEPECЛ\n"
        L"Ю12 00 74722349 255239255255 0272\n"
        L"Ю12 00 74747247 255239255255 0270\n"
        L"Ю12 00 74722174 255255255255 \n"
        L"Ю12 00 74722547 255239255255 0271\n"
        L"Ю12 00 72322720 195231199135 000002210684100800240110072000000000220000009826 \n"
        L"Ю12 00 72815020 255239255255 0223\n"
        L"Ю12 00 72601248 255239255255 0232\n"
        L"Ю12 00 72868334 255239255255 0231\n"
        L"Ю12 00 72845407 255239255255 0226\n"
        L"Ю12 00 72030018 255239255255 0231\n"
        L"Ю12 00 72843014 255239255255 0233\n"
        L"Ю12 00 72843154 255239255255 0236\n"
        L"Ю12 00 72833304 255239255255 0243\n"
        L"Ю12 00 72797558 255239255255 0216\n"
        L"Ю12 00 72810518 255239255255 0231\n"
        L"Ю12 00 72895204 255255255255 \n"
        L"Ю12 00 72851330 255239255255 0238\n"
        L"Ю12 00 72321813 255239255255 0230\n"
        L"Ю12 00 72353469 255239255255 0225\n"
        L"Ю12 00 72815368 255239255255 0169\n"
        L"Ю12 00 72098544 255239255255 0240\n"
        L"Ю12 00 72872211 255239255255 0244\n"
        L"Ю12 00 72831811 255239255255 0255\n"
        L"Ю12 00 72882830 255239255255 0245\n"
        L"Ю12 00 72353956 255239255255 0236\n"
        L"Ю12 00 54198502 195225069143 160001610463023000240126056001000000016169130102011020\n"
        L"Ю12 00 56250038 255239255255 0236\n"
        L"Ю12 00 55450407 255239255255 0240\n"
        L"Ю12 00 55106074 255255255255 \n"
        L"Ю12 00 55124689 255239255255 0241\n"
        L"Ю12 00 54359047 251239255255 5000242\n"
        L"Ю12 00 60540333 255237255255 02400600\n"
        L"Ю12 00 72242555 195241069143 00000221068410080117207920860072000000000302200000\n"
        L"Ю12 00 72818982 255239255255 0247\n"
        L"Ю12 00 72792336 255239255255 0228\n"
        L"Ю12 00 72303837 255239255255 0169\n"
        L"Ю12 00 72843105 255239255255 0248\n"
        L"Ю12 00 72303589 255239255255 0233\n"
        L"Ю12 00 72785009 255239255255 0227\n"
        L"Ю12 00 72795511 255239255255 0241\n"
        L"Ю12 00 72845845 255239255255 0231\n"
        L"Ю12 00 72351224 255239255255 0237\n"
        L"Ю12 00 72824220 227233223255 211035906060026171079100710\n"
        L"Ю12 00 73716011 255239255255 0230\n"
        L"Ю12 00 77217073 255233223255 023570077400700\n"
        L"Ю12 00 57953820 195225069143 160001610463024000241126056001000000016169130102011020\n"
        L"Ю12 00 54167440 255239255255 0240\n"
        L"Ю12 00 52346145 255255255255 \n"
        L"Ю12 00 55428148 255255255255 \n"
        L"Ю12 00 54368154 251239255255 3000243\n"
        L"Ю12 00 56158728 255255255255 \n"
        L"Ю12 00 55450506 255239255255 0240\n"
        L"Ю12 00 54114145 255255255255 :)";

    std::shared_ptr<Asoup::Message> message = Asoup::Message::parse(c_longYu12, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false);
    CPPUNIT_ASSERT(message);
    CPPUNIT_ASSERT(message->get1042Info());
    const auto& m1042 = *message->get1042Info();
    CPPUNIT_ASSERT_EQUAL((size_t)62u, m1042.u12.size());

    {
        const Asoup::M1042::Yu12& u12 = m1042.u12[0];
        // L"Ю12 00 61667671 1 000 16000 16112 6302 500 00 00 00 00 00 0244 12 60 0600 04 100 16000 00000 16169 13 09820 01 00000 0000 020 1 1020 09826  128 000 00000000\n"
        CPPUNIT_ASSERT(u12.adjustmentSign == std::wstring(L"00"));
        CPPUNIT_ASSERT(u12.carInventoryNumber == std::wstring(L"61667671"));

        // 1 группа
        CPPUNIT_ASSERT(u12.castorsFlags == L'1');
        CPPUNIT_ASSERT(u12.cargoWeight == 0u);
        CPPUNIT_ASSERT(u12.carDestinationStation == std::wstring(L"16000"));
        CPPUNIT_ASSERT(u12.cargoCode == std::wstring(L"16112"));
        CPPUNIT_ASSERT(u12.cargoReceiverCode == std::wstring(L"6302"));
        CPPUNIT_ASSERT(u12.specialSigns == std::wstring(L"500"));
        CPPUNIT_ASSERT(u12.sealsCount == 0u);
        CPPUNIT_ASSERT(u12.midWeightFullContainersCount == 0u);

        // 2 группа
        CPPUNIT_ASSERT(u12.midWeightEmptyContainersCount == 0u);
        CPPUNIT_ASSERT(u12.bigWeightFullContainersCount == 0u);
        CPPUNIT_ASSERT(u12.bigWeightEmptyContainersCount == 0u);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(24.4f, u12.tare, c_floatDelta);
        CPPUNIT_ASSERT(u12.parkType == std::wstring(L"12"));
        CPPUNIT_ASSERT(u12.accountingCarKind == std::wstring(L"60"));
        CPPUNIT_ASSERT(u12.conventionalCarType == std::wstring(L"0600"));
        CPPUNIT_ASSERT(u12.axisCount == 4u);

        // 3 группа
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.f, u12.conventionalCarLength, c_floatDelta);
        CPPUNIT_ASSERT(u12.trainTerminus == std::wstring(L"16000"));
        CPPUNIT_ASSERT(u12.destinationByPF == std::wstring(L"00000"));
        CPPUNIT_ASSERT(u12.carDeliveryJoint == std::wstring(L"16169"));
        CPPUNIT_ASSERT(u12.destinationNOD == std::wstring(L"13"));
        CPPUNIT_ASSERT(u12.lastTechnicalProcessingStation == std::wstring(L"09820"));
        CPPUNIT_ASSERT(u12.cargoKindForOperationalNomenclature == std::wstring(L"01"));
        CPPUNIT_ASSERT(u12.loadAndUnloadStation == std::wstring(L"00000"));

        // 4 группа
        CPPUNIT_ASSERT(u12.cargoSenderCode == std::wstring(L"0000"));
        CPPUNIT_ASSERT(u12.carQualityFlag == Asoup::M1042::Yu12::QualitySign::NothingWrong);
        CPPUNIT_ASSERT(u12.carOwnerCode == std::wstring(L"20"));
        CPPUNIT_ASSERT(u12.outputBorderStation == std::wstring(L"11020"));
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"09826"), u12.note);
        CPPUNIT_ASSERT(u12.carSignsBytes == std::wstring(L"128"));
        CPPUNIT_ASSERT(u12.trackingSignsBytes == std::wstring(L"000"));
        CPPUNIT_ASSERT(u12.specialCarFeatures == std::wstring(L"00000000"));
    }

    {
        // L"Ю12 00 61667671 1 000 16000 16112 6302 500 00 00 00 00 00 0244 12 60 0600 04 100 16000 00000 16169 13 09820 01 00000 0000 020 1 1020 09826  128 000 00000000\n"
        // L"Ю12 00 74754508 235225125183 2210698002704072079208603021ПEPECЛ\n"
        const Asoup::M1042::Yu12& u12 = m1042.u12[8];
        
        CPPUNIT_ASSERT(u12.adjustmentSign == std::wstring(L"00"));
        CPPUNIT_ASSERT(u12.carInventoryNumber == std::wstring(L"74754508"));

        // 1 группа 11101011
        //CPPUNIT_ASSERT(u12.castorsFlags == L'1');
        //CPPUNIT_ASSERT(u12.cargoWeight == 0u);
        //CPPUNIT_ASSERT(u12.carDestinationStation == std::wstring(L"16000"));
        CPPUNIT_ASSERT(u12.cargoCode == std::wstring(L"22106"));
        //CPPUNIT_ASSERT(u12.cargoReceiverCode == std::wstring(L"6302"));
        CPPUNIT_ASSERT(u12.specialSigns == std::wstring(L"980"));
        //CPPUNIT_ASSERT(u12.sealsCount == 0u);
        //CPPUNIT_ASSERT(u12.midWeightFullContainersCount == 0u);

        // 2 группа 11100001
        //CPPUNIT_ASSERT(u12.midWeightEmptyContainersCount == 0u);
        //CPPUNIT_ASSERT(u12.bigWeightFullContainersCount == 0u);
        //CPPUNIT_ASSERT(u12.bigWeightEmptyContainersCount == 0u);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(27.f, u12.tare, c_floatDelta);
        CPPUNIT_ASSERT(u12.parkType == std::wstring(L"40"));
        CPPUNIT_ASSERT(u12.accountingCarKind == std::wstring(L"72"));
        CPPUNIT_ASSERT(u12.conventionalCarType == std::wstring(L"0792"));
        //CPPUNIT_ASSERT(u12.axisCount == 4u);

        // 3 группа 01111101
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.86f, u12.conventionalCarLength, c_floatDelta);
        //CPPUNIT_ASSERT(u12.trainTerminus == std::wstring(L"16000"));
        //CPPUNIT_ASSERT(u12.destinationByPF == std::wstring(L"00000"));
        //CPPUNIT_ASSERT(u12.carDeliveryJoint == std::wstring(L"16169"));
        //CPPUNIT_ASSERT(u12.destinationNOD == std::wstring(L"13"));
        //CPPUNIT_ASSERT(u12.lastTechnicalProcessingStation == std::wstring(L"09820"));
        CPPUNIT_ASSERT(u12.cargoKindForOperationalNomenclature == std::wstring(L"03"));
        //CPPUNIT_ASSERT(u12.loadAndUnloadStation == std::wstring(L"00000"));

        // 4 группа 10110111
        //CPPUNIT_ASSERT(u12.cargoSenderCode == std::wstring(L"0000")); // 1
        CPPUNIT_ASSERT(u12.carQualityFlag == Asoup::M1042::Yu12::QualitySign::NothingWrong); // 2
        CPPUNIT_ASSERT(u12.carOwnerCode == std::wstring(L"21")); // 2
        //CPPUNIT_ASSERT(u12.outputBorderStation == std::wstring(L"11020")); // 3, 4
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"ПEPECЛ"), u12.note); // 5
        //CPPUNIT_ASSERT(u12.carSignsBytes == std::wstring(L"128")); // 6
        //CPPUNIT_ASSERT(u12.trackingSignsBytes == std::wstring(L"000")); // 7
        //CPPUNIT_ASSERT(u12.specialCarFeatures == std::wstring(L"00000000")); // 8
    }
}

void TC_Asoup::tc_Parse1042_7()
{
    const wchar_t* messageText = L"(:1042 909/000+11760 4901 0001 089 0900 04 11420+09000 20 02 08 09 00/00 0 0/00 00 0\n"
        L"Ю3 579 00001261 1 04 30 0000 00000 BIZNJA      \n"
        L"Ю3 579 00001262 9\n"
        L"Ю3 539 00002931 4 04 30 0000 00000 OSTROUHOVS  \n"
        L"Ю3 539 00002932 9\n"
        L"Ю3 539 00007181 4 04 30 0000 00000 GNEZDILOVS  \n"
        L"Ю3 539 00007182 9:)\n";

    std::shared_ptr<Asoup::Message> message = Asoup::Message::parse(messageText, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false);
    CPPUNIT_ASSERT(message);
    CPPUNIT_ASSERT(message->get1042Info());
    const auto& u3 = message->get1042Info()->u3;
    CPPUNIT_ASSERT_EQUAL(3u, (unsigned)u3.size());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"BIZNJA"), u3[0u].machinistSurname);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"0126"), u3[0u].locomotiveNumber);
    CPPUNIT_ASSERT_EQUAL(4u, (unsigned)u3[0u].appearanceTime.first);
    CPPUNIT_ASSERT_EQUAL(30u, (unsigned)u3[0u].appearanceTime.second);
    CPPUNIT_ASSERT_EQUAL(2u, u3[0u].sectionsCount);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"OSTROUHOVS"), u3[1u].machinistSurname);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"0293"), u3[1u].locomotiveNumber);
    CPPUNIT_ASSERT_EQUAL(4u, (unsigned)u3[1u].appearanceTime.first);
    CPPUNIT_ASSERT_EQUAL(30u, (unsigned)u3[1u].appearanceTime.second);
    CPPUNIT_ASSERT_EQUAL(2u, u3[1u].sectionsCount);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"GNEZDILOVS"), u3[2u].machinistSurname);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"0718"), u3[2u].locomotiveNumber);
    CPPUNIT_ASSERT_EQUAL(4u, (unsigned)u3[2u].appearanceTime.first);
    CPPUNIT_ASSERT_EQUAL(30u, (unsigned)u3[2u].appearanceTime.second);
    CPPUNIT_ASSERT_EQUAL(2u, u3[2u].sectionsCount);
}

void TC_Asoup::tc_February29()
{
    CPPUNIT_ASSERT(Asoup::Message::parse(
        L"(:1042 909/000+09500 4871 0001 015 0950 01 09000 29 02 10 20 00/00 1 0/00 00 0:)", m_receivedDate, Asoup::AsoupOperationMode::LDZ, true));

    CPPUNIT_ASSERT(Asoup::Message::parse(
        L"(:1042 909/000+11340 8502 1100 232 1134 01 11000 29 02 09 58 00/00 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 001 00038 00014 004 19016716 19016716 001 000 00 000 000:)", m_receivedDate, Asoup::AsoupOperationMode::LDZ, true));
}

void TC_Asoup::tc_Parse17082016()
{
    std::wstring const message = L"(:1042 909/000+09830 2210 0983 012 1800 03 09750 17 08 05 00 06/05 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01725 00000 264 50560572 55945851 000 001 00 000 065.70 000 065 001 73 000 010 000\n"
        L"Ю3 530 00009481 1 03 11 0000 00000 ROMANCUKS   \n"
        L"Ю3 530 00009482 9\n"
        L"Ю4 11290 2 63 000 065.70 000 065 73 000 010 \n"
        L"Ю4 00000 3 00 001 000.70 001 000 \n"
        L"Ю12 00 50560572 1 000 64840 22602 8683 460 00 00 00 00 00 0249 12 70 5700 04 086 18000 00000 11290 63 09830 03 00000 0000 020 1 1280 09826  128 000 00000000\n"
        L"Ю12 00 51469484 255239255255 0265\n"
        L"Ю12 00 53953378 255239255255 0268\n"
        L"Ю12 00 51531358 251239255255 3600272\n"
        L"Ю12 00 51467538 255239255255 0265\n"
        L"Ю12 00 50559814 255239255255 0249\n"
        L"Ю12 00 50560549 255255255255 \n"
        L"Ю12 00 50567080 255255255255 \n"
        L"Ю12 00 51043966 255239255255 0247\n"
        L"Ю12 00 53953386 251239255255 5600268\n"
        L"Ю12 00 51552750 255255255255 \n"
        L"Ю12 00 51552438 255255255255 \n"
        L"Ю12 00 50682434 255239255255 0232\n"
        L"Ю12 00 51565257 255239255255 0271\n"
        L"Ю12 00 51466977 255239255255 0275\n"
        L"Ю12 00 53951810 251239255255 4600268\n"
        L"Ю12 00 50591874 255239255255 0249\n"
        L"Ю12 00 51467645 255239255255 0265\n"
        L"Ю12 00 53952867 255239255255 0268\n"
        L"Ю12 00 51428852 195231255247 64780226025112960026540ПEPECЛ\n"
        L"Ю12 00 51249753 195231255247 6484022602868346002491209826 \n"
        L"Ю12 00 51552578 255239255255 0268\n"
        L"Ю12 00 50579416 251239255255 3600249\n"
        L"Ю12 00 51551067 255239255255 0271\n"
        L"Ю12 00 50559616 255239255255 0249\n"
        L"Ю12 00 53952982 255239255255 0268\n"
        L"Ю12 00 51287589 255239255255 0248\n"
        L"Ю12 00 51467041 255239255255 0275\n"
        L"Ю12 00 53952859 251239255255 4600268\n"
        L"Ю12 00 51552552 195255255255 64780226025112560\n"
        L"Ю12 00 51428712 195239255255 648402260286834600265\n"
        L"Ю12 00 50682210 255239255255 0249\n"
        L"Ю12 00 51428555 255239255255 0275\n"
        L"Ю12 00 51529436 255239255255 0271\n"
        L"Ю12 00 51467520 195239255255 647802260251125600265\n"
        L"Ю12 00 51514024 195239255255 648402260286834600271\n"
        L"Ю12 00 50567130 251239255255 3600249\n"
        L"Ю12 00 50581107 255255255255 \n"
        L"Ю12 00 50949577 195239255255 635602110344305600232\n"
        L"Ю12 00 73414054 255233255255 0238710791\n"
        L"Ю12 00 53856068 255233255255 0272705700\n"
        L"Ю12 00 51903714 251239255255 3600270\n"
        L"Ю12 00 75026245 251233255191 4600260710791027\n"
        L"Ю12 00 75027250 255255255255 \n"
        L"Ю12 00 75024307 251255255255 560\n"
        L"Ю12 00 75027086 255255255255 \n"
        L"Ю12 00 53994844 251233255191 3600269705700020\n"
        L"Ю12 00 50963719 255239255255 0240\n"
        L"Ю12 00 50150317 255239255255 0270\n"
        L"Ю12 00 53876959 255239255255 0278\n"
        L"Ю12 00 51799096 255239255255 0262\n"
        L"Ю12 00 53936647 251239255255 4600265\n"
        L"Ю12 00 51036176 255239255255 0276\n"
        L"Ю12 00 53957056 255239255255 0265\n"
        L"Ю12 00 75026872 251233255191 5600260710791027\n"
        L"Ю12 00 50977107 255233255255 0266705700\n"
        L"Ю12 00 50610732 251239255191 4600240020\n"
        L"Ю12 00 73530487 227233255255 2110346823600269710791\n"
        L"Ю12 00 55052930 255233255255 0271705700\n"
        L"Ю12 00 74983537 255233255255 0268710791\n"
        L"Ю12 00 57271645 255233255255 0271705700\n"
        L"Ю12 00 73922379 227233255191 2110344305600260710791027\n"
        L"Ю12 00 74989708 255239255255 0267\n"
        L"Ю12 00 50795269 251233255191 4600240705700020\n"
        L"Ю12 00 53967667 255239255255 0269\n"
        L"Ю12 00 55945851 227239255255 2110346823600270:)";
        auto parseResult = Asoup::Message::parse(message, m_receivedDate, Asoup::AsoupOperationMode::LDZ, true);
        CPPUNIT_ASSERT(parseResult);
}

void TC_Asoup::tc_Parse1042_Yu3_WithoutMachinist()
{
    auto ptr = Asoup::Message::parse(c_1042_withoutMachinist, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false);
    CPPUNIT_ASSERT(ptr);
    const Asoup::Message& message = *ptr;
    // 909/000+11310 2419 1131 039 0950 03 11420 27 01 08 30 00/00 0 0/00 00 0
    verify(message, Asoup::AsoupMsgAttribute::MessageCode, 1042u);
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::EsrOfSenderStation, L"11310");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::InformationOriginPointCode, L"909/000");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::TrainNumber, L"2419");
    verify(message, Asoup::AsoupMsgAttribute::TrainNumber, 2419u);
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::TrainIndex, L"1131-039-0950");
    verify(message, Trindex(1131, 39, 950));
    verify(message, Asoup::AsoupMsgAttribute::OperationCode, Asoup::OperationCodeEnum::DepartureOfTheTrainFromTheStation);
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::DirectionTo, L"11420");
    CPPUNIT_ASSERT(!message.containsAttribute(Asoup::AsoupMsgAttribute::DirectionFrom));
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::OperationDateTime, L"20150127T083000");
    verifyDateTime(message, Asoup::AsoupMsgAttribute::OperationDateTime, 
        timeForLocalDateTime(2015u, 1u, 27u, 8u, 30u));
    CPPUNIT_ASSERT_EQUAL(false, message.containsAttribute(Asoup::AsoupMsgAttribute::ParkAndWay));
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::WayOfOvertake, L"0");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::FullDaysOfDelayAndReason, L"0/00");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::ConnectionCodeOrTrowCode, L"00");
    verify<Asoup::Message, Asoup::AsoupMsgAttribute>(message, Asoup::AsoupMsgAttribute::TrainCategory, L"0");

    const Asoup::M1042& m1042 = *message.get1042Info();

    CPPUNIT_ASSERT_MESSAGE("Число блоков Ю1 должно быть равно 0", 0u == m1042.u1.size());

    CPPUNIT_ASSERT_MESSAGE("Блок Ю2 должен сущестовать", nullptr != m1042.u2.get());
    Asoup::M1042::Yu2& u2 = *m1042.u2;
    // L"Ю2 0 00 00 00 00 0000 0 0 042 02429 01647 136 56069321 52600855 000 000 00 028 006.20 001 000 000 60 004 000 000 90 023 006 000 95 012 000 000 96 011 006 000\n"
    CPPUNIT_ASSERT_EQUAL(L'0', u2.specialCharacteristics.category);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"00"), u2.specialCharacteristics.highWeight);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"00"), u2.specialCharacteristics.longTrain);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"00"), u2.specialCharacteristics.highLoad);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"00"), u2.specialCharacteristics.connectedTrains);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"0000"), u2.specialCharacteristics.oversizeIndex);
    CPPUNIT_ASSERT_EQUAL(L'0', u2.specialCharacteristics.poultry);
    CPPUNIT_ASSERT_EQUAL(L'0', u2.specialCharacteristics.route);
    CPPUNIT_ASSERT_EQUAL(42u, u2.conventionalLength);
    CPPUNIT_ASSERT_EQUAL(2429u, u2.bruttoWeight);
    CPPUNIT_ASSERT_EQUAL(1647u, u2.nettoWeight);
    CPPUNIT_ASSERT_EQUAL(136u, u2.axisCount);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"56069321"), u2.headCarNumber);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"52600855"), u2.tailCarNumber);
    CPPUNIT_ASSERT_EQUAL(0u, u2.NDLMCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.NRPCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.passengerCarsCount);
    CPPUNIT_ASSERT_EQUAL(28u, u2.fullActiveParkCarsCount);
    CPPUNIT_ASSERT_EQUAL(6u, u2.emptyActiveParkCarsCount);

    CPPUNIT_ASSERT_MESSAGE("Число блоков разложения по НРП должно быть равно 5", 5u == u2.decomposition.size());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"20"), u2.decomposition[0].RPSCode);
    CPPUNIT_ASSERT_EQUAL(1u, u2.decomposition[0].fullCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.decomposition[0].emptyCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.decomposition[0].inactiveParkCarsCount);

    CPPUNIT_ASSERT_EQUAL(std::wstring(L"96"), u2.decomposition[4].RPSCode);
    CPPUNIT_ASSERT_EQUAL(11u, u2.decomposition[4].fullCarsCount);
    CPPUNIT_ASSERT_EQUAL(6u, u2.decomposition[4].emptyCarsCount);
    CPPUNIT_ASSERT_EQUAL(0u, u2.decomposition[4].inactiveParkCarsCount);

    CPPUNIT_ASSERT_MESSAGE("Число блоков Ю3 должно быть равно 1", 1u == m1042.u3.size());
    // L"Ю3 539 00012331 1 00 00 0000 00000 0           \n"
    // L"Ю3 539 00012332 9:)";
    CPPUNIT_ASSERT_EQUAL(539u, m1042.u3[0].seriesCode);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Секций в блоке должно быть две", 2u, m1042.u3[0].sectionsCount);
    CPPUNIT_ASSERT_EQUAL(0, m1042.u3[0].locomotiveNumber.compare(L"1233"));
    CPPUNIT_ASSERT(Asoup::M1042::Yu3::ConsecutionType::TrainHead == m1042.u3[0].consecutionTypeCode);
    CPPUNIT_ASSERT_EQUAL((unsigned short)0u, m1042.u3[0].appearanceTime.first);
    CPPUNIT_ASSERT_EQUAL((unsigned short)0u, m1042.u3[0].appearanceTime.second);
    CPPUNIT_ASSERT_EQUAL(0, m1042.u3[0].registryDepot.compare(L"0000"));
    CPPUNIT_ASSERT_EQUAL(0, m1042.u3[0].machinistPersonnelNumber.compare(L"00000"));
    CPPUNIT_ASSERT(m1042.u3[0].machinistSurname.empty());
}

void TC_Asoup::tc_IsWithdrawal()
{
    std::wstring const text333 = L"(:333 1042 909/000+11290 2609 1800 541 1131 04 11310 27 01 12 45 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 04713 03417 220 75085555 95316873 000 000 00 055 000.60 001 000 000 70 009 000 000 73 002 000 000 90 045 000 000 95 045 000 000:)";
    auto asoup333 = Asoup::Message::parse(text333, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false);
    CPPUNIT_ASSERT(asoup333);
    CPPUNIT_ASSERT(asoup333->isWithdrawalMessage());

    CPPUNIT_ASSERT(!asoup333->isWithdrawalMessageFor(c_1042, m_receivedDate, Asoup::AsoupOperationMode::LDZ));
    CPPUNIT_ASSERT(!asoup333->isWithdrawalMessageFor(c_1042_2, m_receivedDate, Asoup::AsoupOperationMode::LDZ));
    CPPUNIT_ASSERT(!asoup333->isWithdrawalMessageFor(c_1042_3, m_receivedDate, Asoup::AsoupOperationMode::LDZ));

    // Сообщение с кодом 333 невозможно разобрать парсером 333 при сравнении, т.к. код сообщения опускается 
    CPPUNIT_ASSERT_THROW(asoup333->isWithdrawalMessageFor(text333, m_receivedDate, Asoup::AsoupOperationMode::LDZ), std::runtime_error);
    // Служебный блок сообщений ТГНЛ не поддерживается парсером сообщения отмены
    CPPUNIT_ASSERT_THROW(asoup333->isWithdrawalMessageFor(c_904, m_receivedDate, Asoup::AsoupOperationMode::LDZ), std::runtime_error);
    CPPUNIT_ASSERT_THROW(asoup333->isWithdrawalMessageFor(c_904_2, m_receivedDate, Asoup::AsoupOperationMode::LDZ), std::runtime_error);

    // Текст сообщения получен вычитанием из предыдущего кода 333
    std::wstring const text1042 = L"(:1042 909/000+11290 2609 1800 541 1131 04 11310 27 01 12 45 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 04713 03417 220 75085555 95316873 000 000 00 055 000.60 001 000 000 70 009 000 000 73 002 000 000 90 045 000 000 95 045 000 000:)";
    auto asoup1042 = Asoup::Message::parse(text1042, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false);
    CPPUNIT_ASSERT(asoup1042);
    CPPUNIT_ASSERT(!asoup1042->isWithdrawalMessage());

    CPPUNIT_ASSERT(asoup333->isWithdrawalMessageFor(text1042, m_receivedDate, Asoup::AsoupOperationMode::LDZ));
    
    // Похоже, дата получения сообщения не влияет на его служебный блок, это поведение может измениться в дальнейшем
    CPPUNIT_ASSERT(asoup333->isWithdrawalMessageFor(text1042, m_receivedDate + boost::gregorian::days(2), Asoup::AsoupOperationMode::LDZ));

    CPPUNIT_ASSERT(!asoup1042->isWithdrawalMessageFor(text333, m_receivedDate, Asoup::AsoupOperationMode::LDZ));
    CPPUNIT_ASSERT(!asoup1042->isWithdrawalMessageFor(text1042, m_receivedDate, Asoup::AsoupOperationMode::LDZ));
}

// варианты Ю12
//     Ю12 00 00000075 1 020 11050 23200 7755 100 00 00 00 00 00 0707 60 30 7019 04 193 11050 00000 00000 02 11000 00 00000 0000 000 0 0000 94     132 000 00000000:)]]></AsoupEvent>
//     Ю12 00 98016579 255253255255 0944
//     Ю12 11 50475243
//     Ю12 13 19005032 239236127255 860401020055106130
//     Ю12 13 19016955 255237127255 02100575105
//     Ю12 13 19016971 1 000 09180 87019 7755 000 00 00 00 00 00 0210 50 10 0575 04 105 09180 09180 00000 01 09940 00 00000 0000 000 0 0000 0      128 000 00000000:)]]></AsoupEvent>
//     Ю12 13 98016579 255253255255 0944
//     Ю12 14 24093114 129225124051 0671414028104207600002024020200206106230932034110250     148

/*
00 - ФPAЗA CФOPMИPOBAHA ПO C.02
99 - ФPAЗA CФOPMИPOBAHA ПO ПOBTOPHOЙ TГHЛ
01, 81 - ИCKЛЮЧEH BAГOH. ФPAЗA HEПOЛHAЯ.
02 - ЗAMEHA ИHBEHTAPHOГO HOMEPA. OБЯЗATEЛЬHЫ  ДBE ИHФOPMAЦИOHHЫE ФPAЗЫ:
    ПEPBAЯ C KOДOM 02 - KOPOTKAЯ  C  HEПPABИЛЬHЫM  HOMEPOM BAГOHA.
    CЛEДOM ЗA HEЙ C KOДOM 22 - C ПPABИЛЬHЫM HOMEPOM BAГOHA (ПOЛHAЯ).
04 - BKЛЮЧИTЬ BAГOH.
05 - ПEPECOPTИPOBKA.
03, 33, 83 - ЗAMEHA CBEДEHИЙ O BAГOHE.
13, 43 - ПEPEAДPECOBKA.
08, 88 - ЗAMEHA OTДEЛЬHЫX ПOKAЗATEЛEЙ.
11 - OTЦEПKA. ФPAЗA HEПOЛHAЯ.
14 - ПPИЦEПKA.
 */
void TC_Asoup::tc_Parse1042_Yu12_upd()
{
    const wchar_t *updYu12 =
        L"(:1042 909/000+09180 9501 1131 024 0987 12 00000 23 10 00 25 01/06 0 0/00 00 0\n"
        L"Ю4 00000 1 01 051 000.70 006 000 90 045 000 95 045 000 \n"
        L"Ю4 09870 1 01 051 000.70 006 000 90 045 000 95 045 000 \n"
        L"Ю12 00 59092320 1 000 82000 01100 7351 000 00 00 00 00 00 0235 12 95 5935 04 106 09180 09000 11290 80 09330 34 00000 0000 020 1 1280 09336  128 000 00000000\n"
        L"Ю12 00 95467577 255237255255 02320950\n"
        L"Ю12 11 50475243\n"
        L"Ю12 00 53207924 255237255255 02275935\n"
        L"Ю12 13 19641430 235237127247 86060100022005771050     \n"
        L"Ю12 14 58088832 129225099207 071110002810483335000001933093533008600000021100000000\n"
        L"Ю12 14 58088659 191255255255 069\n"
        L"Ю12 11 58565425:)";

    m_receivedDate = boost::gregorian::date(2018, 10, 22);
    std::shared_ptr<Asoup::Message> message = Asoup::Message::parse(updYu12, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false);
    CPPUNIT_ASSERT(message);
    CPPUNIT_ASSERT(message->get1042Info());
    const auto& m1042 = *message->get1042Info();
    CPPUNIT_ASSERT_EQUAL( size_t(8), m1042.u12.size() );

    CPPUNIT_ASSERT(m1042.u12[0].adjustmentSign == std::wstring(L"00"));
    CPPUNIT_ASSERT(m1042.u12[1].adjustmentSign == std::wstring(L"00"));
    CPPUNIT_ASSERT(m1042.u12[2].adjustmentSign == std::wstring(L"11"));
    CPPUNIT_ASSERT(m1042.u12[3].adjustmentSign == std::wstring(L"00"));
    CPPUNIT_ASSERT(m1042.u12[4].adjustmentSign == std::wstring(L"13"));
    CPPUNIT_ASSERT(m1042.u12[5].adjustmentSign == std::wstring(L"14"));
    CPPUNIT_ASSERT(m1042.u12[6].adjustmentSign == std::wstring(L"14"));
    CPPUNIT_ASSERT(m1042.u12[7].adjustmentSign == std::wstring(L"11"));

    CPPUNIT_ASSERT(m1042.u12[0].carInventoryNumber == std::wstring(L"59092320"));
    CPPUNIT_ASSERT(m1042.u12[1].carInventoryNumber == std::wstring(L"95467577"));
    CPPUNIT_ASSERT(m1042.u12[2].carInventoryNumber == std::wstring(L"50475243"));
    CPPUNIT_ASSERT(m1042.u12[3].carInventoryNumber == std::wstring(L"53207924"));
    CPPUNIT_ASSERT(m1042.u12[4].carInventoryNumber == std::wstring(L"19641430"));
    CPPUNIT_ASSERT(m1042.u12[5].carInventoryNumber == std::wstring(L"58088832"));
    CPPUNIT_ASSERT(m1042.u12[6].carInventoryNumber == std::wstring(L"58088659"));
    CPPUNIT_ASSERT(m1042.u12[7].carInventoryNumber == std::wstring(L"58565425"));
}

void TC_Asoup::tc_ParseMessagePassagirInfo()
{
    auto ptr = Asoup::MessagePassengerLDZ::parse(c_PassagirMsgInfo);
    CPPUNIT_ASSERT(ptr);
    const Asoup::MessagePassengerLDZ& message = *ptr;
    verify<Asoup::MessagePassengerLDZ, Asoup::AsoupMsgPassengerLdzAttribute>(message, Asoup::AsoupMsgPassengerLdzAttribute::OperationCode, L"1");
    verify<Asoup::MessagePassengerLDZ, Asoup::AsoupMsgPassengerLdzAttribute>(message, Asoup::AsoupMsgPassengerLdzAttribute::OperationEsrStation, L"12345");
    verify<Asoup::MessagePassengerLDZ, Asoup::AsoupMsgPassengerLdzAttribute>(message, Asoup::AsoupMsgPassengerLdzAttribute::TrainNumber, L"1235");
    verify<Asoup::MessagePassengerLDZ, Asoup::AsoupMsgPassengerLdzAttribute>(message, Asoup::AsoupMsgPassengerLdzAttribute::Location, L"0,0");
    verify<Asoup::MessagePassengerLDZ, Asoup::AsoupMsgPassengerLdzAttribute>(message, Asoup::AsoupMsgPassengerLdzAttribute::OperationDateTime, L"12-02-2019 15:26");

    const Asoup::MsgPassengerInfo& mPassangerInfo = *message.getInfo();
    ASSERT(Asoup::MsgPassengerInfo::OperationType::AppearanceOnLdz==mPassangerInfo.operationType);
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"12-02-2019 15:26"), mPassangerInfo.time_to_str());
    ASSERT(EsrKit(12345)==mPassangerInfo.operationStationEsrCode);
    CPPUNIT_ASSERT_EQUAL(std::string("0,0"), mPassangerInfo.coord);
    ASSERT(1235==mPassangerInfo.trainNumber);
    
    CPPUNIT_ASSERT_MESSAGE("Локомотивов должно быть 2", 2u == mPassangerInfo.vSections.size());
    {
        const auto& section = mPassangerInfo.vSections[0];
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"123432"), section.loconumber);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"Иванов"), section.machinist);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"12-02-2019 00:00"), section.time_to_str());
        CPPUNIT_ASSERT_EQUAL(234, section.brutto);
        CPPUNIT_ASSERT_EQUAL(24, section.relLength);
        CPPUNIT_ASSERT_EQUAL(std::wstring(L"M"), section.notes);
        CPPUNIT_ASSERT_MESSAGE("Вагонов в первой секции должно быть 4", 4u == section.vCars.size());
        const auto& car = section.vCars[0];
        CPPUNIT_ASSERT_EQUAL(std::string("1231"), car.carNumber);
        CPPUNIT_ASSERT_EQUAL(4, car.axisCount);
        CPPUNIT_ASSERT_EQUAL(12, car.brutto);
        CPPUNIT_ASSERT_EQUAL(1.f, car.conditionLength);
        CPPUNIT_ASSERT_EQUAL(22, car.ownerCode);
    }
}

void TC_Asoup::tc_Parse1042_8()
{
    ExceptionTracer tracer;

    std::wstring msgE = 
        // 27.07.19 06:43:27 ASOUP Инкрементная hi:42 538 796     \n"
        L"(:904 0779 2954 0905 84 3100 1 27 07 06 43 112 02588 6 0000 0 0\n"
        L" 01 59040154 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0228 0095 0700\n"
        L" 02 59249813 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0223 0095 0700\n"
        L" 03 59251454 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0223 0095 0700\n"
        L" 04 90231275 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0228 0106 0710\n"
        L" 05 59252056 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0223 0095 0700\n"
        L" 06 58889254 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0238 0095 0700\n"
        L" 07 58888223 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0236 0095 0700\n"
        L" 08 58653544 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0225 0095 0700\n"
        L" 09 53822102 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0227 0095 0700\n"
        L" 10 59251918 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0223 0095 0700\n"
        L" 11 59037655 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0219 0095 0700\n"
        L" 12 59031179 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0225 0095 0700\n"
        L" 13 59252973 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0224 0095 0700\n"
        L" 14 59249706 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0223 0095 0700\n"
        L" 15 58898321 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0218 0095 0700\n"
        L" 16 59030924 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0228 0095 0700\n"
        L" 17 58898289 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0225 0095 0700\n"
        L" 18 53821641 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0226 0095 0700\n"
        L" 19 59037861 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0220 0095 0700\n"
        L" 20 58652926 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0224 0095 0700\n"
        L" 21 58898891 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0225 0095 0700\n"
        L" 22 59040634 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0225 0095 0700\n"
        L" 23 53373825 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0228 0106 0710\n"
        L" 24 58886755 0202 000 76947  43304 2843 5 0 0 0 00/00 00000 000 Э      0216 0095 0700\n"
        L" 25 59039651 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0227 0095 0700\n"
        L" 26 59038612 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0222 0095 0700\n"
        L" 27 53373759 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0228 0106 0710\n"
        L" 28 53363578 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0226 0106 0710\n"
        L" 29 58899733 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0225 0095 0700\n"
        L" 30 58892019 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0225 0095 0700\n"
        L" 31 53822300 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0227 0095 0700\n"
        L" 32 58891813 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0225 0095 0700\n"
        L" 33 58653239 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0224 0095 0700\n"
        L" 34 90294711 0202 000 76162  43304 9051 0 0 0 0 00/00 00000 000 Э      0234 0106 0765\n"
        L" 35 58888264 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0236 0095 0700\n"
        L" 36 59043752 0202 000 27365  43304 6987 0 0 0 0 00/00 00000 000 Э      0225 0095 0700\n"
        L" 37 58888777 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0236 0095 0700\n"
        L" 38 58889601 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0237 0095 0700\n"
        L" 39 59044636 0202 000 76950  43403 4953 4 0 0 0 00/00 00000 000 Э      0235 0095 0700\n"
        L" 40 59812487 0202 000 76950  43403 4953 4 0 0 0 00/00 00000 000 Э      0230 0095 0700\n"
        L" 41 59868463 0202 000 76950  43403 4953 4 0 0 0 00/00 00000 000 Э      0227 0100 0710\n"
        L" 42 59835512 0202 000 76950  43403 4953 4 0 0 0 00/00 00000 000 Э      0236 0095 0700\n"
        L" 43 59830950 0202 000 76950  43403 4953 4 0 0 0 00/00 00000 000 Э      0234 0095 0700\n"
        L" 44 90249699 0202 000 76950  43403 4953 4 0 0 0 00/00 00000 000 Э      0227 0106 0710\n"
        L" 45 53387007 0202 000 76950  43403 4953 4 0 0 0 00/00 00000 000 Э      0224 0106 0710\n"
        L" 46 53256392 0202 000 76950  43403 4953 4 0 0 0 00/00 00000 000 Э      0227 0106 0710\n"
        L" 47 59849331 0202 000 76950  43403 4953 4 0 0 0 00/00 00000 000 Э      0240 0100 0700\n"
        L" 48 58320698 0202 000 76950  43403 4953 4 0 0 0 00/00 00000 000 Э      0227 0106 0710\n"
        L" 49 53820874 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0226 0095 0700\n"
        L" 50 59265835 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0224 0095 0700\n"
        L" 51 58892258 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0225 0095 0700\n"
        L" 52 58650888 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0224 0095 0700\n"
        L" 53 90252149 0202 000 76162  43304 9051 0 0 0 0 00/00 00000 000 Э      0238 0106 0762\n"
        L" 54 58892894 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0224 0095 0700\n"
        L" 55 58888843 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0237 0095 0700\n"
        L" 56 53333977 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0225 0106 0710\n"
        L" 57 90249277 0202 000 76947  43304 2843 0 0 0 0 00/00 00000 000 Э      0235 0106 0765\n"
        L" 58 53820890 0202 000 76947  43304 2843 3 0 0 0 00/00 00000 000 Э      0226 0095 0700\n"
        L" 59 59054262 0202 000 27365  43619 6987 3 6 0 0 00/00 00000 000 ЭAK501 0225 0095 0700\n"
        L" 60 59230821 0202 000 27365  43619 6987 3 6 0 0 00/00 00000 000 ЭAK501 0224 0095 0700\n"
        L" 61 90247800 0202 000 76947  43619 2843 0 6 0 0 00/00 00000 000 ЭAK501 0238 0106 0762\n"
        L" 62 59239855 0202 000 27365  43619 6987 3 6 0 0 00/00 00000 000 ЭAK501 0225 0095 0700\n"
        L" 63 53821088 0202 000 27365  43619 6987 3 6 0 0 00/00 00000 000 ЭAK501 0226 0095 0700\n"
        L" 64 59251033 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0223 0095 0700\n"
        L" 65 58652371 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0220 0095 0700\n"
        L" 66 59233601 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0225 0095 0700\n"
        L" 67 59043968 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0225 0095 0700\n"
        L" 68 53357919 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0225 0106 0710\n"
        L" 69 59249953 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0223 0095 0700\n"
        L" 70 59038018 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0220 0095 0700\n"
        L" 71 59038752 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0220 0095 0700\n"
        L" 72 58651050 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0225 0095 0700\n"
        L" 73 58898883 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0225 0095 0700\n"
        L" 74 59037671 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0220 0095 0700\n"
        L" 75 58889403 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0236 0095 0700\n"
        L" 76 90252826 0202 000 76950  43403 4953 5 0 0 0 00/00 00000 000 Э      0226 0106 0710\n"
        L" 77 58320011 0202 000 76950  43403 4953 5 0 0 0 00/00 00000 000 Э      0227 0106 0710\n"
        L" 78 90242199 0202 000 76950  43403 4953 5 0 0 0 00/00 00000 000 Э      0229 0106 0710\n"
        L" 79 53241097 0202 000 76950  43403 4953 5 0 0 0 00/00 00000 000 Э      0227 0106 0710\n"
        L" 80 58866849 0202 000 76950  43403 4953 5 0 0 0 00/00 00000 000 Э      0235 0095 0700\n"
        L" 81 53246138 0202 000 76950  43403 4953 5 0 0 0 00/00 00000 000 Э      0227 0106 0710\n"
        L" 82 59817494 0202 000 76950  43403 4953 5 0 0 0 00/00 00000 000 Э      0238 0100 0700\n"
        L" 83 53366837 0202 000 76950  43403 4953 5 0 0 0 00/00 00000 000 Э      0226 0106 0710\n"
        L" 84 59868075 0202 000 76950  43403 4953 5 0 0 0 00/00 00000 000 Э      0227 0100 0710\n"
        L" 85 59057513 0202 000 76950  43403 4953 5 0 0 0 00/00 00000 000 Э      0235 0095 0700\n"
        L" 86 53243036 0202 000 76950  43403 4953 5 0 0 0 00/00 00000 000 Э      0227 0106 0710\n"
        L" 87 53368312 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0228 0106 0710\n"
        L" 88 53363271 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0225 0106 0710\n"
        L" 89 53370813 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0225 0106 0710\n"
        L" 90 53369583 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0228 0106 0710\n"
        L" 91 59042937 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0225 0095 0700\n"
        L" 92 59041632 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0224 0095 0700\n"
        L" 93 58892027 0202 000 76947  43307 2843 4 6 0 0 00/00 00000 000 ЭAK509 0225 0095 0700\n"
        L" 94 59050245 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0220 0095 0640\n"
        L" 95 53820635 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0227 0095 0700\n"
        L" 96 59047480 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0220 0095 0640\n"
        L" 97 59236273 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0224 0095 0700\n"
        L" 98 59043158 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0225 0095 0700\n"
        L" 99 59256156 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0223 0095 0700\n"
        L"100 90175993 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0227 0106 0710\n"
        L"101 90014390 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0225 0106 0710\n"
        L"102 58899576 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0225 0095 0700\n"
        L"103 58898511 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0224 0095 0700\n"
        L"104 58788258 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0232 0095 0700\n"
        L"105 90206210 0202 000 76947  43307 2843 0 6 0 0 00/00 00000 000 ЭAK509 0227 0106 0710\n"
        L"106 58651837 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0224 0095 0700\n"
        L"107 58889031 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0237 0095 0700\n"
        L"108 59236588 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0215 0095 0700\n"
        L"109 58888199 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0236 0095 0700\n"
        L"110 58788621 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0230 0095 0700\n"
        L"111 58891730 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0225 0095 0700\n"
        L"112 58653726 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0224 0095 0700\n"
        L"113 58888645 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0236 0095 0700\n"
        L"114 58788746 0202 000 76947  43307 2843 3 6 0 0 00/00 00000 000 ЭAK509 0230 0095 0700:)\n"
        L"\n";

    CPPUNIT_ASSERT( Asoup::Message::parse( msgE, m_receivedDate, Asoup::AsoupOperationMode::LDZ, false ) );
}

void TC_Asoup::tc_acceptAsoupEmptyCrew_6193()
{
    std::wstring asoup9006  = L"(:1042 909/000+09000 2533 0833 022 0983 01 11100 29 03 06 30 04/16 2 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 050 04896 03403 228 51727741 51729168 000 000 00 057 000.70 057 000 000\n"
        L"Ю3 530 00009591 1 00 00 0000 00000 0           \n"
        L"Ю3 530 00009592 9\n"
        L"Ю4 00000 1 01 057 000.70 057 000 \n"
        L"Ю4 09830 1 01 057 000.70 057 000 \n"
        L"Ю12 00 51727741 1 058 09830 71152 5906 460 01 00 00 00 00 0265 30 70 5700 04 086 09830 09820 00000 01 08330 18 08330 2467 026 0 0000 0      132 000 00000000\n"
        L"Ю12 00 51481760 255239255255 0267\n"
        L"Ю12 00 51729093 191255255255 062\n"
        L"Ю12 00 51726966 191239255255 0580265\n"
        L"Ю12 00 51729036 191239255255 0610269\n"
        L"Ю12 00 51729077 255239255255 0268\n"
        L"Ю12 00 51726867 191239255255 0590265\n"
        L"Ю12 00 51726891 255255255255 \n"
        L"Ю12 00 51727188 255255255255 \n"
        L"Ю12 00 51527869 255239255255 0261\n"
        L"Ю12 00 50119924 187239255191 0603600240025\n"
        L"Ю12 00 50352590 255255255255 \n"
        L"Ю12 00 57253841 255255255255 \n"
        L"Ю12 00 51729267 191239255191 0610268026\n"
        L"Ю12 00 51682060 191239255255 0590252\n"
        L"Ю12 00 51727469 191239255255 0580265\n"
        L"Ю12 00 51964674 191239255191 0600240025\n"
        L"Ю12 00 51257939 255255255255 \n"
        L"Ю12 00 57250268 255255255255 \n"
        L"Ю12 00 51865236 191255255255 059\n"
        L"Ю12 00 51861292 191255255255 060\n"
        L"Ю12 00 51727030 191239255191 0590265026\n"
        L"Ю12 00 51899771 191239255191 0600240025\n"
        L"Ю12 00 51729317 191239255191 0610267026\n"
        L"Ю12 00 51729218 255255255255 \n"
        L"Ю12 00 51727014 191239255255 0580265\n"
        L"Ю12 00 51726941 255255255255 \n"
        L"Ю12 00 51092393 187239255255 0615600268\n"
        L"Ю12 00 51682045 191239255255 0590252\n"
        L"Ю12 00 51528016 255239255255 0260\n"
        L"Ю12 00 51726974 255239255255 0265\n"
        L"Ю12 00 51727006 255255255255 \n"
        L"Ю12 00 51682136 191239255255 0600252\n"
        L"Ю12 00 51729242 191239255255 0620268\n"
        L"Ю12 00 51527471 191239255255 0610250\n"
        L"Ю12 00 51242592 191239255255 0600270\n"
        L"Ю12 00 51729119 191239255255 0620267\n"
        L"Ю12 00 51729135 255239255255 0268\n"
        L"Ю12 00 51417558 191239255255 0590284\n"
        L"Ю12 00 51729291 191239255255 0610267\n"
        L"Ю12 00 51729259 191239255255 0620268\n"
        L"Ю12 00 51727022 191239255255 0590265\n"
        L"Ю12 00 53984852 255239255255 0279\n"
        L"Ю12 00 51528065 191239255255 0600261\n"
        L"Ю12 00 51527240 191239255255 0610269\n"
        L"Ю12 00 51727287 191239255255 0590265\n"
        L"Ю12 00 51418036 191239255255 0580282\n"
        L"Ю12 00 51729010 187239255255 0614600271\n"
        L"Ю12 00 51727832 191239255255 0590265\n"
        L"Ю12 00 51727808 255255255255 \n"
        L"Ю12 00 51728160 255239255255 0260\n"
        L"Ю12 00 51726875 191239255255 0580265\n"
        L"Ю12 00 51727303 191255255255 059\n"
        L"Ю12 00 51528057 255239255255 0261\n"
        L"Ю12 00 51727550 255239255255 0265\n"
        L"Ю12 00 53984035 191239255255 0580281\n"
        L"Ю12 00 51729168 191239255255 0620267:)";

    boost::gregorian::date receivedDate(2021, 03, 29);
    std::shared_ptr<Asoup::Message> asoupMessagePtr = Asoup::Message::parse(asoup9006, receivedDate, Asoup::AsoupOperationMode::LDZ, true);

    CPPUNIT_ASSERT( asoupMessagePtr );
    std::shared_ptr<AsoupEvent> asoupEventPtr = AsoupEvent::Create(*asoupMessagePtr, nullptr, nullptr, nullptr, 
        BadgeE(L"Badge", EsrKit(9006)), Asoup::AsoupOperationMode::LDZ);

    Locomotive headLoco;
    CPPUNIT_ASSERT( asoupEventPtr->GetDescr() && asoupEventPtr->GetDescr()->get_head_locomotive( headLoco ) );
    CPPUNIT_ASSERT( !headLoco.exist_crew() );
    attic::a_document doc("Test");
    asoupEventPtr->Serialize(doc.document_element());
    auto node = doc.document_element().child("Locomotive");
    CPPUNIT_ASSERT( node && !node.child("Crew") );

}

void TC_Asoup::tc_StrangeAsoupCheck()
{
std::wstring asoupCheck  = L"(:1042 928/400+30610 2595 7600 097 0356 99 00000 09 12 12 47 01/01 3 0/00 00 0:\n"
L"Ю8 7600 097 0356 30610 99 09 12 12 47 2595 760008 097 035601:)";

boost::gregorian::date receivedDate(2021, 12, 9);
std::shared_ptr<Asoup::Message> asoupMessagePtr = Asoup::Message::parse(asoupCheck, receivedDate, Asoup::AsoupOperationMode::LDZ, true);

CPPUNIT_ASSERT( !asoupMessagePtr );
}