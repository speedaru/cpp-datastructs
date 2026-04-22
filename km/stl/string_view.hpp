#pragma once
#include "common.h"
#include "iterator.hpp"


namespace spd {
	template <typename CH>
	class StringView {
	public:
		StringView();
		StringView(const CH* buff, size_t length);
		StringView(const spd::const_iterator<CH>& start, const spd::const_iterator<CH>& end);
		StringView(const CH* cstr);

		// non null terminated cstr
		const CH* GetData() const;
		size_t GetLength() const;

		// dangling pointer to null terminated cstr
		const CH* c_str() const;

		CH operator[](int idx) const {
			return m_data[idx];
		}

		template <typename T>
		operator StringView<T>() {
			// new type is same size or divisible by len
			assert(m_length % sizeof(T) == 0);
			size_t newLen = m_length / sizeof(T);
			return StringView(reinterpret_cast<const T*>(m_data), newLen);
		}

	private:
		const CH* m_data;
		size_t m_length;
	};
}


// IMPL

template<typename CH>
inline spd::StringView<CH>::StringView() : m_data(nullptr), m_length(0ull) { }

template<typename CH>
inline spd::StringView<CH>::StringView(const CH* buff, size_t length) : m_data(buff), m_length(length) { }

template<typename CH>
inline spd::StringView<CH>::StringView(const spd::const_iterator<CH>& start, const spd::const_iterator<CH>& end) {
	assert(end >= start);
	size_t length = end - start;
	m_data = start;
	m_length = length;
}

template<typename CH>
inline spd::StringView<CH>::StringView(const CH* cstr) : m_data(cstr) {
	const CH* it = cstr;
	
	// basicly strlen
	while (*it++ != '\0') {
		++m_length;
	}
}


template<typename CH>
inline const CH* spd::StringView<CH>::GetData() const {
	return m_data;
}

template<typename CH>
inline size_t spd::StringView<CH>::GetLength() const {
	return m_length;
}

template<typename CH>
inline const CH* spd::StringView<CH>::c_str() const {
	CH* buff = SPD_ALLOC(CH, m_length + 1);
	memcpy((void*)buff, (void*)m_data, m_length * sizeof(CH));
	buff[m_length] = '\0'; // null terminator
	SPD_FREE(buff);

	// return dangling pointer to cstr
	return buff;
}
