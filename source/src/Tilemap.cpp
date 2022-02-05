#include <Tilemap.h>

Tilemap::Tilemap(unsigned int chunkSize, unsigned int cacheSize) :
	chunkBatch(cacheSize, chunkSize + 1),
	chunkCache(cacheSize - 1) {

	this->chunkSize = chunkSize;

	this->tileset = &Context::getResourceManager().get<Tileset>("tilesets/default");

	for (int i = 0; i < cacheSize; i++)
		this->chunkPool.add(Chunk(chunkSize, i));

	this->chunkCache.onPop([this](unsigned int key, Chunk * value) {
		this->onUnloadCallback(value->getCoordinates(), value->isUpdated(), &value->getData());

		this->chunkBatch.remove(*value);
		this->chunkPool.free(value);
	});
	
	Context::getResourceManager().get<sf::Shader>("shaders/default").setUniform("tileAtlas", this->tileset->texture);
	Context::getResourceManager().get<sf::Shader>("shaders/default").setUniform("tileSize", float(this->tileset->tileSize + this->tileset->padding * 2));
	Context::getResourceManager().get<sf::Shader>("shaders/default").setUniform("chunkSize", float(chunkSize));
	Context::getResourceManager().get<sf::Shader>("shaders/default").setUniform("padding", float(this->tileset->padding));
}

void Tilemap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(this->chunkBatch);
}

//This can be done on the GPU via a 2 pass approach
// GPU: Determine visible chunks
// CPU: Load visible chunks
// GPU: Render chunks
std::vector<sf::Vector2i> Tilemap::getVisibleChunks(const sf::FloatRect& viewport) {
	int minX = floor(viewport.left / int(this->chunkSize * this->tileset->tileSize));
	int maxX = ceil((viewport.left + viewport.width) / int(this->chunkSize * this->tileset->tileSize));

	int minY = floor(viewport.top / int(this->chunkSize * this->tileset->tileSize));
	int maxY = ceil((viewport.top + viewport.height) / int(this->chunkSize * this->tileset->tileSize));

	std::vector<sf::Vector2i> displayList;
	for (int i = minX; i < maxX; i++)
		for (int j = minY; j < maxY; j++)
			displayList.push_back(sf::Vector2i(i, j));
	
	return displayList;
}

void Tilemap::addChunk(const sf::Vector2i& coords, const sf::Uint8 data[]) {
	Chunk* chunk = this->chunkPool.get();
	chunk->init(coords, this->tileset->tileSize, data);
	this->chunkCache.push(this->coordsToId(chunk->getCoordinates()), chunk);
}

void Tilemap::removeChunk(const sf::Vector2i& coords) {
	unsigned int id = this->coordsToId(coords);

	Chunk * chunk = this->chunkCache.get(id);

	this->onUnloadCallback(chunk->getCoordinates(), chunk->isUpdated(), & chunk->getData());

	this->chunkBatch.remove(*chunk);
	this->chunkCache.remove(id);
	this->chunkPool.free(chunk);
}

void Tilemap::update(const Camera& camera) {
	sf::Vector2f center = camera.getCenter();
	sf::FloatRect viewport = camera.getViewport();
	std::vector<sf::Vector2i> displayList = this->getVisibleChunks(viewport);
	
	const std::list<std::pair<unsigned int, Chunk*>>& items = this->chunkCache.getItems();
	for (const auto& item : items) {
		Chunk* chunk = item.second;
		if (!viewport.intersects(chunk->getBounds())) {
			this->chunkBatch.remove(*chunk);
		} else {
			this->chunkBatch.add(*chunk);
		}
	}

	std::vector<sf::Vector2i> loadList;
	for (const sf::Vector2i& coords : displayList) {
		unsigned int id = this->coordsToId(coords);
		if (!this->chunkCache.moveToFront(id))
			loadList.push_back(coords);
	}

	unsigned int loadCount = std::min(loadList.size(), this->chunkCache.getSize() - this->chunkBatch.getNumItems());
	if (loadCount > 0) {
		std::sort(loadList.begin(), loadList.end(), [center, this](const sf::Vector2i& a, const sf::Vector2i& b) {
			sf::Vector2i centerCoords = this->worldToChunk(center);

			int dist1 = sqrt(pow(centerCoords.x - a.x, 2) + pow(centerCoords.y - a.y, 2));
			int dist2 = sqrt(pow(centerCoords.x - b.x, 2) + pow(centerCoords.y - b.y, 2));

			return dist1 < dist2;
		});

		for (int i = 0; i < loadCount; i++)
			this->onLoadCallback(loadList[i]);
	}

	while (!this->updatedChunks.queue.empty()) {
		Chunk* chunk = this->updatedChunks.queue.front();
		this->updatedChunks.queue.pop();
		this->chunkBatch.update(*chunk);
		this->updatedChunks.set.erase(this->coordsToId(chunk->getCoordinates()));
	}
}

void Tilemap::reset() {
	std::list<std::pair<unsigned int, Chunk *>> items = this->chunkCache.getItems();
	this->chunkCache.clear();
	this->chunkBatch.clear();

	for (const auto& item : items)
		this->chunkPool.free(item.second);
}

void Tilemap::setData(const sf::Vector2i& global, unsigned int value) {
	sf::Vector2i chunkCoords = this->globalToChunk(global);
	sf::Vector2u local = this->globalToLocal(global);

	const sf::Vector2u offsets[4] = {
		sf::Vector2u(0, 0),
		sf::Vector2u(1, 0),
		sf::Vector2u(0, 1),
		sf::Vector2u(1, 1)
	};

	const std::function<bool(const sf::Vector2u&)> validators[4] = {
		[&](const sf::Vector2u& coords) {
			return coords.x >= 0 && coords.y <= this->chunkSize;
		},
		[&](const sf::Vector2u& coords) {
			return coords.x == 0;
		},
		[&](const sf::Vector2u& coords) {
			return coords.y == 0;
		},
		[&](const sf::Vector2u& coords) {
			return coords.x == 0 && coords.y == 0;
		}
	};

	for (int i = 0; i < 4; i++) {
		std::function<bool(const sf::Vector2u&)> validator = validators[i];
		sf::Vector2u offset = offsets[i] * this->chunkSize;
		unsigned int id = this->coordsToId(chunkCoords - sf::Vector2i(offsets[i]));
		
		if (!validator(local))
			continue;

		Chunk * chunk = this->chunkCache.get(id);

		chunk->setData(local + offset, value);
		
		if (this->updatedChunks.set.insert(id).second)
			this->updatedChunks.queue.push(chunk);
	}
}

sf::Vector2u Tilemap::globalToLocal(const sf::Vector2i& global) const {
	int x = global.x % int(this->chunkSize);
	int y = global.y % int(this->chunkSize);

	if (x < 0) x += this->chunkSize;
	if (y < 0) y += this->chunkSize;

	return sf::Vector2u(x, y);
}

sf::Vector2i Tilemap::globalToChunk(const sf::Vector2i& global) const {
	return sf::Vector2i(
		floor(float(global.x) / this->chunkSize),
		floor(float(global.y) / this->chunkSize)
	);
}

sf::Vector2i Tilemap::worldToGlobal(const sf::Vector2f& world) const {
	return sf::Vector2i(
		round(world.x / float(this->tileset->tileSize)),
		round(world.y / float(this->tileset->tileSize))
	);
}

sf::Vector2i Tilemap::worldToChunk(const sf::Vector2f& world) const {
	return sf::Vector2i(
		floor(world.x / float(this->chunkSize * this->tileset->tileSize)),
		floor(world.y / float(this->chunkSize * this->tileset->tileSize))
	);
}

int64_t Tilemap::coordsToId(const sf::Vector2i& coords) const {
	int64_t A = (coords.x >= 0 ? 2 * coords.x : -2 * coords.x - 1);
	int64_t B = (coords.y >= 0 ? 2 * coords.y : -2 * coords.y - 1);

	return (A + B) * (A + B + 1) / 2 + B;
}

sf::Vector2i Tilemap::idToCoords(int64_t id) const {
	int64_t w = floor((sqrt(8 * id + 1) - 1) / 2);
	int64_t t = (w * w + w) / 2;
	int64_t y = id - t;
	int64_t x = w - y;

	return sf::Vector2i(
		(x % 2 == 0 ? floor(x / 2) : floor(-x / 2 - 1)),
		(y % 2 == 0 ? floor(y / 2) : floor(-y / 2 - 1))
	);
}

unsigned int Tilemap::getNumChunks() const {
	return this->chunkBatch.getNumItems();
}

unsigned int Tilemap::getNumTiles() const {
	return this->chunkBatch.getNumItems() * this->chunkSize * this->chunkSize;
}

sf::Uint8 Tilemap::getData(const sf::Vector2i& global) const {
	sf::Vector2i chunkCoords = this->globalToChunk(global);
	sf::Vector2u local = this->globalToLocal(global);

	Chunk* chunk = this->chunkCache.get(this->coordsToId(chunkCoords));
	return chunk->getData(local);
}

unsigned int Tilemap::getChunkSize() const {
	return this->chunkSize;
}

const Tileset& Tilemap::getTileset() const {
	return *this->tileset;
}

void Tilemap::onLoad(std::function<void(const sf::Vector2i&)> callback) {
	this->onLoadCallback = callback;
}

void Tilemap::onUnload(std::function<void(const sf::Vector2i&, bool updated, const sf::Uint8 data[])> callback) {
	this->onUnloadCallback = callback;
}