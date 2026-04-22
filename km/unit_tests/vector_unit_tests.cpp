#include "vector_unit_tests.hpp"
using namespace spd::unit_test;

#include "stl/type_traits.hpp"

static void TestReserve() {
	LOG_D("-------------------- RESERVE TEST --------------------\n");
	spd::unit_test::ResetConstructorsCount();
	size_t bytesAllocatedBeforeVec = spd::GetBytesAllocated();
	spd::Vector<TestClass> vec;

	vec.Reserve(12);
	SPD_ASSERT(vec.Capacity() == 12);
	SPD_ASSERT(spd::GetBytesAllocated() - bytesAllocatedBeforeVec == 12 * sizeof(TestClass));

	LOG_D("-------------------------------------------------------\n");
}

static void TestResize() {
	LOG_D("-------------------- RESIZE TEST --------------------\n");
	spd::unit_test::ResetConstructorsCount();
	spd::Vector<TestClass> vec;

	vec.Resize(12);
	SPD_ASSERT(vec.Size() == 12);

	LOG_D("-------------------------------------------------------\n");
}

static void TestPushback() {
	LOG_D("-------------------- PUSHBACK TEST --------------------\n");
	spd::unit_test::ResetConstructorsCount();
	spd::Vector<TestClass> vec(8);

	// copy pushback
	TestClass t1(1);
	vec.PushBack(t1);
	SPD_ASSERT(g_testCtor == 1);
	SPD_ASSERT(g_testCopyCtor == 1);
	SPD_ASSERT(vec.Size() == 1);
	LOG_D("passed copy pushback test\n");

	// move pushback
	TestClass t2(2);
	vec.PushBack(spd::move(t2));
	SPD_ASSERT(g_testCtor == 2);
	SPD_ASSERT(g_testCopyCtor == 1);
	SPD_ASSERT(g_testMoveCtor == 1);
	SPD_ASSERT(vec.Size() == 2);
	LOG_D("passed move pushback test\n");

	// ensure size grows
	size_t initialCapacity = vec.Capacity();
	for (uint8_t i = 3; i < 12; i++) {
		TestClass t(i);
		vec.PushBack(t);
	}
	SPD_ASSERT(vec.Capacity() > initialCapacity);

	// should have 0 copy/move assignments
	SPD_ASSERT(g_testCopyAssign == 0);
	SPD_ASSERT(g_testMoveAssign == 0);

	logging::LogOutputRaw("[PushBack] vec:\n");
	PrintVector(vec);
	
	LOG_D("-------------------------------------------------------\n");
}

static void TestEmplace() {
	LOG_D("-------------------- EMPLACE TEST --------------------\n");
	spd::unit_test::ResetConstructorsCount();
	spd::Vector<TestClass> vec(8);

	// emplace back
	vec.EmplaceBack((uint8_t)1);
	SPD_ASSERT(g_testCtor == 1);

	// emplace back with move ctor
	vec.EmplaceBack(TestClass((uint8_t)2));
	SPD_ASSERT(g_testCtor == 2);
	SPD_ASSERT(g_testMoveCtor == 1);

	// emplace in middle
	vec.Emplace(1, (uint8_t)3);
	SPD_ASSERT(g_testCtor == 3);
	SPD_ASSERT(g_testMoveCtor == 2); // should shift 1 element

	// emplace a bunch of stuff so vector grows
	size_t initialCapacity = vec.Capacity();
	for (uint8_t i = 12; i >= 4; i--) {
		vec.Emplace(0, i);
	}
	for (int i = 0; i < 9; i++) {
		TestClass& el = vec[i];
		SPD_ASSERT(el.m_data == i + 4);
	}
	SPD_ASSERT(vec.Size() == 12);
	SPD_ASSERT(vec.Capacity() > initialCapacity);

	SPD_ASSERT(g_testCopyAssign == 0);

	logging::LogOutputRaw("[Emplace] vec:\n");
	PrintVector(vec);
	
	LOG_D("-------------------------------------------------------\n");
}

static void TestRemove() {
	LOG_D("-------------------- REMOVE TEST --------------------\n");
	spd::unit_test::ResetConstructorsCount();
	spd::Vector<TestClass> vec;

	// fill with test data
	for (uint8_t i = 0; i < 10; i++) {
		vec.EmplaceBack(i);
	}
	SPD_ASSERT(vec.Size() == 10);

	logging::LogOutputRaw("[Remove] vec before:\n");
	PrintVector(vec);

	// start removing stuff
	// check if were moving next data and stuff
	for (int i = 1; i < 10; i++) {
		spd::unit_test::ResetConstructorsCount();
		vec.RemoveAt(1);
		SPD_ASSERT(vec.Size() == 9 - i + 1);
		SPD_ASSERT(g_testMoveCtor == 9 - i);
	}

	logging::LogOutputRaw("[Remove] vec after:\n");
	PrintVector(vec);

	LOG_D("-------------------------------------------------------\n");
}

static void TestPopBack() {
	LOG_D("-------------------- POP BACK TEST --------------------\n");
	spd::unit_test::ResetConstructorsCount();
	spd::Vector<TestClass> vec;

	for (uint8_t i = 0; i < 10; i++) {
		vec.EmplaceBack(i);
	}

	logging::LogOutputRaw("[PopBack] vec before:\n");
	PrintVector(vec);

	for (int i = 0; i < 10; i++) {
		vec.PopBack();
		SPD_ASSERT(vec.Size() == 10 - i - 1);
	}
	SPD_ASSERT(vec.Size() == 0);

	logging::LogOutputRaw("[PopBack] vec after:\n");
	PrintVector(vec);

	LOG_D("-------------------------------------------------------\n");
}


void spd::unit_test::PrintVector(const spd::Vector<TestClass>& vec) {
	UNREFERENCED_PARAMETER(vec);
	//for (auto it = vec.begin(); it != vec.end(); ++it) {
	//	if (it != vec.begin()) {
	//		logging::LogOutputRaw(", ");
	//	}
	//	std::cout << (int)it->m_data;
	//}
	//std::cout << std::endl;
}

void spd::unit_test::Vector() {
	TestReserve();
	TestResize();
	TestPushback();
	TestEmplace();
	TestRemove();
	TestPopBack();
}
