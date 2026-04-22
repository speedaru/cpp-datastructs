#include "string_unit_tests.hpp"
#include "stl/string.hpp"
#include "stl/stl_includes.h"

#define PRINT_SEPARATOR logging::LogOutputRaw("--------------------------------------------------\n\n")

void spd::unit_test::String() {
	{
		// test basic constructor
		LOG_I("testing string constructor from c string\n");
		const char cstr[] = "my string";
		spd::String str(cstr);
		SPD_ASSERT(str.Size() == sizeof(cstr) - 1);
		LOG_D("spd::string : %s\n", str.Str());

		PRINT_SEPARATOR;

		// test concatenation
		LOG_I("testing concact\n");
		LOG_D("creating str2\n");
		const char cstr2[] = "new string";
		spd::String str2(cstr2);
		LOG_D("putting str2 after str\n");
		str.Concat(str2);
		LOG_D("spd::string : %s\n", str.Str());
		SPD_ASSERT(str.Size() == sizeof(cstr) - 1 + sizeof(cstr2) - 1);

		PRINT_SEPARATOR;

		// test operators
		{
			LOG_I("testing operators\n");
			LOG_I("testing +=\n");
			spd::String<char>& res = str += str2;
			SPD_ASSERT(&str == &res);
			LOG_D("str : %s\n", str.Str());

			LOG_I("testing +\n");
			size_t allocatedBefore = spd::GetBytesAllocated();
			spd::String res2 = str + str2;
			size_t allocatedAfter = spd::GetBytesAllocated();
			SPD_ASSERT(allocatedBefore == allocatedAfter - res2.Capacity());

			LOG_D("res2 : %s\n", res2.Str());
		}

		PRINT_SEPARATOR;
		LOG_D("cleanup :\n");
	}
}
