#ifndef CONTEXT_H
#define CONTEXT_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <Utils/Singleton.h>
#include <Utils/WorkerPool.h>
#include <Utils/ResourceManager.h>

class Context : Singleton<Context> {
	sf::RenderWindow window;
	sf::Time time;
	TaskScheduler taskScheduler;
	ResourceManager resourceManager;
public:
	Context() = default;

	static sf::RenderWindow& getWindow() {
		Context& context = Context::getInstance();
		return context.window;
	}

	static sf::Time& getTime() {
		Context& context = Context::getInstance();
		return context.time;
	}

	static ResourceManager& getResourceManager() {
		Context& context = Context::getInstance();
		return context.resourceManager;
	}

	static TaskScheduler& getTaskScheduler() {
		Context& context = Context::getInstance();
		return context.taskScheduler;
	}
};

#endif