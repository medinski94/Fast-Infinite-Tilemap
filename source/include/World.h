#ifndef WORLD_H
#define WORLD_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <Context.h>
#include <Shaders.h>
#include <Tilemap.h>
#include <TileBrush.h>
#include <Camera.h>
#include <UI.h>
#include <PageManager.h>

#define PI 3.14159265358979323846f
#define DAY_SECONDS 86400.0f
#define HOUR_SECONDS 3600.0f
#define MIDNIGHT_ANGLE 0.0f
#define NOON_ANGLE 180.0f
#define MAX_SCALE 16.0f

struct WorldDebugInfo {
	const unsigned int numChunks;
	const unsigned int numTiles;
	const sf::Vector2f worldCoords;
	const sf::Vector2i chunkCoords;
	const sf::Vector2i globalCoords;
	const sf::Vector2u localCoords;
};

class World : public sf::Drawable {
	Tilemap tilemap;
	Camera camera;
	TileBrush brush;
	PageManager pageManager;

	struct {
		const float daySpeed = HOUR_SECONDS * 4.0f;
		float time = DAY_SECONDS / 2.0f;
		float enable = false;
	} lighting;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
	World(unsigned int chunkSize, unsigned int cacheSize);

	void update(const sf::Time& deltaTime);

	void reset();

	void enableGrid(bool enable);

	void enableLighting(bool enable);

	Camera& getCamera();

	TileBrush& getBrush();

	WorldDebugInfo getDebugInfo() const;

	sf::Vector2f screenToWorld(const sf::Vector2i& position) const;
};

#endif