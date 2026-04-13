#pragma once

namespace spd {
	template <typename T>
	class iterator {
	public:
		using value_type = T;

		iterator(T* ptr) : m_ptr(ptr) {
		}

		T* Get() const {
			return m_ptr;
		}

		// operators
		T& operator*() const {
			return *m_ptr;
		}
		T* operator->() const {
			return m_ptr;
		}

		iterator operator+(int i) {
			return iterator(m_ptr + i);
		}

		iterator operator-(int i) {
			return iterator(m_ptr - i);
		}

		// ++it
		iterator& operator++() {
			++m_ptr;
			return *this;
		}

		// it++
		iterator operator++(int) {
			iterator tmp = *this;
			++(*this);
			return tmp;
		}

		bool operator==(const iterator& b) {
			return m_ptr == b.m_ptr;
		}

		bool operator!=(const iterator& b) {
			return m_ptr != b.m_ptr;
		}

		operator T*() const {
			return m_ptr;
		}

	protected:
		T* m_ptr;
	};

	template <typename T>
	using const_iterator = iterator<const std::remove_const_t<T>>;
}
