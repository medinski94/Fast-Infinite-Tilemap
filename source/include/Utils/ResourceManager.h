#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <unordered_map>
#include <typeindex>
#include <stdexcept>
#include <Context.h>
#include <Utils/Factory.h>

class ResourceManager {
	Factory factory;
	std::unordered_map<std::string, void*> cache;
public:
	ResourceManager() = default;
	ResourceManager(const ResourceManager& other) = delete;
	ResourceManager(ResourceManager&& other) = delete;
	ResourceManager& operator=(const ResourceManager& other) = delete;
	ResourceManager& operator=(ResourceManager&& other) = delete;

	template <typename T = void, typename ...Args>
	void load(const std::string& key, Args ...args) {
		if (this->cache.find(key) != this->cache.end())
			throw std::runtime_error("Resource already exists");

		this->cache[key] = this->factory.get<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	T& get(const std::string& key) {
		if (this->cache.find(key) == this->cache.end())
			throw std::runtime_error("Resource does not exist");

		return *static_cast<T*>(this->cache[key]);
	}

	void remove(const std::string& key) {
		if (this->cache.find(key) == this->cache.end())
			throw std::runtime_error("Resource does not exist");

		delete this->cache[key];
		this->cache.erase(key);
	}

	Factory& getFactory() {
		return this->factory;
	}

	~ResourceManager() {
		for (const auto& pair : this->cache)
			delete pair.second;
	}
};
#endif