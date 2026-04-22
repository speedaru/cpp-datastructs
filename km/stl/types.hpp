#include "type_traits.hpp"
#include "stl_includes.h"

#ifdef STL_KM

using int8_t = char;
using int16_t = short;
using int32_t = int;
using int64_t = spd::conditional_t<
	sizeof(long int) == 8,
	long int,
	long long int>;

using uint8_t = unsigned char;
using uint16_t = unsigned short;
using uint32_t = unsigned int;
using uint64_t = spd::conditional_t<
	sizeof(long unsigned int) == 8,
	long unsigned int,
	long long unsigned int>;

#endif

using BYTE = uint8_t;
using WORD = uint16_t;
using DWORD = uint32_t;
using QWORD = uint64_t;