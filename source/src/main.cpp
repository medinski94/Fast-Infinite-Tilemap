#include <Engine.h>
#include <Game.h>

#define TITLE "Fast Infinite Tilemap "
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define CHUNK_SIZE 255
#define CACHE_SIZE 32
#define FRAME_LIMIT false

int main(int argc, char* argv[]) {
	Engine engine(sf::Vector2u(SCREEN_WIDTH, SCREEN_HEIGHT), TITLE, FRAME_LIMIT);
	engine.setGame(new Game({ CHUNK_SIZE, CACHE_SIZE }));
	engine.run();
}
