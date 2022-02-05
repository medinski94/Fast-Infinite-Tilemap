#ifndef TILE_BRUSH_H
#define TILE_BRUSH_H

#include <SFML/Graphics.hpp>
#include <list>
#include "Context.h"
#include "Tilemap.h"

struct BrushOptions {
	unsigned int type;
	unsigned int brushSize;
	bool active;
};

class TileBrush {
	Tilemap* tilemap;
	BrushOptions options;
	sf::Vector2f center;

	std::list<sf::Vector2i> getNeighbors(const sf::Vector2i& coords);

	void balance(const std::list<sf::Vector2i>& source);

public:
	TileBrush(Tilemap* tilemap);

	void paint();
	
	void setCenter(const sf::Vector2f& center);

	void setOptions(const BrushOptions& options);

	const sf::Vector2f& getCenter() const;
};

#endif