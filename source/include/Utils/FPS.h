#ifndef FPS_H
#define FPS_H

#include <SFML/Graphics.hpp>

class FPS {
	unsigned int frame;
	unsigned int fps;
	sf::Clock clock;
public:
	FPS() : frame(0), fps(0) {}

	void update() {
		if (this->clock.getElapsedTime().asSeconds() >= 1.f)
		{
			fps = frame;
			frame = 0;
			clock.restart();
		}

		++frame;
	}

	unsigned int getFPS() const { return this->fps; }
};

#endif