#pragma once

#define STL_UM
//#define STL_KM

#ifdef STL_KM
	#ifndef _AMD64_
        #define _AMD64_ 
    #endif
	#define NOMINMAX

	#undef min
	#undef max

	#include <ntifs.h>
	
	#define SPD_ASSERT(cond) ASSERT(cond)
	#define SPD_POOL_TAG 'Spdl'
#else
	#include <cassert>
	#include <memory>

	#define SPD_ASSERT(cond) assert(cond)
#endif


