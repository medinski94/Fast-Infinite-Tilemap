#include <Engine.h>

Engine::Engine(const sf::Vector2u& windowSize, const std::string& title, bool frameLimit = false) : window(Context::getWindow()) {
	this->window.create(sf::VideoMode(windowSize.x, windowSize.y), title);

	if (frameLimit) this->window.setFramerateLimit(60);
}

void Engine::handleEvent() {
	Context::getTaskScheduler().poll();
	
	sf::Event event;
	while (this->window.pollEvent(event)) {
		switch (event.type) {
			case sf::Event::Closed: {
				this->window.close();
				break;
			}
			default: {
				this->game->handleEvent(event);
				break;
			}
		}
	}
}

void Engine::update(const sf::Time& deltaTime) {
	this->game->update(deltaTime);	
}

void Engine::draw() {
	this->window.clear(sf::Color::Transparent);
		
	this->game->draw(this->window);
	
	this->window.display();
}

void Engine::setGame(Game* game) {
	this->game = game;
	this->game->init();
}

void Engine::run() {
	const sf::Time deltaTime = sf::seconds(UPDATE_RATE);
	sf::Time& time = Context::getTime();
	sf::Time accumulator = deltaTime;
	sf::Clock clock;

	while (this->window.isOpen()) {
		accumulator += clock.restart();
		
		while (accumulator >= deltaTime) {
			this->handleEvent();
			this->update(deltaTime);
			time += deltaTime;
			accumulator -= deltaTime;
		}

		this->draw();
	}
}

Engine::~Engine() {
	delete this->game;
}