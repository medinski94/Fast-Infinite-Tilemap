#ifndef SINGLETON_H
#define SINGLETON_H

template <class T>
class Singleton {
protected:
	Singleton() noexcept = default;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	virtual ~Singleton() = default;
public:
	static T& getInstance() {
		static T instance{};
		return instance;
	}
};

#endif