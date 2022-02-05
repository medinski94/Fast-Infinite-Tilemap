#ifndef PAGE_IMPORTER_H
#define PAGE_IMPORTER_H

#include <SFML/Graphics.hpp>

struct PageImporter {
	const sf::Uint8* operator()(const std::string& path) {
		sf::Image image;

		if (!image.loadFromFile(path))
			throw std::runtime_error("Failed to load " + path);

		sf::Uint8* data = new sf::Uint8[image.getSize().x * image.getSize().y];
		for (int i = 0; i < image.getSize().x; i++) {
			for (int j = 0; j < image.getSize().y; j++) {
				data[i + image.getSize().x * j] = image.getPixel(i, j).r;
			}
		}

		return data;
	}
};

#endif