#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics.hpp>
#include <Context.h>

#define VIEWPORT_SCALE 1.0f

class Camera : public sf::View {
	sf::Vector2f position;
	float zoomLevel;
public:
	Camera(const sf::FloatRect& rectangle, float factor);

	void move(const sf::Vector2f& delta);

	void zoom(float delta);

	float getZoom() const;

	sf::FloatRect getViewport() const;
};

#endif