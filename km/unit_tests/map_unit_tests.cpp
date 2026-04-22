#include "unit_tests/map_unit_tests.hpp"
#include "unit_tests/unit_tests.hpp"
#include "stl/unordered_map.hpp"

using spd::unit_test::TestClass;

//void spd::unit_test::PrintVector(const spd::Vector<TestClass>& vec) {
//
//}

#define PRINT_SEPARATOR logging::LogOutputRaw("--------------------------------------------------\n\n")

struct MyStruct {
	size_t data1;
	const char* myStr;
};

template <>
struct ::spd::Hash<MyStruct> {
    size_t operator()(MyStruct data) {
        size_t hashes[] = {
            Hash<decltype(data.data1)>()(data.data1),
            Hash<decltype(data.myStr)>()(data.myStr)
        };

        return hash::FNV1a(hashes, sizeof(hashes));
    }
};

void spd::unit_test::UnorderedMap() {
	using K = int;
    using V = TestClass;

    using K2 = const char*;
    using V2 = TestClass;

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
        V& ref = map[1]; 
        UNREFERENCED_PARAMETER(ref);
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
        spd::UnorderedMap<K2, V2> map;

        LOG_I("testing map with string keys\n");
        map.Set("sn1", TestClass(5));
        map.Set("sn2", TestClass(32));
        SPD_ASSERT(map["sn1"].m_data == 5);
        SPD_ASSERT(map["sn2"].m_data == 32);

        LOG_D("map[sn1] : %hhu\n", map["sn1"].m_data);
        LOG_D("map[sn2] : %hhu\n", map["sn2"].m_data);
        LOG_D("map[sn3] : %hhu\n", map["sn3"].m_data);

        PRINT_SEPARATOR;
    }

    {
        spd::UnorderedMap<GUID, GUID> map;

        GUID og1 = { 0x11223344, 0x5566, 0x7788, { 0x99, 0xAA, 0xBB, 0xCC, 0xCC, 0xDD, 0xEE, 0xFF } };
        GUID spoofed1 = { 0xDEEADDBB, 0xEEEE, 0xFFFF, { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 } };

        map[og1] = spoofed1;

        MyStruct mystruct1{
            .data1 = 0x22,
            .myStr = "nigger"
        };
        MyStruct mystruct2{
            .data1 = 0x22,
            .myStr = "nigger"
        };
        size_t hashMystruct1 = Hash<MyStruct>()(mystruct1);
        size_t hashMystruct2 = Hash<MyStruct>()(mystruct2);
        LOG_D("hash mystruct1 : 0x%llX\n", hashMystruct1);
        LOG_D("hash mystruct2 : 0x%llX\n", hashMystruct2);
        SPD_ASSERT(hashMystruct1 == hashMystruct2);

        const GUID& g = map[og1];
        LOG_D("map[og1] : %lX-%hX-%hX-%02X%02X%02X%02X%02X%02X%02X%02X\n",
            g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
    }

    LOG_I("Map destroyed. Check logs for balanced ALLOC/FREE and Destructor calls.\n");
}
