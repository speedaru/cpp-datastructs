#include "unit_tests/map_unit_tests.hpp"
#include "unit_tests/unit_tests.hpp"
#include "stl/unordered_map.hpp"
#include "stl/string.hpp"
#include "stl/hash_str.hpp"

using spd::unit_test::TestClass;

//void spd::unit_test::PrintVector(const spd::Vector<TestClass>& vec) {
//
//}

#define PRINT_SEPARATOR logging::LogOutputRaw("--------------------------------------------------\n\n")

void spd::unit_test::UnorderedMap() {
	using K = int;
    using V = TestClass;

    LOG_I("-------------------- UNORDERED MAP TEST --------------------\n");

    {
        // Initialize with a small bucket count to force collisions
        spd::UnorderedMap<K, V> map(2); 

        // 1. Test Insertion
        LOG_I("Testing Insertion...\n");
        V v1(5);
        V v2(7);
        map.Set(1, v1); // Key 1, Bucket 1
        map.Set(2, v2); // Key 2, Bucket 2
        SPD_ASSERT(map.Contains(1));
        SPD_ASSERT(map.Contains(2));
        SPD_ASSERT(*map.Get(1) == v1 && map[1] == v1);
        SPD_ASSERT(*map.Get(2) == v2 && map[2] == v2);
        LOG_I("passed insertion tests\n");

        PRINT_SEPARATOR;

        // 2. Test operator[] (Access and Default Insertion)
        LOG_I("Testing operator[]...\n");
        LOG_D("nonexisting key (1), value : %hhu\n", map[1].m_data);
        map[5] = V(); // Key 5, Bucket 1 (Collision with Key 1!)

        PRINT_SEPARATOR;
        
        // 3. Test Update
        LOG_I("Testing Update...\n");
        map.Set(2, V()); // Should call destructor on old V and update

        PRINT_SEPARATOR;

        // 4. Test Get (Pointer access)
        LOG_I("Testing Get...\n");
        const V* valPtr = map.Get(5);
        SPD_ASSERT(valPtr != nullptr);
        SPD_ASSERT(map.Get(100) == nullptr); // Non-existent

        PRINT_SEPARATOR;

        // 5. Test Collision Handling & Deletion
        // Current Bucket 1: [5] -> [1] -> nullptr
        LOG_I("Testing Deletion (Collision middle/end)...\n");
        map.DeleteKey(1); 
        SPD_ASSERT(!map.Contains(1));
        SPD_ASSERT(map.Contains(5)); // Ensure the rest of the chain is intact

        PRINT_SEPARATOR;

        // 6. Test Clear
        LOG_I("Testing Clear...\n");
        map.Clear();
        SPD_ASSERT(!map.Contains(2));
        SPD_ASSERT(!map.Contains(5));

        PRINT_SEPARATOR;
    } 

    {
        using K2 = spd::String<char>;
        using V2 = TestClass;

        spd::UnorderedMap<K, V> map;

        spd::String str1("string1");
        spd::String str2("string1");

        size_t hash1 = Hash<K2>()(str1);
        size_t hash2 = Hash<K2>()(str2);

        LOG_D("hash for str1 : %.*s, 0x%llX\n", SPD_FMT_SV(str1.Str()), hash1);
        LOG_D("hash for str2 : %.*s, 0x%llX\n", SPD_FMT_SV(str2.Str()), hash2);
        SPD_ASSERT(hash1 == hash2);

        spd::String str3("string3");
        str2 = str3;
        hash2 = Hash<K2>()(str2);
        LOG_D("new hash for str2 : %.*s, 0x%llX\n", SPD_FMT_SV(str1.Str()), hash2);
        SPD_ASSERT(hash1 != hash2);

        //TestClass t1(1);
        //TestClass t2(2);

        //map[str1] = t1;
        //map[str2] = t2;
        PRINT_SEPARATOR;
    }

	PRINT_SEPARATOR;
    LOG_I("Map destroyed. Check logs for balanced ALLOC/FREE and Destructor calls.\n");
}
