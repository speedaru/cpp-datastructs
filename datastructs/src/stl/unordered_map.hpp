#pragma once
#include "hash.hpp"
#include "vector.hpp"
#include "new.hpp"

namespace spd {
	template <typename K, typename V>
	struct MapNode {
		K key;
		V value;
		size_t cachedHash;
		MapNode* next;

		MapNode(const K& k, const V& v, size_t hash)
			: key(k), value(v), cachedHash(hash), next(nullptr) {}

		~MapNode() {
			//key.~K();
			//value.~V();
		} 
	};

	template <typename K, typename V, typename HashFn = spd::Hash<K>>
	class UnorderedMap { 
	public:
		using Node = MapNode<K, V>;
		using Bucket = Node*; // linked list, ptr to first node

#pragma region constructors
		UnorderedMap(size_t bucketCount = 16);

		~UnorderedMap();
#pragma endregion // constructors


#pragma region api
		bool Contains(const K& key) const;

		const V* Get(const K& key) const;
		void Set(const K& key, const V& val);

		void DeleteKey(const K& key);

		void Clear();
#pragma endregion // api


#pragma region operator_overloading
		V& operator[](const K& key);
#pragma endregion // operator_overloading

	private:
#pragma region private_fn
		// get bucket index
		auto GetBucket(const K& key) const -> Bucket;

		void InsertNewNode(size_t bucketIdx, Node* newNode);

		void Rehash(size_t newBucketCount);
#pragma endregion // private_fn

	private:
		const float LOAD_FACTOR = 0.75f;
		const int GROWTH_FACTOR = 2;

		Bucket* m_buckets;
		size_t m_size{ 0 }; // num of elements
		size_t m_bucketCount;
	ADD_CLASS_TAG
	};
}


// ------------------------- IMPL

#pragma region constructors

template <typename K, typename V, typename HashFn>
inline spd::UnorderedMap<K, V, HashFn>::UnorderedMap(size_t bucketCount)
	: m_bucketCount(bucketCount)
{
	LOG_SCOPE();
	m_buckets = SPD_ALLOC(Bucket, m_bucketCount);
	for (size_t i = 0; i < m_bucketCount; i++) {
		m_buckets[i] = nullptr;
	}

	LOG_OBJ_T("created map with %llu buckets\n", m_bucketCount);
}

template<typename K, typename V, typename HashFn>
inline spd::UnorderedMap<K, V, HashFn>::~UnorderedMap() {
	LOG_SCOPE();
	Clear();
	SPD_FREE(m_buckets);

	LOG_OBJ_T("destroyed map (%llu buckets)\n", m_bucketCount);
}

#pragma endregion // constructors


#pragma region api

template<typename K, typename V, typename HashFn>
inline bool spd::UnorderedMap<K, V, HashFn>::Contains(const K& key) const {
	Node* current = GetBucket(key);
	while (current) {
		if (current->key == key) {
			return true;
		}
		current = current->next;
	}

	return false;
}

template<typename K, typename V, typename HashFn>
inline const V* spd::UnorderedMap<K, V, HashFn>::Get(const K& key) const {
	Node* current = GetBucket(key);
	while (current) {
		if (current->key == key) {
			return &current->value;
		}
		current = current->next;
	}

	// key doesn't exist
	return nullptr;
}

template<typename K, typename V, typename HashFn>
inline void spd::UnorderedMap<K, V, HashFn>::Set(const K& key, const V& val) {
	LOG_SCOPE();
	size_t hash = HashFn()(key);
	size_t bucketIdx = hash % m_bucketCount;

	// find and update existing
	Node* current = m_buckets[bucketIdx];
	while (current) {
		if (current->key == key) {
			current->value = val;
			return; // finished
		}
		current = current->next;
	}

	// key doesn't exist, create new node
	Node* newNode = SPD_ALLOC(Node, 1);
	::new (newNode) Node(key, val, hash);
	InsertNewNode(bucketIdx, newNode);

	LOG_OBJ_D("inserted new key into bucket %llu. map size: %llu\n", bucketIdx, m_size);
}

template<typename K, typename V, typename HashFn>
inline void spd::UnorderedMap<K, V, HashFn>::DeleteKey(const K& key) {
	// current points to the pointer that points to our node
    // this is either &m_buckets[idx] or &some_node->next
    Node** current = &m_buckets[HashFn()(key) % m_bucketCount];

    while (*current) {
        Node* entry = *current; // actual node

        if (entry->key == key) {
			// update actual pointer in list to the next one
            *current = entry->next;

            // cleanup
            entry->~Node();
            SPD_FREE(entry);
            m_size--; // track size
            return;
        }
        
        // advance current to point to the next pointer inside the current node
        current = &entry->next;
    }
}

template<typename K, typename V, typename HashFn>
inline void spd::UnorderedMap<K, V, HashFn>::Clear() {
	spd::iterator<Bucket> end(m_buckets + m_bucketCount);
	for (spd::iterator<Bucket> it(m_buckets); it != end; it++) {
		// delete linked list
		Node* current = *it;
		while (current) {
			Node* toDelete = current;
			current = current->next;

			// delete and free current link
			toDelete->~Node(); // call destructor
			SPD_FREE(toDelete);
		}

		// clear bucket pointer
		*it = nullptr;
	}

	m_size = 0;
}

#pragma endregion // api


#pragma region operator_overloading

template<typename K, typename V, typename HashFn>
inline V& spd::UnorderedMap<K, V, HashFn>::operator[](const K& key) {
	LOG_SCOPE();
	size_t hash = HashFn()(key);
	size_t bucketIdx = hash % m_bucketCount;

	Node* current = m_buckets[bucketIdx];
	while (current) {
		if (current->key == key) {
			return current->value;
		}
		current = current->next;
	}

	// key doesn't exist, create new default entry
	Node* newNode = SPD_ALLOC(Node, 1);
	new (newNode) Node(key, V(), hash);
	InsertNewNode(bucketIdx, newNode);

	LOG_OBJ_D("key not found, created a new default node : 0x%p\n", newNode);
	return newNode->value;
}

#pragma endregion


#pragma region private_fn

template<typename K, typename V, typename HashFn>
inline auto spd::UnorderedMap<K, V, HashFn>::GetBucket(const K& key) const -> Bucket {
	size_t hash = HashFn()(key);
	return m_buckets[hash % m_bucketCount];
}

template<typename K, typename V, typename HashFn>
inline void spd::UnorderedMap<K, V, HashFn>::InsertNewNode(size_t bucketIdx, Node* newNode) {
	newNode->next = m_buckets[bucketIdx];
	m_buckets[bucketIdx] = newNode;
	m_size++;

	if ((float)m_size / (float)m_bucketCount > LOAD_FACTOR) {
		Rehash(m_size * GROWTH_FACTOR);
	}
}

template<typename K, typename V, typename HashFn>
inline void spd::UnorderedMap<K, V, HashFn>::Rehash(size_t newBucketCount) {
	LOG_I("rehashing map %llu to %llu buckets\n", m_bucketCount, newBucketCount);

	Bucket* newBuckets = SPD_ALLOC(Bucket, newBucketCount);
	for (size_t i = 0; i < newBucketCount; i++) {
		newBuckets[i] = nullptr;
	}

	// ensure size is preserved
	size_t newSize = 0;

	// iterate over each linked list
	for (size_t i = 0; i < m_bucketCount; i++) {
		Node* current = m_buckets[i]; // iterate over every list head and move linked list into new buckets
		while (current) {
			// save next node
			Node* next = current->next;

			// recalculate new index
			size_t newIdx = current->cachedHash % newBucketCount;

			// move current node into new buckets
			current->next = newBuckets[newIdx];
			newBuckets[newIdx] = current;
			newSize++;

			// go to next node in original buckets
			current = next;
		}
	}

	SPD_ASSERT(m_size == newSize);

	SPD_FREE(m_buckets);
	m_buckets = newBuckets;
	m_bucketCount = newBucketCount;
}

#pragma endregion
