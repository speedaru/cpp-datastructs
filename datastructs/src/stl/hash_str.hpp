#pragma once
#include "hash.hpp"
#include "string.hpp"

namespace spd {
	template <typename CH>
	struct Hash<String<CH>> {
		size_t operator()(const String<CH>& str) {
			const CH* data = str.Data();
			size_t strlen = str.Size();

			LOG_D("hashging spd::String : len %llu, str: %.*s\n", strlen, strlen, data);
			return hash::FNV1a(data, strlen);
		}
	};
}
