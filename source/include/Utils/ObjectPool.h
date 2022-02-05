#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include <stack>
#include <list>

template<class T>
class ObjectPool {
	std::stack<T*> pool;
	std::list<T> objects;
public:
	ObjectPool() = default;

	ObjectPool(std::size_t size, const T& value) {
		this->objects.assign(size, T(value));

		for (T& object : this->objects)
			this->pool.push(&object);
	}

	ObjectPool(const ObjectPool& other) = delete;
	ObjectPool(ObjectPool&& other) = delete;
	ObjectPool& operator=(const ObjectPool& other) = delete;
	ObjectPool& operator=(ObjectPool&& other) = delete;

	void add(const T& object) {
		this->objects.push_back(object);
		this->pool.push(&this->objects.back());
	}

	T* get() {
		T* object = this->pool.top();
		this->pool.pop();

		return object;
	}

	void free(T* object) {
		this->pool.push(object);
	}

	std::size_t getSize() const {
		return this->objects.size();
	}

	bool empty() const {
		return this->pool.empty();
	}

	~ObjectPool() = default;
};

#endif