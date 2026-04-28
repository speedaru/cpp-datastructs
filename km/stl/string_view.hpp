#pragma once
#include "iterator.hpp"

// format string view
#define SPD_FMT_SV(str_view) str_view.GetLength(), str_view.GetData()

namespace spd {
	template <typename CH>
	class StringView {
	public:
#pragma region constructors
		StringView();
		StringView(const CH* buff, size_t length);
		StringView(const spd::const_iterator<CH>& start, const spd::const_iterator<CH>& end);
		StringView(const CH* cstr);
#pragma endregion

#pragma region api
		// dangling pointer to null terminated cstr
		const CH* c_str() const;
#pragma endregion

#pragma region getters
		// non null terminated cstr
		inline const CH* GetData() const { return m_data; }
		inline size_t GetLength() const { return m_length; }
#pragma endregion

#pragma region operators
		CH operator[](int idx) const { return m_data[idx]; }

		// cast operator
		template <typename T>
		operator StringView<T>() const;
#pragma endregion

	private:
		const CH* m_data;
		size_t m_length;
	};
}


// IMPL

#pragma region constructors

template<typename CH>
inline spd::StringView<CH>::StringView() : m_data(nullptr), m_length(0ull) { }

template<typename CH>
inline spd::StringView<CH>::StringView(const CH* buff, size_t length) : m_data(buff), m_length(length) { }

template<typename CH>
inline spd::StringView<CH>::StringView(const spd::const_iterator<CH>& start, const spd::const_iterator<CH>& end) {
	SPD_ASSERT(end >= start);
	size_t length = end - start;
	m_data = start;
	m_length = length;
}

template<typename CH>
inline spd::StringView<CH>::StringView(const CH* cstr) : m_data(cstr) {
	while (*cstr++) { // get strlen
		++m_length;
	}
}

#pragma endregion


#pragma region api

template<typename CH>
inline const CH* spd::StringView<CH>::c_str() const {
	CH* buff = SPD_ALLOC(CH, m_length + 1);
	memcpy((void*)buff, (void*)m_data, m_length * sizeof(CH));
	buff[m_length] = '\0'; // null terminator
	SPD_FREE(buff);

	// return dangling pointer to cstr
	return buff;
}

#pragma endregion


#pragma region operators

template<typename CH>
template <typename T>
inline spd::StringView<CH>::operator StringView<T>() const {
	// new type is same size or divisible by len
	assert(m_length % sizeof(T) == 0);
	size_t newLen = m_length / sizeof(T);
	return StringView(reinterpret_cast<const T*>(m_data), newLen);
}

#pragma endregion
