#include "vector_unit_tests.hpp"
using namespace spd::unit_test;


static void TestReserve() {
	LOG_D("-------------------- RESERVE TEST --------------------\n");
	spd::unit_test::ResetConstructorsCount();
	size_t bytesAllocatedBeforeVec = spd::GetBytesAllocated();
	spd::Vector<TestClass> vec;

	vec.Reserve(12);
	assert(vec.Capacity() == 12);
	assert(spd::GetBytesAllocated() - bytesAllocatedBeforeVec == 12 * sizeof(TestClass));

	LOG_D("-------------------------------------------------------\n");
}

static void TestResize() {
	LOG_D("-------------------- RESIZE TEST --------------------\n");
	spd::unit_test::ResetConstructorsCount();
	spd::Vector<TestClass> vec;

	vec.Resize(12);
	assert(vec.Size() == 12);

	LOG_D("-------------------------------------------------------\n");
}

static void TestPushback() {
	LOG_D("-------------------- PUSHBACK TEST --------------------\n");
	spd::unit_test::ResetConstructorsCount();
	spd::Vector<TestClass> vec(8);

	// copy pushback
	TestClass t1(1);
	vec.PushBack(t1);
	assert(g_testCtor == 1);
	assert(g_testCopyCtor == 1);
	assert(vec.Size() == 1);
	LOG_D("passed copy pushback test\n");

	// move pushback
	TestClass t2(2);
	vec.PushBack(std::move(t2));
	assert(g_testCtor == 2);
	assert(g_testCopyCtor == 1);
	assert(g_testMoveCtor == 1);
	assert(vec.Size() == 2);
	LOG_D("passed move pushback test\n");

	// ensure size grows
	size_t initialCapacity = vec.Capacity();
	for (int i = 3; i < 12; i++) {
		TestClass t(i);
		vec.PushBack(t);
	}
	assert(vec.Capacity() > initialCapacity);

	// should have 0 copy/move assignments
	assert(g_testCopyAssign == 0);
	assert(g_testMoveAssign == 0);

	printf("[PushBack] vec:\n");
	PrintVector(vec);
	
	LOG_D("-------------------------------------------------------\n");
}

static void TestEmplace() {
	LOG_D("-------------------- EMPLACE TEST --------------------\n");
	spd::unit_test::ResetConstructorsCount();
	spd::Vector<TestClass> vec(8);

	// emplace back
	vec.EmplaceBack(1);
	assert(g_testCtor == 1);

	// emplace back with move ctor
	vec.EmplaceBack(TestClass(2));
	assert(g_testCtor == 2);
	assert(g_testMoveCtor == 1);

	// emplace in middle
	vec.Emplace(1, 3);
	assert(g_testCtor == 3);
	assert(g_testMoveCtor == 2); // should shift 1 element

	// emplace a bunch of stuff so vector grows
	size_t initialCapacity = vec.Capacity();
	for (int i = 12; i >= 4; i--) {
		vec.Emplace(0, i);
	}
	for (int i = 0; i < 9; i++) {
		TestClass& el = vec[i];
		assert(el.m_data == i + 4);
	}
	assert(vec.Size() == 12);
	assert(vec.Capacity() > initialCapacity);

	assert(g_testCopyAssign == 0);

	printf("[Emplace] vec:\n");
	PrintVector(vec);
	
	LOG_D("-------------------------------------------------------\n");
}

static void TestRemove() {
	LOG_D("-------------------- REMOVE TEST --------------------\n");
	spd::unit_test::ResetConstructorsCount();
	spd::Vector<TestClass> vec;

	// fill with test data
	for (int i = 0; i < 10; i++) {
		vec.EmplaceBack(i);
	}
	assert(vec.Size() == 10);

	printf("[Remove] vec before:\n");
	PrintVector(vec);

	// start removing stuff
	// check if were moving next data and stuff
	for (int i = 1; i < 10; i++) {
		spd::unit_test::ResetConstructorsCount();
		vec.RemoveAt(1);
		assert(vec.Size() == 9 - i + 1);
		assert(g_testMoveCtor == 9 - i);
	}

	printf("[Remove] vec after:\n");
	PrintVector(vec);

	LOG_D("-------------------------------------------------------\n");
}

static void TestPopBack() {
	LOG_D("-------------------- POP BACK TEST --------------------\n");
	spd::unit_test::ResetConstructorsCount();
	spd::Vector<TestClass> vec;

	for (int i = 0; i < 10; i++) {
		vec.EmplaceBack(i);
	}

	printf("[PopBack] vec before:\n");
	PrintVector(vec);

	for (int i = 0; i < 10; i++) {
		vec.PopBack();
		assert(vec.Size() == 10 - i - 1);
	}
	assert(vec.Size() == 0);

	printf("[PopBack] vec after:\n");
	PrintVector(vec);

	LOG_D("-------------------------------------------------------\n");
}


void spd::unit_test::PrintVector(const spd::Vector<TestClass>& vec) {
	for (auto it = vec.begin(); it != vec.end(); ++it) {
		if (it != vec.begin()) {
			std::cout << ", ";
		}
		std::cout << (int)it->m_data;
	}
	std::cout << std::endl;
}

void spd::unit_test::Vector() {
	TestReserve();
	TestResize();
	TestPushback();
	TestEmplace();
	TestRemove();
	TestPopBack();
}
