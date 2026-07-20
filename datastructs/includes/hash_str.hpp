#pragma once
#include "hash.hpp"
#include "string.hpp"

namespace spd {
	template <typename CH>
	struct Hash<String<CH>> {
		size_t operator()(const String<CH>& str) {
			const CH* data = str.Data();
			size_t len = str.Size();

			LOG_D("hashging spd::String : len %llu, str: %.*s\n", len, len, data);
			return hash::FNV1a(data, len);
		}
	};
}
