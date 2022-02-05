#ifndef DEBUG_INFO_H
#define DEBUG_INFO_H

#include <SFML/Graphics.hpp>
#include <Context.h>

#define VECTOR_FORMAT "(%d, %d)"
#define FIELD_FORMAT "%-30s"

class DebugInfo : public sf::Drawable {
	const std::string renderFPSFormat = "Render FPS: " FIELD_FORMAT;
	const std::string updateFPSFormat = "Update FPS: " FIELD_FORMAT;
	const std::string chunksFormat = "Chunks Rendered: " FIELD_FORMAT;
	const std::string tilesFormat = "Tiles Rendered: " FIELD_FORMAT;
	const std::string worldFormat = "World: " FIELD_FORMAT;
	const std::string chunkFormat = "Chunk: " FIELD_FORMAT;
	const std::string globalFormat = "Global: " FIELD_FORMAT;
	const std::string localFormat = "Local: " FIELD_FORMAT;

	sf::Text renderFPSText;
	sf::Text updateFPSText;

	sf::Text chunksText;
	sf::Text tilesText;

	sf::Text worldText;
	sf::Text chunkText;
	sf::Text globalText;
	sf::Text localText;

	template<typename ... Args>
	std::string format(const std::string& format, Args ... args) {
		int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
		if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
		auto size = static_cast<size_t>(size_s);
		auto buf = std::make_unique<char[]>(size);
		std::snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1);
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
	DebugInfo();

	void setRenderFPS(unsigned int value);

	void setUpdateFPS(unsigned int value);

	void setNumChunks(unsigned int value);

	void setNumTiles(unsigned int value);

	void setWorldCoords(const sf::Vector2f& value);

	void setChunkCoords(const sf::Vector2i& value);

	void setGlobalCoords(const sf::Vector2i& value);

	void setLocalCoords(const sf::Vector2u& value);
};

#endif