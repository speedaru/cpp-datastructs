#pragma once
#include "stl_includes.h"
#include "types.hpp"
#include "iterator.hpp"

namespace spd {
    namespace hash {
		size_t FNV1a(const void* data, size_t size) {
			size_t hash = 2166136261u;
			for (const uint8_t* it = (const uint8_t*)data; it < (const uint8_t*)data + size; it++) {
				hash ^= *it;
				hash *= 16777619u;
			}
			return hash;
		}
    }

    // default hash template
    template <typename T>
    struct Hash {
        size_t operator()(const T& key) const {
            return hash::FNV1a(&key, sizeof(T));
        }
    };

    // specialization for strings
    template <>
    struct Hash<const char*> {
        size_t operator()(const char* str) const {
            size_t strlen = 0;
            while (*str++) ++strlen;

            return hash::FNV1a(str, strlen);
        }
    };
}