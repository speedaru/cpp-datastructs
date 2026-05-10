#pragma once
#include <stl/stl_includes.h>

namespace spd {
	namespace unit_test {
		inline size_t g_testCtor = 0;		// default constructor count
		inline size_t g_testDtor = 0;		// destructor count
		inline size_t g_testCopyCtor = 0;	// copy constructor count
		inline size_t g_testCopyAssign = 0;	// copy assign count
		inline size_t g_testMoveCtor = 0;	// move constructor count
		inline size_t g_testMoveAssign = 0;	// move assign count

		struct TestClass {
			uint32_t m_data{};

			TestClass() : m_data(0) {
				LOG_D("default constructor\n");
				g_testCtor++;
			}
			TestClass(uint8_t data) : m_data(data) {
				LOG_D("default constructor\n");
				g_testCtor++;
			}

			~TestClass() {
				LOG_D("destructor\n");
				g_testDtor++;
			}

			// copy constructor
			TestClass(const TestClass& other) : m_data(other.m_data) {
				LOG_D("copy constructor\n");

				g_testCopyCtor++;
			}
			// copy assignment
			void operator=(const TestClass& other) {
				LOG_D("copy assignment\n");
				m_data = other.m_data;

				g_testCopyAssign++;
			}

			// move constructor
			TestClass(TestClass&& other) noexcept : m_data(other.m_data) {
				LOG_D("move constructor\n");

				g_testMoveCtor++;
			}
			// move assignment
			void operator=(TestClass&& other) noexcept {
				LOG_D("move assignment\n");
				m_data = other.m_data;
				other.m_data = 0;

				g_testMoveAssign++;
			}

			bool operator==(const TestClass& other) const {
				return m_data == other.m_data;
			}
		};


		inline void ResetConstructorsCount() {
			g_testCtor = 0;
			g_testDtor = 0;

			g_testCopyCtor = 0;
			g_testCopyAssign = 0;

			g_testMoveCtor = 0;
			g_testMoveAssign = 0;
		}
	}
}