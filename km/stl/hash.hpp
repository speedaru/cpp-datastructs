#pragma once
#include "stl_includes.h"
#include "types.hpp"
#include "iterator.hpp"

namespace spd {
    namespace hash {
        size_t FNV1a(const void* data, size_t size);
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