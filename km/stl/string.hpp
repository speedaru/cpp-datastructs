#pragma once
#include "vector.hpp"
#include "string_view.hpp"

namespace spd {
	// non-null terminated string
	template <typename CH>
	class String : public Vector<CH> {
	public:
		static constexpr const char* DEFAULT_TAG = "Unnamed String";

#pragma region constructors
		//String(const char* tag = DEFAULT_TAG) : Vector<CH>(tag) { }
		String(size_t capacity) : Vector<CH>(capacity, DEFAULT_TAG) { }

		String(const String& other) : Vector<CH>::Vector(other) { }
		String(String&& other) noexcept : Vector<CH>::Vector(spd::move(other)) { }

		String& operator=(const String& other) {
			Vector<CH>::operator=(other);
			return *this;
		}

		String& operator=(String&& other) noexcept {
			Vector<CH>::operator=(spd::move(other));
			return *this;
		}

		String(const CH* cstr, const char* tag = DEFAULT_TAG);
		String(const spd::StringView<CH>& view, const char* tag = DEFAULT_TAG);
#pragma endregion

#pragma region data_manipulation
		void Concat(const String& other);
#pragma endregion

#pragma region data_access
		StringView<CH> Str() const;
#pragma endregion

#pragma region operators
		String& operator+=(const String& other);
		String operator+(const String& other);

		bool operator==(const String& other) const;
#pragma endregion
	ADD_CLASS_TAG
	};

	using StringA = typename String<char>;
}


// -------------------- IMPLEMENTATION --------------------

#pragma region constructors

template <typename CH>
inline spd::String<CH>::String(const CH* cstr, const char* tag) : Vector<CH>(0, tag) {
	LOG_SCOPE();
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
	LOG_OBJ_D("created string: %.*s from cstring, len %llu\n", this->m_size, this->m_data, this->m_size);
}

template<typename CH>
inline spd::String<CH>::String(const spd::StringView<CH>& view, const char* tag) : Vector<CH>(0, tag) {
	LOG_SCOPE();
	if (!view.GetLength()) return;

	this->Reserve(view.GetLength());

	// copy data
	for (size_t i = 0; i < view.GetLength(); i++) {
		this->m_data[i] = view[(int)i];
	}

	this->m_size = view.GetLength();
	LOG_OBJ_D("created string: %.*s from cstring, len %llu\n", this->m_size, this->m_data, this->m_size);
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
	LOG_OBJ_T("string += operator (returns REFRENCE)\n");
	Concat(other);
	return *this;
}

template<typename CH>
inline spd::String<CH> spd::String<CH>::operator+(const String& other) {
	LOG_OBJ_T("string + operator (returns COPY)\n");

	// create new string with required size by this string and other string
	size_t requiredSize = this->Size() + other.Size() + sizeof(CH);
	spd::String<CH> res(requiredSize);

	// add this string, then other string
	res.Concat(*this);
	res.Concat(other);
	return res;
}

template<typename CH>
inline bool spd::String<CH>::operator==(const String& other) const {
	if (!this->m_size || !other.m_size
		|| !this->m_capacity || !other.m_capacity
		|| !this->m_data || !other.m_data)
	{
		return false;
	}

	// first compare string lengths
	if (this->m_size != other.m_size) {
		return false;
	}

	// if same len, compare string buffers
	const CH* thisData = this->m_data;
	const CH* otherData = other.m_data;
	size_t len = this->m_size;
	for (size_t i = 0; i < len; i++) {
		if (thisData[i] != otherData[i]) {
			return false;
		}
	}

	// same len and same data
	return true;
}

#pragma endregion

