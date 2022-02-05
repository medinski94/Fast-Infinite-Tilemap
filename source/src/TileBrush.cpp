#include <TileBrush.h>

TileBrush::TileBrush(Tilemap* tilemap) {
	this->tilemap = tilemap;
}

std::list<sf::Vector2i> TileBrush::getNeighbors(const sf::Vector2i& coords) {
	std::list<sf::Vector2i> neighbors;

	neighbors.push_back(sf::Vector2i(coords.x - 1, coords.y));
	neighbors.push_back(sf::Vector2i(coords.x + 1, coords.y));
	neighbors.push_back(sf::Vector2i(coords.x, coords.y - 1));
	neighbors.push_back(sf::Vector2i(coords.x, coords.y + 1));

	neighbors.push_back(sf::Vector2i(coords.x - 1, coords.y - 1));
	neighbors.push_back(sf::Vector2i(coords.x - 1, coords.y + 1));
	neighbors.push_back(sf::Vector2i(coords.x + 1, coords.y + 1));
	neighbors.push_back(sf::Vector2i(coords.x + 1, coords.y - 1));

	return neighbors;
}


void TileBrush::balance(const std::list<sf::Vector2i>& source) {
	std::queue<sf::Vector2i> queue;
	std::unordered_set<int> visited;
	unsigned int variations = tilemap->getTileset().numVariations;

	for (const sf::Vector2i& cell : source) {
		queue.push(cell);
		visited.insert(this->tilemap->coordsToId(cell));
	}

	while (!queue.empty()) {
		sf::Vector2i current = queue.front();
		queue.pop();

		unsigned int currentValue = floor(this->tilemap->getData(current) / variations);

		std::list<sf::Vector2i> neighbors = this->getNeighbors(current);
		for (const sf::Vector2i& neighbor : neighbors) {
			unsigned int id = this->tilemap->coordsToId(neighbor);
			unsigned int neighborValue = floor(this->tilemap->getData(neighbor) / variations);

			if (!visited.insert(id).second)
				continue;

			if (abs(int(currentValue - neighborValue)) <= 1)
				continue;

			if (currentValue < neighborValue) {
				this->tilemap->setData(neighbor, (currentValue + 1) * variations + (rand() % 2));
			} else {
				this->tilemap->setData(neighbor, (currentValue - 1) * variations + (rand() % 2));
			}

			queue.push(neighbor);
		}
	}
}
	
void TileBrush::paint() {
	if (this->options.active) {
		sf::Vector2i global = this->tilemap->worldToGlobal(this->center);

		int top = global.y - this->options.brushSize;
		int bottom = global.y + this->options.brushSize;
		int left = global.x - this->options.brushSize;
		int right = global.x + this->options.brushSize;

		int variations = tilemap->getTileset().numVariations;

		try {
			std::list<sf::Vector2i> updatedCells;
			for (int i = left; i <= right; i++) {
				for (int j = top; j <= bottom; j++) {
					sf::Vector2i coords = sf::Vector2i(i, j);
					unsigned int current = this->tilemap->getData(coords) / variations;
						
					int distance = sqrt(pow(global.x - coords.x, 2) + pow(global.y - coords.y, 2));
					if (distance >= this->options.brushSize)
						continue;

					if (current == this->options.type)
						continue;

					this->tilemap->setData(coords, this->options.type * variations + (rand() % 2));
					updatedCells.push_back(coords);
				}
			}
				
			this->balance(updatedCells);
		} catch (std::exception e) {}
	}	
}

void TileBrush::setCenter(const sf::Vector2f& center) {
	this->center = center;

	sf::Vector2i global = this->tilemap->worldToGlobal(this->center);
	
	Context::getResourceManager().get<sf::Shader>("shaders/default").setUniform(
		"brush", sf::Vector3f(global.x, global.y, this->options.brushSize)
	);
}

void TileBrush::setOptions(const BrushOptions& options) {
	this->options = options;

	sf::Vector2i global = this->tilemap->worldToGlobal(this->center);
	
	Context::getResourceManager().get<sf::Shader>("shaders/default").setUniform("drawBrush", this->options.active);

	Context::getResourceManager().get<sf::Shader>("shaders/default").setUniform(
		"brush", sf::Vector3f(global.x, global.y, this->options.brushSize)
	);
}

const sf::Vector2f& TileBrush::getCenter() const {
	return this->center;
}