#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

enum TextureFormat {
	R = GL_RED,
	RGB = GL_RGB,
	RGBA = GL_RGBA
};

class TextureAtlas {
	sf::Texture texture;
	sf::Vector2u size;
	sf::Vector2u tileSize;
	TextureFormat format;

	void update(const sf::FloatRect& bounds, const sf::Uint8 data[]) {
		glBindTexture(GL_TEXTURE_2D, this->texture.getNativeHandle());

		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			bounds.left,
			bounds.top,
			bounds.width,
			bounds.height,
			this->format,
			GL_UNSIGNED_BYTE,
			data
		);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
public:
	TextureAtlas(unsigned int size, unsigned int tileSize, const TextureFormat& format) {
		this->size = sf::Vector2u(size, size);
		this->tileSize = sf::Vector2u(tileSize, tileSize);
		this->format = format;

		this->texture.create(
			this->size.x * this->tileSize.x,
			this->size.y * this->tileSize.y
		);

		glBindTexture(GL_TEXTURE_2D, this->texture.getNativeHandle());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			this->format,
			this->texture.getSize().x,
			this->texture.getSize().y,
			0,
			this->format,
			GL_UNSIGNED_BYTE,
			0
		);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void update(unsigned int index, const sf::Uint8 data[]) {
		sf::Vector2u coords = this->indexToCoords(index);

		sf::FloatRect bounds(
			coords.x * this->tileSize.x,
			coords.y * this->tileSize.y,
			this->tileSize.x,
			this->tileSize.y
		);

		this->update(bounds, data);
	}

	void clear() {
		sf::FloatRect bounds(
			0,
			0,
			this->size.x * this->tileSize.x,
			this->size.y * this->tileSize.y
		);

		this->update(bounds, std::vector<sf::Uint8>(bounds.width * bounds.height, 0).data());

	}

	sf::Vector2u indexToCoords(unsigned int index) const {
		unsigned int x = index % this->size.x;
		unsigned int y = index / this->size.y;

		return sf::Vector2u(x, y);
	}

	const sf::Texture& getTexture() const {
		return this->texture;
	}

	const sf::Vector2u& getSize() const {
		return this->size;
	}

	const sf::Vector2u& getTileSize() const {
		return this->tileSize;
	}
};

#endif
