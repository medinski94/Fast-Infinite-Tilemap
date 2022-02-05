#ifndef TILESET_LOADER_H
#define TILESET_LOADER_H

#include <SFML/OpenGL.hpp>
#include <nlohmann/json.hpp>
#include <list>
#include <fstream>  
#include <Tilemap.h>

#define GL_TEXTURE_MAX_LEVEL 0x813D

struct TilesetLoader {
	nlohmann::json parseJSON(const std::string& path) {
		std::ifstream ifs(path);
		return nlohmann::json::parse(ifs);
	}

	Tileset* operator()(const std::string& path) {
		nlohmann::json json = this->parseJSON(path);

		unsigned int tileSize = json["tileSize"];
		unsigned int numTypes = json["numTypes"];
		unsigned int numVariations = json["numVariations"];
		std::list<std::string> paths = json["textures"];

		Tileset* tileset = new Tileset();
		tileset->tileSize = tileSize;
		tileset->numTypes = numTypes;
		tileset->numVariations = numVariations;
		tileset->numMips = paths.size();

		if (paths.size() > 1)
			tileset->padding = tileSize;
		
		glBindTexture(GL_TEXTURE_2D, tileset->texture.getNativeHandle());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, tileset->numMips - 1);

		unsigned int index = 0;
		for (const std::string& path : paths) {
			sf::Image image;
			if (!image.loadFromFile(path))
				throw std::invalid_argument("Failed to load tileset");

			glTexImage2D(
				GL_TEXTURE_2D,
				index,
				GL_RGBA,
				image.getSize().x,
				image.getSize().y,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				image.getPixelsPtr()
			);
	
			index++;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	
		return tileset;
	}
};

#endif