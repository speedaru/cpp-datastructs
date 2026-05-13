#include "unit_tests/map_unit_tests.hpp"
#include "unit_tests/unit_tests.hpp"
#include "stl/unordered_map.hpp"
#include "stl/string.hpp"
#include "stl/hash_str.hpp"

using namespace spd::unit_test;

//void spd::unit_test::PrintVector(const spd::Vector<TestClass>& vec) {
//
//}

#define PRINT_SEPARATOR logging::LogOutputRaw("--------------------------------------------------\n")
#define PRINT_HEADER(header) LOG_I("----------" header "----------\n")

#define MAP_PROLOGUE(bucketCount, tag) \
	spd::UnorderedMap<K, V> map(bucketCount, tag); \
	ResetConstructorsCount(); \

using K = int;
using V = TestClass;

static void TestInsertion() {
    MAP_PROLOGUE(2, "main map");

	LOG_I("Testing Insertion...\n");
	V v1(5);
	V v2(7);
	map.Set(1, v1); // Key 1, Bucket 1
	map.Set(2, spd::move(v2)); // Key 2, Bucket 2
	SPD_ASSERT(map.Contains(1));
	SPD_ASSERT(map.Contains(2));
	SPD_ASSERT(*map.Get(1) == v1 && map[1] == v1);
	SPD_ASSERT(*map.Get(2) == v2 && map[2] == v2);
	SPD_ASSERT(g_testCopyCtor == 1);
	SPD_ASSERT(g_testMoveCtor == 1);
	LOG_I("passed insertion tests\n");
}

static void TestAssignmentOperators() {
    LOG_SCOPE();
    MAP_PROLOGUE(8, "assignment test map");

    // sample data
    map.Set(1, TestClass(5));
    map.Set(2, TestClass(6));
    map.Set(2, TestClass(7));

    SPD_ASSERT(map.Size() == 2);

    ResetConstructorsCount();
    PRINT_SEPARATOR;

    decltype(map) map2 = map;
    SPD_ASSERT(g_testCopyCtor == 2); // 2 copies bcs we copied the map
    SPD_ASSERT(map[1] == TestClass(5));
    SPD_ASSERT(map[2] == TestClass(7));

    ResetConstructorsCount();
    PRINT_SEPARATOR;

    decltype(map) map3 = spd::move(map);
    SPD_ASSERT(g_testCopyCtor + g_testMoveCtor == 0); // 0 moves, 0 copies bcs we just stole the buckets

    PRINT_SEPARATOR;
}

static void TestContains() {
    MAP_PROLOGUE(2, "main map");

	PRINT_HEADER("Testing Contains (key & val tests)");
    TestClass v1(5);
    TestClass v2(15);
    TestClass v3(25);
    int k1 = 1;
    int k2 = 3;
    int k3 = 7;

	map.Set(k1, v1);
	map.Set(k2, v2);
	map.Set(k3, v3);

	SPD_ASSERT(map.Contains(k1));
	SPD_ASSERT(map.Contains(k2));
	SPD_ASSERT(map.Contains(k3));

	SPD_ASSERT(map.ContainsVal(v1));
	SPD_ASSERT(map.ContainsVal(v2));
	SPD_ASSERT(map.ContainsVal(v3));

	SPD_ASSERT(*map.Get(k1) == v1 && map[k1] == v1);
	SPD_ASSERT(*map.Get(k2) == v2 && map[k2] == v2);
	SPD_ASSERT(*map.Get(k3) == v3 && map[k3] == v3);

	LOG_I("passed contains tests\n");
    PRINT_SEPARATOR;
}

void spd::unit_test::UnorderedMap() {
    LOG_I("-------------------- UNORDERED MAP TEST --------------------\n");

    TestAssignmentOperators();
    PRINT_SEPARATOR;

    TestContains();
    PRINT_SEPARATOR;

    //{
    //    // Initialize with a small bucket count to force collisions
    //    spd::UnorderedMap<K, V> map(2, "main map");


    //    PRINT_SEPARATOR;

    //    // 2. Test operator[] (Access and Default Insertion)
    //    LOG_I("Testing operator[]...\n");
    //    V& ref = map[1]; 
    //    map[5] = V(); // Key 5, Bucket 1 (Collision with Key 1!)

    //    PRINT_SEPARATOR;
    //    
    //    // 3. Test Update
    //    LOG_I("Testing Update...\n");
    //    map.Set(2, V()); // Should call destructor on old V and update

    //    PRINT_SEPARATOR;

    //    // 4. Test Get (Pointer access)
    //    LOG_I("Testing Get...\n");
    //    const V* valPtr = map.Get(5);
    //    SPD_ASSERT(valPtr != nullptr);
    //    SPD_ASSERT(map.Get(100) == nullptr); // Non-existent

    //    PRINT_SEPARATOR;

    //    // 5. Test Collision Handling & Deletion
    //    // Current Bucket 1: [5] -> [1] -> nullptr
    //    LOG_I("Testing Deletion (Collision middle/end)...\n");
    //    map.DeleteKey(1); 
    //    SPD_ASSERT(!map.Contains(1));
    //    SPD_ASSERT(map.Contains(5)); // Ensure the rest of the chain is intact

    //    PRINT_SEPARATOR;

    //    // 6. Test Clear
    //    LOG_I("Testing Clear...\n");
    //    map.Clear();
    //    SPD_ASSERT(!map.Contains(2));
    //    SPD_ASSERT(!map.Contains(5));

    //    PRINT_SEPARATOR;
    //} 

    //{
    //    using K = spd::String<char>;
    //    using V = TestClass;

    //    spd::UnorderedMap<K, V> map;

    //    spd::StringA str1("string1", "str1");
    //    spd::StringA str2("string1", "str2");

    //    size_t hash1 = Hash<K>()(str1);
    //    size_t hash2 = Hash<K>()(str2);

    //    LOG_D("hash for str1 : %.*s, 0x%llX\n", SPD_FMT_SV(str1.Str()), hash1);
    //    LOG_D("hash for str2 : %.*s, 0x%llX\n", SPD_FMT_SV(str2.Str()), hash2);
    //    SPD_ASSERT(hash1 == hash2);

    //    spd::String str3("string3", "str3");
    //    str2 = str3;
    //    hash2 = Hash<K>()(str2);
    //    LOG_D("new hash for str2 : %.*s, 0x%llX\n", SPD_FMT_SV(str1.Str()), hash2);
    //    SPD_ASSERT(hash1 != hash2);

    //    //TestClass t1(1);
    //    //TestClass t2(2);

    //    //map[str1] = t1;
    //    //map[str2] = t2;
    //    PRINT_SEPARATOR;
    //}

	//PRINT_SEPARATOR;
 //   LOG_I("Map destroyed. Check logs for balanced ALLOC/FREE and Destructor calls.\n");
}
