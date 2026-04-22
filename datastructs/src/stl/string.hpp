#pragma once
#include "vector.hpp"

namespace spd {
	// null terminated string, null terminator not included in size
	template <typename CH>
	class String : public Vector<CH> {
	public:
#pragma region constructors
		String() : Vector<CH>() { }
		String(const CH* cstr);
		String(size_t capacity) : Vector<CH>(capacity) { }
#pragma endregion

#pragma region data_manipulation
		const CH* Str();

		void Concat(const String& other);
#pragma endregion

#pragma region data_access
#pragma endregion

#pragma region operators
		String& operator+=(const String& other);
		String operator+(const String& other);
#pragma endregion
	};
}


// -------------------- IMPLEMENTATION --------------------

#pragma region constructors

template <typename CH>
inline spd::String<CH>::String(const CH* cstr) : Vector<CH>(0) {
	if (!cstr) return;

	// calc string len
	size_t len = 0;
	while (cstr[len]) len++;

	// allocate +1 for null terminator
	this->Reserve(len + 1);

	// copy data
	for (size_t i = 0; i < len; i++) {
		this->m_data[i] = cstr[i];
	}

	this->m_size = len;
	this->m_data[len] = (CH)0; // add null terminator
}

#pragma endregion


#pragma region data_manipulation

template<typename CH>
inline const CH* spd::String<CH>::Str() {
	return this->m_data ? this->m_data : (const CH*)"";
}

template<typename CH>
inline void spd::String<CH>::Concat(const String& other) {
	size_t sizeRequired = this->m_size + other.m_size + sizeof(CH); // null terminator at end

	// if current capacity too small, resize
	if (this->m_capacity < sizeRequired) {
		this->Realloc(sizeRequired);
	}

	// add new data at end
	for (auto it = other.begin(); it != other.end(); it++) {
		this->m_data[this->m_size++] = *it;
	}

	// add null terminator
	this->m_data[this->m_size] = (CH)0;
}

#pragma endregion


#pragma region operators

template<typename CH>
inline spd::String<CH>& spd::String<CH>::operator+=(const String& other) {
	LOG_T("string += operator (returns REFRENCE)\n");
	Concat(other);
	return *this;
}

template<typename CH>
inline spd::String<CH> spd::String<CH>::operator+(const String& other) {
	LOG_T("string + operator (returns COPY)\n");

	// create new string with required size by this string and other string
	size_t requiredSize = this->Size() + other.Size() + sizeof(CH);
	spd::String<CH> res(requiredSize);

	// add this string, then other string
	res.Concat(*this);
	res.Concat(other);
	return res;
}

#pragma endregion
