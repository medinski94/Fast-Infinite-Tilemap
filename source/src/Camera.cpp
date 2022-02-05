#include <Camera.h>

Camera::Camera(const sf::FloatRect& rectangle, float factor) : sf::View(rectangle) {
	this->zoomLevel = 1.0 / factor;

	Context::getResourceManager().get<sf::Shader>("shaders/default").setUniform(
		"viewport", sf::Vector3f(this->getCenter().x, this->getCenter().y, this->zoomLevel)
	);
	
	Context::getResourceManager().get<sf::Shader>("shaders/default").setUniform("bias", 1.5f);
}

void Camera::move(const sf::Vector2f& delta) {
	sf::View::move(delta);

	Context::getResourceManager().get<sf::Shader>("shaders/default").setUniform(
		"viewport", sf::Vector3f(this->getCenter().x, this->getCenter().y, this->zoomLevel)
	);
}

void Camera::zoom(float delta) {
	if (this->zoomLevel * delta < 1.0) {
		sf::View::zoom(delta);

		this->zoomLevel *= delta;
	
		Context::getResourceManager().get<sf::Shader>("shaders/default").setUniform(
			"viewport", sf::Vector3f(this->getCenter().x, this->getCenter().y, this->zoomLevel)
		);
	}
}

float Camera::getZoom() const {
	return this->zoomLevel;
}

sf::FloatRect Camera::getViewport() const {
	const sf::Vector2f topLeft = Context::getWindow().mapPixelToCoords(
		sf::Vector2i(View::getViewport().left, View::getViewport().top),
		*this
	);

	return sf::FloatRect(
		topLeft + (this->getSize() - this->getSize() * VIEWPORT_SCALE) / 2.0f,
		this->getSize() * VIEWPORT_SCALE
	);
}
