#include <iostream>

#include <stl/stl_includes.h>
#include <unit_tests/vector_unit_tests.hpp>
#include <unit_tests/map_unit_tests.hpp>
#include <unit_tests/string_unit_tests.hpp>

int main() {
	logging::LoggerInit("logging.txt");

	//spd::unit_test::Vector();
	//spd::unit_test::UnorderedMap();
	spd::unit_test::String();

	logging::LoggerShutdown();
}
