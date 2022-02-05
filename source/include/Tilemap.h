#ifndef TILEMAP_H
#define TILEMAP_H

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <unordered_set>
#include <cmath>
#include "Context.h"
#include <Utils/TextureAtlas.h>
#include <Utils/ObjectPool.h>
#include <Utils/LRUCache.h>
#include <Chunk.h>
#include <Camera.h>

struct Tileset {
	sf::Texture texture;
	unsigned int tileSize;
	unsigned int padding;
	unsigned int numTypes;
	unsigned int numVariations;
	unsigned int numMips;
};

class Tilemap : public sf::Drawable {
	ObjectPool<Chunk> chunkPool;
	LRUCache<unsigned int, Chunk*> chunkCache;
	Tileset * tileset;

	unsigned int chunkSize;

	class ChunkBatch : public sf::Drawable {
		TextureAtlas atlas;
		sf::VertexBuffer vertexBuffer;
		sf::Shader * shader;

		std::unordered_set<unsigned int> indices;
		unsigned int size;

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
			states.shader = this->shader;
			target.draw(this->vertexBuffer, states);
		}
	public:
		//TODO: atlas has problems if size is not power of 2
		ChunkBatch(unsigned int batchSize, unsigned int textureSize) :
			vertexBuffer(sf::VertexBuffer::Dynamic),
			atlas(batchSize, textureSize, TextureFormat::R) {

			this->size = batchSize;
			
			this->shader = &Context::getResourceManager().get<sf::Shader>("shaders/default");

			this->shader->setUniform("chunkAtlas", this->atlas.getTexture());
			
			this->vertexBuffer.setPrimitiveType(sf::Quads);
			this->vertexBuffer.create(this->size * 4);
		}

		void add(const Chunk& chunk) {
			if (!this->indices.insert(chunk.getIndex()).second)
				return;

			this->atlas.update(chunk.getIndex(), &chunk.getData());
			this->vertexBuffer.update(&chunk.getVertices(), 4, chunk.getIndex() * 4);
		}

		void update(const Chunk& chunk) {
			if (this->indices.find(chunk.getIndex()) == this->indices.end())
				return;

			this->atlas.update(chunk.getIndex(), &chunk.getData());
		}
		
		void remove(const Chunk& chunk) {
			if(!this->indices.erase(chunk.getIndex()))
				return;

			sf::Vertex vertices[4];
			this->vertexBuffer.update(vertices, 4, chunk.getIndex() * 4);
		}

		unsigned int getNumItems() const {
			return this->indices.size();
		}

		void clear() {
			this->vertexBuffer.create(0);
			this->vertexBuffer.create(this->size * 4);
			this->indices.clear();
			this->atlas.clear();
		}
	} chunkBatch;

	struct {
		std::queue<Chunk*> queue;
		std::unordered_set<unsigned int> set;
	} updatedChunks;

	std::function<void(const sf::Vector2i&)> onLoadCallback;
	std::function<void(const sf::Vector2i&, bool, const sf::Uint8[])> onUnloadCallback;

	std::vector<sf::Vector2i> getVisibleChunks(const sf::FloatRect& viewport);
	
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
	Tilemap(unsigned int chunkSize, unsigned int cacheSize);

	void addChunk(const sf::Vector2i& coords, const sf::Uint8 data[]);

	void removeChunk(const sf::Vector2i& coords);

	void update(const Camera& camera);

	void reset();

	void setData(const sf::Vector2i& coords, unsigned int value);
	
	sf::Vector2i globalToChunk(const sf::Vector2i& global) const;

	sf::Vector2u globalToLocal(const sf::Vector2i& global) const;

	sf::Vector2i worldToGlobal(const sf::Vector2f& world) const;

	sf::Vector2i worldToChunk(const sf::Vector2f& world) const;

	sf::Uint8 getData(const sf::Vector2i& global) const;

	const Tileset& getTileset() const;

	int64_t coordsToId(const sf::Vector2i& coords) const;

	sf::Vector2i idToCoords(int64_t id) const;

	unsigned int getNumChunks() const;

	unsigned int getNumTiles() const;

	unsigned int getChunkSize() const;

	void onLoad(std::function<void(const sf::Vector2i&)> callback);

	void onUnload(std::function<void(const sf::Vector2i&, bool, const sf::Uint8[])> callback);
};

#endif
