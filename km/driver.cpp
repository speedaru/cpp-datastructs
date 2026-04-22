#include "stl/stl_includes.h"

#include "unit_tests/map_unit_tests.hpp"
#include "unit_tests/string_unit_tests.hpp"

void DriverUnload(PDRIVER_OBJECT DriverObject);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING) {
	UNREFERENCED_PARAMETER(DriverObject);

	logging::LoggerInit("");

	spd::unit_test::UnorderedMap();
	spd::unit_test::String();

	logging::LoggerShutdown();

	DriverObject->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;
}

void DriverUnload(PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);

	LOG_I("Unloading driver, allocated bytes : %llu\n", spd::GetBytesAllocated());
}
