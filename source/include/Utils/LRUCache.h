#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <list>
#include <functional>
#include <unordered_map>

template<typename K, typename T>
class LRUCache {
	typedef typename std::pair<K, T> pair;
	typedef typename std::list<pair>::iterator iterator;

	std::list<pair> items;
	std::unordered_map<K, iterator> map;
	std::size_t size;

	std::function<void(const K&, const T&)> onPushCallback;
	std::function<void(const K&, const T&)> onPopCallback;
	std::function<void(const K&, const T&)> onUpdateCallback;
public:
	LRUCache() = delete;

	LRUCache(std::size_t size) {
		this->size = size;
	}

	LRUCache(const LRUCache& other) = delete;
	LRUCache(LRUCache&& other) = delete;
	LRUCache& operator=(const LRUCache & other) = delete;
	LRUCache& operator=(LRUCache && other) = delete;

	void resize(std::size_t size) {
		this->size = size;
		while (this->items.size() > this->size)
			this->pop();
	}

	void push(const K& key, const T& value) {
		if (this->moveToFront(key))
			return;
		
		this->items.push_front(std::make_pair( key, value ));
		this->map[key] = this->items.begin();

		if(this->onPushCallback)
			this->onPushCallback(key, value);

		while (this->items.size() > this->size)
			this->pop();
	}
	
	void pop() {
		auto last = this->items.end();
		last--;

		const K key = last->first;
		const T value = last->second;
		
		this->map.erase(key);
		this->items.pop_back();

		if (this->onPopCallback) 
			this->onPopCallback(key, value);
	}

	void remove(const K& key) {
		auto it = this->map.find(key);

		if (it == this->map.end())
			return;

		this->map.erase(it->first);
		this->items.erase(it->second);
	}

	bool moveToFront(const K& key) {
		auto it = this->map.find(key);

		if (it == this->map.end())
			return false;

		this->items.splice(this->items.begin(), this->items, it->second);
		
		if (this->onUpdateCallback)
			this->onUpdateCallback(key, it->second->second);

		return true;
	}

	const T& get(const K& key) const {
		auto it = this->map.find(key);
		if (it == this->map.end())
			throw std::runtime_error("Key not found in cache");

		return it->second->second;
	}

	void clear() {
		this->items.clear();
		this->map.clear();
	}
	
	const std::list<pair>& getItems() const {
		return this->items;
	}

	bool exists(const K& key) const {
		return this->map.find(key) != this->map.end();
	}

	std::size_t getSize() const {
		return this->size;
	}

	void onPush(std::function<void(const K&, const T&)> callback) {
		this->onPushCallback = callback;
	}

	void onPop(std::function<void(const K&, const T&)> callback) {
		this->onPopCallback = callback;
	}

	void onUpdate(std::function<void(const K&, const T&)> callback) {
		this->onUpdateCallback = callback;
	}

	~LRUCache() = default;
};

#endif