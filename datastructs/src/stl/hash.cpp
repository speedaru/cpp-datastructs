#include "hash.hpp"

size_t spd::hash::FNV1a(const void* data, size_t size) {
	size_t hash = 2166136261u;
	for (const uint8_t* it = (const uint8_t*)data; it < (const uint8_t*)data + size; it++) {
		hash ^= *it;
		hash *= 16777619u;
	}
	return hash;
}
