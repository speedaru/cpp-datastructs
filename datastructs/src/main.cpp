#include <iostream>
#include <stl/stl_includes.h>
#include <unit_tests/vector_unit_tests.hpp>

int main() {
	logging::LoggerInit("logging.txt");

	spd::unit_test::Vector();

	logging::LoggerShutdown();
}