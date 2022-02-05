#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics.hpp>
#include <Context.h>
#include <Game.h>

#define UPDATE_RATE 1.0f/60.0f

class Engine {
	sf::RenderWindow& window;
	Game* game;

	void handleEvent();

	void update(const sf::Time& deltaTime);

	void draw();
public:
	Engine(const sf::Vector2u& windowSize, const std::string& title, bool frameLimit);

	void setGame(Game* game);

	void run();
	
	~Engine();
};

#endif