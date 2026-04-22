#pragma once
#include "vector.hpp"
#include "string_view.hpp"

namespace spd {
	// non-null terminated string
	template <typename CH>
	class String : public Vector<CH> {
	public:
#pragma region constructors
		String() : Vector<CH>() { }
		String(const CH* cstr);
		String(size_t capacity) : Vector<CH>(capacity) { }
#pragma endregion

#pragma region data_manipulation
		void Concat(const String& other);

		void CopyFrom(const String& other);
#pragma endregion

#pragma region data_access
		StringView<CH> Str() const;
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

	this->Reserve(len);

	// copy data
	for (size_t i = 0; i < len; i++) {
		this->m_data[i] = cstr[i];
	}

	this->m_size = len;
	LOG_D("created string from cstring, len %llu\n", this->m_size);
}

#pragma endregion


#pragma region data_manipulation

template<typename CH>
inline void spd::String<CH>::Concat(const String& other) {
	size_t sizeRequired = this->m_size + other.m_size;

	// if current capacity too small, resize
	if (this->m_capacity < sizeRequired) {
		this->Realloc(sizeRequired);
	}

	// add new data at end
	for (auto it = other.begin(); it != other.end(); it++) {
		this->m_data[this->m_size++] = *it;
	}
}

#pragma endregion


#pragma region data_access

template<typename CH>
inline spd::StringView<CH> spd::String<CH>::Str() const {
	return spd::StringView<CH>(this->m_data, this->m_size);
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
