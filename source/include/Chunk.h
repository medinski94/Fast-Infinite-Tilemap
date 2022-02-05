#ifndef CHUNK_H
#define CHUNK_H

#include <SFML/Graphics.hpp>

class Chunk {
	sf::Vertex vertices[4];
	sf::Uint8* data;

	sf::Vector2i coordinates;
	sf::Vector2f position;
	sf::FloatRect bounds;

	unsigned int size;
	unsigned int index;

	bool updated;
public:
	Chunk(unsigned int size, unsigned int index);

	Chunk(const Chunk& other);

	void init(const sf::Vector2i& coordinates, unsigned int tileSize, const sf::Uint8 data[]);

	void setData(const sf::Vector2u& coords, unsigned int value);

	const sf::Vector2i& getCoordinates() const;

	const sf::Vector2f& getPosition() const;
	
	const sf::FloatRect& getBounds() const;

	const sf::Vertex& getVertices() const;

	sf::Uint8 getData(const sf::Vector2u& coords) const;

	const sf::Uint8& getData() const;

	unsigned int getSize() const;

	unsigned int getIndex() const;

	bool isUpdated() const;

	~Chunk();
};

#endif