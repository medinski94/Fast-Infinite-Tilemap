#ifndef GAME_H
#define GAME_H
 
#include <SFML/Graphics.hpp>
#include <Context.h>
#include <World.h>
#include <UI.h>
#include <DebugInfo.h>
#include <Utils/FPS.h>
#include <TilesetLoader.h>

#define ZOOM_FACTOR 0.04f

struct GameOptions {
	unsigned int chunkSize;
	unsigned int cacheSize;
};

class Game {
	World * world;
	UI * ui;
	DebugInfo * debugInfo;
	GameOptions options;

	FPS renderFPS;
	FPS updateFPS;

	struct {
		sf::Vector2f origin = sf::Vector2f();
		bool rightClick = false;
		bool leftClick = false;
	} mouse;
public:
	Game(const GameOptions& options);

	void init();

	void handleEvent(const sf::Event& event);

	void update(const sf::Time& deltaTime);

	void draw(sf::RenderTarget& target);

	~Game();
};
#endif