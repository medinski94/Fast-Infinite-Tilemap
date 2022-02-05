#ifndef FACTORY_H
#define FACTORY_H

#include <unordered_map>
#include <functional>
#include <stdexcept>

class Factory {
	template <typename R, typename ...Args>
	static inline std::unordered_map<std::type_index, std::function<R* (Args...)>> factories;
public:
	Factory() = default;
	Factory(const Factory& other) = delete;
	Factory(Factory&& other) = delete;
	Factory& operator=(const Factory& other) = delete;
	Factory& operator=(Factory&& other) = delete;

	template <typename T, typename ...Args, typename F>
	void bind(F&& f) {
		static_assert(!std::is_pointer<T>::value, "Template argument must be a non pointer type");

		std::type_index type = std::type_index(typeid(T));

		if (factories<T, Args...>.find(type) != factories<T, Args...>.end())
			throw std::runtime_error("Factory for type is already bound");

		factories<T, Args...>[type] = std::function<T* (Args...)>(f);
	}

	template <typename T, typename ...Args>
	void unbind() {
		static_assert(!std::is_pointer<T>::value, "Template argument must be a non pointer type");

		std::type_index type = std::type_index(typeid(T));

		if (factories<T, Args...>.find(type) == factories<T, Args...>.end())
			throw std::runtime_error("No factory for type bound");

		factories<T, Args...>.erase(type);
	}

	template <typename T = void, typename ...Args>
	T * get(Args ...args) {
		static_assert(!std::is_pointer<T>::value, "Template argument must be a non pointer type");

		std::type_index type = std::type_index(typeid(T));

		if (factories<T, Args...>.find(type) == factories<T, Args...>.end())
			throw std::runtime_error("No factory for type bound");

		std::function<T* (Args...)> factory = factories<T, Args...>[type];

		return factory(std::forward<Args>(args)...);
	}
};

#endif