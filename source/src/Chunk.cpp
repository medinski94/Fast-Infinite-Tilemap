#include <Chunk.h>

Chunk::Chunk(unsigned int size, unsigned int index) {
	this->size = size;
	this->index = index;
	this->updated = false;

	this->coordinates = sf::Vector2i();
	this->position = sf::Vector2f();
	this->bounds = sf::FloatRect();

	this->vertices[0].texCoords = sf::Vector2f(0.0, 0.0);
	this->vertices[1].texCoords = sf::Vector2f(1.0, 0.0);
	this->vertices[2].texCoords = sf::Vector2f(1.0, 1.0);
	this->vertices[3].texCoords = sf::Vector2f(0.0, 1.0);

	for (int i = 0; i < 4; i++)
		vertices[i].position = sf::Vector2f();

	for (int i = 0; i < 4; i++)
		this->vertices[i].color = sf::Color(index, index, index, index);

	this->data = new sf::Uint8[(this->size + 1) * (this->size + 1)];
	for (int i = 0; i < (this->size + 1) * (this->size + 1); i++)
		this->data[i] = 0;
}

Chunk::Chunk(const Chunk& other) {
	this->size = other.size;
	this->position = other.position;
	this->coordinates = other.coordinates;
	this->bounds = other.bounds;
	this->index = other.index;
	this->updated = other.updated;

	this->data = new sf::Uint8[(this->size + 1) * (this->size + 1)];
	for (int i = 0; i < (this->size + 1) * (this->size + 1); i++)
		this->data[i] = other.data[i];

	for (int i = 0; i < 4; i++)
		this->vertices[i] = other.vertices[i];
}

void Chunk::init(const sf::Vector2i& coordinates, unsigned int tileSize, const sf::Uint8 data[]) {
	this->coordinates = coordinates;

	this->position = sf::Vector2f(
		this->coordinates.x * int(this->size * tileSize),
		this->coordinates.y * int(this->size * tileSize)
	);

	this->bounds = sf::FloatRect(
		this->position.x,
		this->position.y,
		(this->size * tileSize),
		(this->size * tileSize)
	);

	vertices[0].position = sf::Vector2f(
		this->bounds.left,
		this->bounds.top
	);

	vertices[1].position = sf::Vector2f(
		this->bounds.left + bounds.width,
		this->bounds.top
	);

	vertices[2].position = sf::Vector2f(
		this->bounds.left + bounds.width,
		this->bounds.top + bounds.height
	);

	vertices[3].position = sf::Vector2f(
		this->bounds.left,
		this->bounds.top + bounds.height
	);

	for (int i = 0; i < (this->size + 1) * (this->size + 1); i++)
		this->data[i] = data[i];

	this->updated = false;
}

void Chunk::setData(const sf::Vector2u& coords, unsigned int value) {
	this->data[coords.x + (this->size + 1) * coords.y] = value;
	this->updated = true;
}

const sf::Vector2i& Chunk::getCoordinates() const {
	return this->coordinates;
}

const sf::Vector2f& Chunk::getPosition() const {
	return this->position;
}

const sf::FloatRect& Chunk::getBounds() const {
	return this->bounds;
}

const sf::Vertex& Chunk::getVertices() const {
	return *this->vertices;
}

sf::Uint8 Chunk::getData(const sf::Vector2u& coords) const {
	return this->data[coords.x + (this->size + 1) * coords.y];
}

const sf::Uint8& Chunk::getData() const {
	return *this->data;
}

unsigned int Chunk::getSize() const {
	return this->size;
}

unsigned int Chunk::getIndex() const {
	return this->index;
}

bool Chunk::isUpdated() const {
	return this->updated;
}

Chunk::~Chunk() {
	delete this->data;
}