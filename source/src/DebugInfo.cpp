#include <DebugInfo.h>

void DebugInfo::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.setView(Context::getWindow().getDefaultView());
	target.draw(this->renderFPSText);
	target.draw(this->updateFPSText);
	target.draw(this->chunksText);
	target.draw(this->tilesText);
	target.draw(this->worldText);
	target.draw(this->chunkText);
	target.draw(this->globalText);
	target.draw(this->localText);
}

DebugInfo::DebugInfo() {
	sf::RenderWindow& window = Context::getWindow();

	this->renderFPSText.setFont(Context::getResourceManager().get<sf::Font>("fonts/arial"));
	this->renderFPSText.setCharacterSize(16);
	this->renderFPSText.setFillColor(sf::Color::White);
	this->renderFPSText.setString(this->format(this->renderFPSFormat, ""));

	this->renderFPSText.setPosition(
		sf::Vector2f(
			window.getSize().x - this->renderFPSText.getLocalBounds().width, 0
		)
	);

	this->updateFPSText.setFont(Context::getResourceManager().get<sf::Font>("fonts/arial"));
	this->updateFPSText.setCharacterSize(16);
	this->updateFPSText.setFillColor(sf::Color::White);
	this->updateFPSText.setString(this->format(this->updateFPSFormat, ""));

	this->updateFPSText.setPosition(
		sf::Vector2f(
			window.getSize().x - this->updateFPSText.getLocalBounds().width, 20
		)
	);

	this->chunksText.setFont(Context::getResourceManager().get<sf::Font>("fonts/arial"));
	this->chunksText.setCharacterSize(16);
	this->chunksText.setFillColor(sf::Color::White);
	this->chunksText.setString(this->format(this->chunksFormat, ""));

	this->chunksText.setPosition(
		sf::Vector2f(
			window.getSize().x - this->chunksText.getLocalBounds().width, 40
		)
	);

	this->tilesText.setFont(Context::getResourceManager().get<sf::Font>("fonts/arial"));
	this->tilesText.setCharacterSize(16);
	this->tilesText.setFillColor(sf::Color::White);
	this->tilesText.setString(this->format(this->tilesFormat, ""));

	this->tilesText.setPosition(
		sf::Vector2f(
			window.getSize().x - this->tilesText.getLocalBounds().width, 60
		)
	);

	this->worldText.setFont(Context::getResourceManager().get<sf::Font>("fonts/arial"));
	this->worldText.setCharacterSize(16);
	this->worldText.setFillColor(sf::Color::White);
	this->worldText.setString(this->format(this->worldFormat, ""));

	this->worldText.setPosition(
		sf::Vector2f(
			window.getSize().x - this->worldText.getLocalBounds().width, 80
		)
	);

	this->chunkText.setFont(Context::getResourceManager().get<sf::Font>("fonts/arial"));
	this->chunkText.setCharacterSize(16);
	this->chunkText.setFillColor(sf::Color::White);
	this->chunkText.setString(this->format(this->chunkFormat, ""));

	this->chunkText.setPosition(
		sf::Vector2f(
			window.getSize().x - this->chunkText.getLocalBounds().width, 100
		)
	);

	this->globalText.setFont(Context::getResourceManager().get<sf::Font>("fonts/arial"));
	this->globalText.setCharacterSize(16);
	this->globalText.setFillColor(sf::Color::White);
	this->globalText.setString(this->format(this->globalFormat, ""));

	this->globalText.setPosition(
		sf::Vector2f(
			window.getSize().x - this->globalText.getLocalBounds().width, 120
		)
	);

	this->localText.setFont(Context::getResourceManager().get<sf::Font>("fonts/arial"));
	this->localText.setCharacterSize(16);
	this->localText.setFillColor(sf::Color::White);
	this->localText.setString(this->format(this->localFormat, ""));

	this->localText.setPosition(
		sf::Vector2f(
			window.getSize().x - this->localText.getLocalBounds().width, 140
		)
	);
}

void DebugInfo::setRenderFPS(unsigned int value) {
	this->renderFPSText.setString(this->format(this->renderFPSFormat, std::to_string(value).c_str()));
}

void DebugInfo::setUpdateFPS(unsigned int value) {
	this->updateFPSText.setString(this->format(this->updateFPSFormat, std::to_string(value).c_str()));
}

void DebugInfo::setNumChunks(unsigned int value) {
	this->chunksText.setString(this->format(this->chunksFormat, std::to_string(value).c_str()));
}

void DebugInfo::setNumTiles(unsigned int value) {
	this->tilesText.setString(this->format(this->tilesFormat, std::to_string(value).c_str()));
}

void DebugInfo::setWorldCoords(const sf::Vector2f& value) {
	std::string vectorStr = this->format(VECTOR_FORMAT, (int)value.x, (int)value.y);
	this->worldText.setString(this->format(this->worldFormat, vectorStr.c_str()));
}

void DebugInfo::setChunkCoords(const sf::Vector2i& value) {
	std::string vectorStr = this->format(VECTOR_FORMAT, value.x, value.y);
	this->chunkText.setString(this->format(this->chunkFormat, vectorStr.c_str()));
}

void DebugInfo::setGlobalCoords(const sf::Vector2i& value) {
	std::string vectorStr = this->format(VECTOR_FORMAT, value.x, value.y);
	this->globalText.setString(this->format(this->globalFormat, vectorStr.c_str()));
}

void DebugInfo::setLocalCoords(const sf::Vector2u& value) {
	std::string vectorStr = this->format(VECTOR_FORMAT, value.x, value.y);
	this->localText.setString(this->format(this->localFormat, vectorStr.c_str()));
}