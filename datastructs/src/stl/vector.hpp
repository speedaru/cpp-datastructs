#pragma once
#include "iterator.hpp"
#include "stl_includes.h"


namespace spd {
	template <typename T>
	class Vector {
	public:
#pragma region constructors
		Vector();
		Vector(size_t capacity);
		~Vector();

		Vector(const Vector& other);
		Vector(Vector&& other) noexcept;
#pragma endregion


#pragma region memory_management
		bool Reserve(size_t newCapacity);
		bool Resize(size_t newSize);
		bool ShrinkToFit();
#pragma endregion


#pragma region data_manipulation
		template<typename Ref>
		T& Insert(size_t idx, Ref&& element); // copy/move insert

		template<typename... Args>
		T& Emplace(size_t idx, Args&&... args);

		template<typename Ref>
		T& PushBack(Ref&& element); // copy/move push back

		template<typename... Args>
		T& EmplaceBack(Args&&... args);

		bool RemoveAt(size_t idx);

		bool PopBack();

		void Clear();
#pragma endregion


#pragma region accesors
		T& Front() const;
		T& Back() const;
#pragma endregion


#pragma region getters
		inline size_t Size() const { return m_size; }

		inline size_t Capacity() const { return m_capacity; }

		inline T* Data() const { return m_data; }

		inline bool Empty() const { return !m_size; }
#pragma endregion

		auto begin() { return spd::iterator<T>(m_data); }
		auto end() { return spd::iterator<T>(m_data + m_size); }

		auto begin() const { return spd::const_iterator<T>(m_data); }
		auto end() const { return spd::const_iterator<T>(m_data + m_size); }

#pragma region operators
		T& operator[](int idx) const {
			assert(idx >= 0 && idx < m_size); // idx in bounds
			return m_data[idx];
		}
#pragma endregion

	private:
		constexpr static const size_t INITIAL_CAPACITY = 8ull;
		constexpr static const float GROWTH_FACTOR = 1.5f;

		bool Realloc(size_t newSize);
		bool Realloc();

		template<typename... Args>
		T& InsertImpl(size_t idx, Args&&... args);

		void GrowIfNeeded();

		void MoveElementsLeftIfNeeded(size_t start);
		void MoveElementsRightIfNeeded(size_t start);

		// calls destructor for each data
		void DestroyData(T* data, size_t size);

		bool IsAlias(const T& element) const { return m_data <= &element && &element < m_data + m_size; }

	protected:
		size_t m_size{};
		size_t m_capacity{};
		T* m_data{ nullptr };
	};
}


// -------------------- IMPLEMENTATION --------------------
#pragma region constructors

template<typename T>
inline spd::Vector<T>::Vector() {
	Realloc(INITIAL_CAPACITY);
	LOG_T("created vector at 0x%p, initial capacity: %llu\n", m_data, m_capacity);
}

template<typename T>
inline spd::Vector<T>::Vector(size_t capacity) {
	Realloc(capacity);
	LOG_T("created vector at 0x%p, initial capacity: %llu\n", m_data, m_capacity);
}

template<typename T>
inline spd::Vector<T>::~Vector() {
	DestroyData(m_data, m_size);
	LOG_D("destroyed vector data\n");

	if (m_data) {
		SPD_FREE(m_data);
	}
	LOG_D("freed vector\n");
}

template<typename T>
inline spd::Vector<T>::Vector(const Vector& other) {
	Realloc(static_cast<size_t>(other.m_size * GROWTH_FACTOR));

	while (m_size < other.m_size) {
		new (m_data + m_size) T(other.m_data[m_size]);
		m_size++;
	}

	LOG_D("copied %llu objects from vector at 0x%p into vector at 0x%p\n", m_size, other.m_data, m_data);
}

template<typename T>
inline spd::Vector<T>::Vector(Vector&& other) noexcept {
	m_size = other.m_size;
	other.m_size = 0ull;

	m_capacity = other.m_capacity;
	other.m_capacity = 0ull;

	m_data = other.m_data;
	other.m_data = nullptr;
}

#pragma endregion


#pragma region memory_management

template<typename T>
inline bool spd::Vector<T>::Reserve(size_t newCapacity) {
	if (newCapacity > m_capacity) {
		return Realloc(newCapacity);
	}
	else {
		LOG_W("trying to reserve smaller size than vector size (%llu to %llu)\n", m_capacity, newCapacity);
		return false;
	}
}

template<typename T>
inline bool spd::Vector<T>::Resize(size_t newSize) {
	bool res{ true };

	// case 1: new size is smaller than old size
	// delete other objects
	if (newSize < m_size) {
		while (m_size > newSize) {
			m_data[m_size--].~T();
		}
	}
	// case 2: new size is bigger than current capacity
	// if new size exceeds capacity, try to realloc, if realloc fails then return false
	else if (newSize > m_capacity && !(res = Realloc(newSize))) {
		return false;
	}
	
	// if new size bigger than current size, create default objects
	if (newSize > m_size) {
		while (m_size < newSize) {
			new (m_data + m_size++) T();
		}
	}

	LOG_D("resized vector to %llu\n", newSize);
	return res;
}

template<typename T>
inline bool spd::Vector<T>::ShrinkToFit() {
	return false;
}

#pragma endregion


#pragma region data_manipulation

template<typename T>
template<typename Ref>
inline T& spd::Vector<T>::Insert(size_t idx, Ref&& element) {
	// ensure not inserting past last element
	assert(idx <= m_size);

	T* res = nullptr;
	if (IsAlias(element)) { // create temp variable if alias, otherwise its gonna mess up stuff
		T temp(std::forward<Ref>(element));
		res = &InsertImpl(idx, std::forward<Ref>(temp));
	}
	else {
		res = &InsertImpl(idx, std::forward<Ref>(element));
	}

	if constexpr (!std::is_lvalue_reference_v<Ref>) {
		LOG_D("inserting element (move) into vector at idx %llu\n", idx);
	}
	else {
		LOG_D("inserting element (copy) into vector at idx %llu\n", idx);
	}
	return *res;
}

template<typename T>
template<typename Ref>
inline T& spd::Vector<T>::PushBack(Ref&& element) {
	GrowIfNeeded();

	T* slot = m_data + m_size++;
	new (slot) T(std::forward<Ref>(element));
	if constexpr (std::is_rvalue_reference_v<Ref>) {
		LOG_D("pushed back element (move)\n");
	}
	else {
		LOG_D("pushed back element (copy)\n");
	}

	return *slot;
}

template<typename T>
template<typename ...Args>
inline T& spd::Vector<T>::Emplace(size_t idx, Args&&... args) {
	assert(idx <= m_size);

	LOG_D("emplacing element at %llu\n", idx);
	return InsertImpl(idx, std::forward<Args>(args)...);
}

template<typename T>
template<typename ...Args>
inline T& spd::Vector<T>::EmplaceBack(Args&&... args) {
	GrowIfNeeded();

	T* slot = m_data + m_size;
	new (slot) T(std::forward<Args>(args)...);
	LOG_D("emplaced back element\n");

	m_size++;
	return *slot;
}

template<typename T>
inline bool spd::Vector<T>::RemoveAt(size_t idx) {
	if (idx >= m_size) {
		LOG_E("trying to remove element at %llu in vector but size is: %llu\n", idx, m_size);
		return false;
	}

	MoveElementsLeftIfNeeded(idx);
	LOG_D("shifted %llu elements left for remove at\n", m_size - idx);

	LOG_D("removed element at %llu from vector\n", idx);
	m_size--;

	return true;
}

template<typename T>
inline bool spd::Vector<T>::PopBack() {
	assert(m_size > 0); // can't pop back when vector has 0 elements
	return RemoveAt(m_size - 1);
}

template<typename T>
inline void spd::Vector<T>::Clear() {
	DestroyData(m_data, m_size);
	m_size = 0;
}

#pragma endregion


#pragma region accesors

template<typename T>
inline T& spd::Vector<T>::Front() const {
	assert(m_size > 0);
	return m_data[0];
}

template<typename T>
inline T& spd::Vector<T>::Back() const {
	assert(m_size > 0);
	return m_data[m_size - 1];
}

#pragma endregion


#pragma region private

template<typename T>
inline bool spd::Vector<T>::Realloc(size_t newCapacity) {
	// allocate new data
	T* newData = SPD_ALLOC(T, newCapacity);
	if (!newData) {
		return false;
	}

	if (m_data) {
		// copy old data if new size at least as big
		if (m_size) {
			// in case we're shrinking
			size_t copyCount = spd::min(m_size, newCapacity);

			// copy data into new buff
			for (size_t i = 0; i < copyCount; i++) {
				new (newData + i) T(std::move(m_data[i]));
			}

			DestroyData(m_data, m_size);
		}

		// free old data
		SPD_FREE(m_data);
	}

	m_data = newData;
	m_capacity = newCapacity;
	LOG_T("resized vector capacity to %llu\n", m_capacity);

	return true;
}

template<typename T>
inline bool spd::Vector<T>::Realloc() {
	return Realloc(static_cast<size_t>(m_capacity * GROWTH_FACTOR));
}

template<typename T>
template<typename ...Args>
inline T& spd::Vector<T>::InsertImpl(size_t idx, Args && ...args) {
	// ensure not inserting past last element
	assert(idx <= m_size);

	GrowIfNeeded();
	MoveElementsRightIfNeeded(idx);

	// construct in hole
	T* slot = m_data + idx;
	new (slot) T(std::forward<Args>(args)...);

	m_size++;
	return *slot;
}

template<typename T>
inline void spd::Vector<T>::GrowIfNeeded() {
	// if data full realloc
	if (m_size >= m_capacity) {
		Realloc();
	}
}

template<typename T>
inline void spd::Vector<T>::MoveElementsLeftIfNeeded(size_t start) {
	if (m_size == 0) {
		return;
	}

	// ensure not oob
	assert(start < m_size);

	// remove last element, nothing to shift
	if (start == m_size - 1) {
		m_data[m_size - 1].~T();
		return;
	}

	// shift data to left
	for (size_t i = start; i < m_size; i++) {
		// delete current element
		m_data[i].~T();

		// not last element
		if (i != m_size - 1) {
			// move next element back 1 index
			new (m_data + i) T(std::move_if_noexcept(m_data[i + 1]));
		}
	}
}

template<typename T>
inline void spd::Vector<T>::MoveElementsRightIfNeeded(size_t idx) {
	// nothing to shift
	if (idx == m_size) {
		return;
	}

	assert(idx < m_size); // ensure not oob
	assert(m_size + 1 <= m_capacity); // ensure enough space

	// move construct last element into new space
	new (m_data + m_size) T(std::move_if_noexcept(m_data[m_size - 1]));

	// shift backward
	for (size_t i = m_size - 1; i > idx; i--) {
		m_data[i] = std::move_if_noexcept(m_data[i - 1]);
	}

	// destory duplicated element at idx
	m_data[idx].~T();
}

template<typename T>
inline void spd::Vector<T>::DestroyData(T* data, size_t size) {
	// nothing to destroy
	if (!size) {
		LOG_W("trying to destroy data, but size is 0\n");
		return;
	}

	while (size) {
		data[--size].~T();
	}
	LOG_T("destroyed %llu objects at 0x%p\n", size, data);
}

#pragma endregion
