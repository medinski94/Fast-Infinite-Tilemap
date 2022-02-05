#ifndef PAGE_EXPORTER_H
#define PAGE_EXPORTER_H

#include <SFML/Graphics.hpp>

struct PageExporter {
	int operator()(const std::string& path, std::size_t size, const sf::Uint8 data[]) {
		sf::Texture texture;

		sf::Image image;
		image.create(size, size);

		for (int i = 0; i < image.getSize().x; i++) {
			for (int j = 0; j < image.getSize().y; j++) {
				sf::Uint8 byte = data[i + size * j];
				image.setPixel(i, j, sf::Color(byte, byte, byte, 255));
			}
		}

		if (!image.saveToFile(path))
			throw std::runtime_error("Failed to save " + path);

		return 0;
	}
};

#endif