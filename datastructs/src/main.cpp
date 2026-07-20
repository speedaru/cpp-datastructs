#include <iostream>

#include <logging.h>
#include <alloc.hpp>
#include <unit_tests/vector_unit_tests.hpp>
#include <unit_tests/map_unit_tests.hpp>
#include <unit_tests/string_unit_tests.hpp>
using logging::LogLevel;

int main() {
	logging::LoggerInit("logging.txt", (LogLevel)0b11111); // all loglevels

	//spd::unit_test::Vector();
	spd::unit_test::UnorderedMap();
	//spd::unit_test::String();

	logging::LoggerShutdown();

	printf("bytes allocated at end: %llu\n", spd::GetBytesAllocated());
}
