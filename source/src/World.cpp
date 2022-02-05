#include <World.h>

World::World(unsigned int chunkSize, unsigned int cacheSize) :
	tilemap(chunkSize, cacheSize),
	brush(&tilemap),
	camera(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(Context::getWindow().getSize())), MAX_SCALE),
	pageManager(chunkSize + 1) {

	this->tilemap.onLoad([this](const sf::Vector2i& coords) {
		unsigned int id = this->tilemap.coordsToId(coords);
		unsigned int chunkSize = this->tilemap.getChunkSize();

		if (this->pageManager.exists(id)) {
			this->pageManager.load(id);
		} else {
			this->pageManager.generate(id, sf::IntRect(coords.x * chunkSize, coords.y * chunkSize, chunkSize + 1, chunkSize + 1));
		}
	});

	this->tilemap.onUnload([this](const sf::Vector2i& coords, bool updated, const sf::Uint8 data[]) {
		if (updated) {
			unsigned int id = this->tilemap.coordsToId(coords);
			unsigned int chunkSize = this->tilemap.getChunkSize();
			this->pageManager.save(id, data);
		}
	});

	this->pageManager.onLoadCompleted([this](unsigned int id, const sf::Uint8 data[]) {
		sf::Vector2i coords = this->tilemap.idToCoords(id);
		this->tilemap.addChunk(coords, data);
	});

	this->reset();
}

void World::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.setView(this->camera);
	target.draw(this->tilemap);
}

void World::update(const sf::Time& deltaTime) {
	Context::getResourceManager().get<sf::Shader>("shaders/default").setUniform(
		"time",
		Context::getTime().asSeconds()
	);

	this->tilemap.update(this->camera);

	float direction = this->lighting.enable ? -1.0f : 1.0f;
	float delta = direction * deltaTime.asSeconds() * this->lighting.daySpeed;

	float radian = (this->lighting.time + delta) * (2.0f * PI / DAY_SECONDS);
	float degree = fmod(radian * 180.0f / PI, 360.0f);

	if (degree > MIDNIGHT_ANGLE && degree < NOON_ANGLE) {
		this->lighting.time += delta;

		float timeOfDay = -cos(radian) / 2.0f + 0.5f;
		Context::getResourceManager().get<sf::Shader>("shaders/default").setUniform(
			"timeOfDay",
			timeOfDay
		);
	}
}

void World::reset() {
	srand(time(0) * rand());

	this->pageManager.setNoiseOptions({
		rand(),
		0.005,
		5,
		this->tilemap.getTileset().numTypes,
		this->tilemap.getTileset().numVariations
	});

	this->pageManager.clear();
	this->tilemap.reset();
}

Camera& World::getCamera() {
	return this->camera;
}

TileBrush& World::getBrush() {
	return this->brush;
}

void World::enableGrid(bool enable) {
	Context::getResourceManager().get<sf::Shader>("shaders/default").setUniform("drawGrid", enable);
}

void World::enableLighting(bool enable) {
	this->lighting.enable = enable;
}

WorldDebugInfo World::getDebugInfo() const {
	const unsigned int numChunks = this->tilemap.getNumChunks();
	const unsigned int numTiles = this->tilemap.getNumTiles();

	const sf::Vector2f world = this->brush.getCenter();
	const sf::Vector2i chunkCoords = this->tilemap.worldToChunk(world);
	const sf::Vector2i globalCoords = this->tilemap.worldToGlobal(world);
	const sf::Vector2u localCoords = this->tilemap.globalToLocal(globalCoords);

	return { numChunks, numTiles, world, chunkCoords, globalCoords, localCoords };
}

sf::Vector2f World::screenToWorld(const sf::Vector2i& position) const {
	return Context::getWindow().mapPixelToCoords(position, this->camera);
}