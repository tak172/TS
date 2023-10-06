#include "stdafx.h"
#include "TC_TopologyHelper.h"
#include "../helpful/TopologyHelper.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TC_TopologyHelper);

TC_TopologyHelper::TC_TopologyHelper(void)
{
}

TC_TopologyHelper::~TC_TopologyHelper(void)
{
}

void TC_TopologyHelper::Correct()
{
	const std::string data = "// This is a comment line\r\n"
		"		# This is comment too\n  "
		"	10000-10001\n"
		"10000-10001-10002\n"
		"10000-10001..10005-10006\n";

	std::istringstream stream(data);
	TopologyHelper table(stream);

	CPPUNIT_ASSERT(table.GetMistakes().empty());

	EsrKit result = table.GetAdjacentStation(10000, 10001, true);
	CPPUNIT_ASSERT(result.filled());
	CPPUNIT_ASSERT_EQUAL(StationEsr(10001), result.getTerm());

	result = table.GetAdjacentStation(10001, 10000, true);
	CPPUNIT_ASSERT(result.filled());
	CPPUNIT_ASSERT_EQUAL(StationEsr(10000), result.getTerm());

	result = table.GetAdjacentStation(10000, 10001, false);
	CPPUNIT_ASSERT_EQUAL(StationEsr(10000), result.getTerm());

	result = table.GetAdjacentStation(10000, 10002, true);
	CPPUNIT_ASSERT(result.filled());
	CPPUNIT_ASSERT_EQUAL(StationEsr(10001), result.getTerm());

	result = table.GetAdjacentStation(10000, 10002, false);
	CPPUNIT_ASSERT(result.filled());
	CPPUNIT_ASSERT_EQUAL(StationEsr(10001), result.getTerm());

	result = table.GetAdjacentStation(10000, 10006, true);
	CPPUNIT_ASSERT(result.filled());
	CPPUNIT_ASSERT_EQUAL(StationEsr(10001), result.getTerm());

	result = table.GetAdjacentStation(10000, 10006, false);
	CPPUNIT_ASSERT(result.filled());
	CPPUNIT_ASSERT_EQUAL(StationEsr(10005), result.getTerm());

	result = table.GetAdjacentStation(10006, 10000, true);
	CPPUNIT_ASSERT(result.filled());
	CPPUNIT_ASSERT_EQUAL(StationEsr(10005), result.getTerm());

	result = table.GetAdjacentStation(10006, 10000, false);
	CPPUNIT_ASSERT(result.filled());
	CPPUNIT_ASSERT_EQUAL(StationEsr(10001), result.getTerm());
}

void TC_TopologyHelper::Mistakes()
{
	{
		const std::string data = "10000..10001\n"
			"10000-10002..10003\n"
			"10001-10002..10003-10004";

		std::istringstream stream(data);
		TopologyHelper table(stream);

		CPPUNIT_ASSERT_EQUAL(2u, (unsigned)table.GetMistakes().size());
		CPPUNIT_ASSERT(table.GetAdjacentStation(10001, 10004, true).filled());
	}

	{
		const std::string data = "# Empty file, fill it with\n"
			"# ESRKIT-ESRKIT\n"
			"// ESRKIT-ESRKIT-ESRKIT\n"
			"# ESRKIT-ESRKIT..ESRKIT-ESRKIT\n"
			"	\n	";

		std::istringstream stream(data);
		TopologyHelper table(stream);

		CPPUNIT_ASSERT(table.GetMistakes().empty());
	}
}

void VerifyOne(const TopologyHelper& helper, unsigned source, unsigned to, bool adjToSource, StationEsr expected)
{
    EsrKit result = helper.GetAdjacentStation(source, to, adjToSource);
    if (expected > 0u)
    {
        CPPUNIT_ASSERT(result.filled());
        CPPUNIT_ASSERT_EQUAL(expected, result.getTerm());
    }
    else
    {
        CPPUNIT_ASSERT(result.empty());
    }
}

void Verify(const TopologyHelper& helper, unsigned source, unsigned expectedSource, unsigned expectedDest, unsigned to)
{
    VerifyOne(helper, source, to, true, StationEsr(expectedSource));
    VerifyOne(helper, source, to, false, StationEsr(expectedDest));
    VerifyOne(helper, to, source, true, StationEsr(expectedDest));
    VerifyOne(helper, to, source, false, StationEsr(expectedSource));
}

void NotExists(const TopologyHelper& helper, unsigned source, unsigned to)
{
    Verify(helper, source, 0, 0, to);
}

void TC_TopologyHelper::Multiple()
{
    const std::string data = "// This is a comment line\r\n"
        "		# This is comment too\n  "
        "	10000-10001\n"
        "10000-10001-10002\n"
        "10000-10001..10005-10006\n"
        "10000-10001..10006-10007\n"
        "10006-10005..11000-11001\n"
        "10006-10005..11001-11002\n"
        "11002-11001..10001-10000\n";

    std::istringstream stream(data);
    TopologyHelper table(stream);

    CPPUNIT_ASSERT(table.GetMistakes().empty());

    NotExists(table, 11002, 11001);
    NotExists(table, 10006, 10007);
    NotExists(table, 10006, 10006);

    Verify(table, 10006, 10005, 10001, 10000);
    Verify(table, 10000, 10001, 10005, 10006);
    Verify(table, 10000, 10001, 10000, 10001);
    Verify(table, 10000, 10001, 10001, 10002);
    Verify(table, 10006, 10005, 11001, 11002);
    Verify(table, 11002, 11001, 10001, 10000);
    Verify(table, 10006, 10005, 11000, 11001);
    Verify(table, 10000, 10001, 10006, 10007);
    Verify(table, 10000, 10001, 10000, 10001);
}

void TC_TopologyHelper::DirectStnExample()
{
    const std::string c_ticket3742 = 
        "// Запись схематическая, первая и последняя запись - станция отправления и станция назначения\n"
        "// станции через дефис - смежные\n"
        "// 10000-10001\n"
        "// 10000-10001-10002\n"
        "// 10000-10001..10010-10011\n"
        "\n"
        "11760-11420\n"
        "\n"
        "09000-09420..09360-11760\n"
        "09000-09420..09370-09360\n"
        "09000-09420..09380-09370\n"
        "09000-09420..09400-09380\n"
        "09000-09420..09410-09400\n"
        "09000-09420-09410\n"
        "09000-09420\n"
        "\n"
        "11000-11070..11042-16169\n"
        "11000-11451..11432-11431\n"
        "11000-11070..11041-11020\n"
        "11000-11070..11041-11042\n"
        "11000-11070..11052-11041\n"
        "11000-11070..11050-11052\n"
        "11000-11070..11051-11050\n"
        "11000-11070..11060-11051\n"
        "11000-11070..11062-11060\n"
        "11000-11070..11063-11062\n"
        "11000-11070-11063\n"
        "11000-11070\n"
        "\n"
        "09180-09170\n"
        "\n"
        "09170-12600..12520-12550\n"
        "\n"
        "09820-09813..09764-09180\n"
        "09820-09813..09772-09764\n"
        "09820-09813..09750-09772\n"
        "09813-09812..09764-09180\n"
        "09813-09812..09772-09764\n"
        "09813-09812..09750-09772\n"
        "09812-09810..09764-09180\n"
        "09812-09810..09772-09764\n"
        "09812-09810..09750-09772\n"
        "09810-09803..09764-09180\n"
        "09810-09803..09772-09764\n"
        "09810-09803..09750-09772\n"
        "09803-09801..09764-09180\n"
        "09803-09801..09772-09764\n"
        "09803-09801..09750-09772\n"
        "09801-09800..09764-09180\n"
        "09801-09800..09772-09764\n"
        "09801-09800..09750-09772\n"
        "09790-09780..09764-09180\n"
        "09790-09780..09772-09764\n"
        "09790-09780..09750-09772\n"
        "09780-09751..09764-09180\n"
        "09780-09751..09772-09764\n"
        "09780-09751..09750-09772\n"
        "09751-09750..09764-09180\n"
        "09751-09750..09772-09764\n"
        "09751-09750-09772\n"
        "\n"
        "09180-09181..09190-11420\n"
        "\n"
        "11310-11272..11010-11000\n"
        "11272-11330..11010-11000\n"
        "11330-11332..11010-11000\n"
        "11332-11340..11010-11000\n"
        "11340-11342..11010-11000\n"
        "11342-11343..11010-11000\n"
        "11343-11360..11010-11000\n"
        "11360-11363..11010-11000\n"
        "\n"
        "09000-09008..09100-09670\n"
        "09000-09008..09670-09676\n"
        "09000-09008..09676-09715\n"
        "09000-09008..09715-09730\n"
        "09000-09008..09730-09732\n"
        "09000-09008..09732-09736\n"
        "09000-09008..09772-09750\n"
        "\n"
        "09000-09008..09006-09500\n"
        "\n"
        "09000-09008..09006-09010\n"
        "\n"
        "11420-11412..11321-11310\n"
        "11420-11412..11311-11321\n"
        "11420-11412..11380-11311\n"
        "11420-11412..11381-11380\n"
        "11420-11412..11390-11381\n"
        "11420-11412..11391-11390\n"
        "11420-11412..11401-11391\n"
        "11420-11412-11401\n"
        "11420-11412\n"
        "11310-11321..11321-11311\n"
        "11310-11321..11310-11380\n"
        "11310-11321..11380-11381\n"
        "11310-11321..11391-11390\n"
        "11310-11321..11390-11391\n"
        "11310-11321..11391-11401\n"
        "11310-11321..11401-11412\n"
        "\n"
        "09104-09100..09008-09000\n"
        "09150-09104..09008-09000\n"
        "09160-09150..09008-09000\n"
        "09162-09160..09008-09000\n"
        "09180-09162..09008-09000\n"
        "\n"
        "11420-11419..11451-11000\n"
        "\n"
        "09180-09280\n"
        "\n"
        "09000-09008..09010-09100\n"
        "\n"
        "12550-12730\n"
        "\n"
        "09000-09008..11101-11100\n"
        "\n"
        "11000-11071..11460-11470\n"
        "\n"
        "11470-12780..12730-12550\n"
        "\n"
        "11240-11076..12730-12550\n"
        "\n"
        "09180-09280..09355-09860\n"
        "\n"
        "09860-09355..09340-09330\n"
        "09860-09355..09350-09340\n"
        "09280-09290..09355-09860\n"
        "\n"
        "11260-11265..11271-11310\n"
        "11250-11265..11271-11310\n"
        "\n"
        "09500-09041-09650\n"
        "\n"
        "09180-09162..09104-09100\n"
        "\n"
        "11310-11272..11273-11330\n"
        "\n"
        "11000-11071..11074-11460\n"
        "\n"
        "09100-09670\n"
        "\n"
        "09670-09100..09162-09180\n"
        "09680-09670..09162-09180\n"
        "09690-09680..09162-09180\n"
        "09700-09701..09162-09180\n"
        "09701-09680..09162-09180\n"
        "\n";

    std::istringstream stream(c_ticket3742);
    TopologyHelper table(stream);

    CPPUNIT_ASSERT(table.GetMistakes().empty());

    // "11310-11272..11010-11000\n"
    Verify(table, 11000, 11010, 11272, 11310);

    // "09180-09181..09190-11420\n"
    Verify(table, 9180, 9181, 9190, 11420);

    // "11000-11070..11042-16169\n"
    Verify(table, 11000, 11070, 11042, 16169);

    // "11000-11070..11041-11020\n"
    Verify(table, 11000, 11070, 11041, 11020);

    // "09180-09181..09190-11420\n"
    Verify(table, 9180, 9181, 9190, 11420);

    // "11310-11272..11010-11000\n"
    Verify(table, 11310, 11272, 11010, 11000);

    NotExists(table, 11000, 11074);
    NotExists(table, 11074, 11000);
}
