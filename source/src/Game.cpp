#include <Game.h>

Game::Game(const GameOptions& options) {
	this->options = options;

	Context::getResourceManager().getFactory().bind<sf::Shader, std::string, std::string>([](const std::string& vertex, const std::string& fragment) {
		sf::Shader* shader = new sf::Shader();
		if (!shader->loadFromMemory(vertex, fragment))
			throw std::runtime_error("Failed to load shader");

		return shader;
	});

	Context::getResourceManager().getFactory().bind<sf::Texture, std::string>([](const std::string& path) {
		sf::Texture* texture = new sf::Texture();
		if (!texture->loadFromFile(path))
			throw std::runtime_error("Failed to load texture");

		return texture;
	});

	Context::getResourceManager().getFactory().bind<sf::Font, std::string>([](const std::string& path) {
		sf::Font* font = new sf::Font();
		if (!font->loadFromFile(path))
			throw std::runtime_error("Failed to load font");

		return font;
	});

	Context::getResourceManager().getFactory().bind<Tileset, std::string>(TilesetLoader());
}

void Game::init() {
	Context::getResourceManager().load<sf::Shader>("shaders/default", vertexShader, fragmentShader);

	Context::getResourceManager().load<sf::Texture>("previews/0", std::string("assets/previews/water.png"));
	Context::getResourceManager().load<sf::Texture>("previews/1", std::string("assets/previews/dirt.png"));
	Context::getResourceManager().load<sf::Texture>("previews/2", std::string("assets/previews/grass.png"));
	Context::getResourceManager().load<sf::Texture>("previews/3", std::string("assets/previews/concrete.png"));

	Context::getResourceManager().load<sf::Texture>("icons/brush", std::string("assets/icons/brush.png"));
	Context::getResourceManager().load<sf::Texture>("icons/size", std::string("assets/icons/size.png"));

	Context::getResourceManager().load<sf::Font>("fonts/arial", std::string("assets/fonts/arial.ttf"));

	Context::getResourceManager().load<Tileset>("tilesets/default", std::string("assets/tilesets/default.json"));

	this->world = new World(this->options.chunkSize, this->options.cacheSize);
	this->ui = new UI();
	this->debugInfo = new DebugInfo();

	this->world->getBrush().setOptions(this->ui->getBrushOptions());
	this->world->enableGrid(this->ui->getGlobalOptions().grid);
		
	this->ui->onBrushChanged([this](const BrushOptions& options) {
		this->world->getBrush().setOptions(options);
	});

	this->ui->onOptionsChanged([this](const GlobalOptions& options) {
		this->world->enableGrid(options.grid);
		this->world->enableLighting(options.lighting);
	});

	this->ui->onGenerate([this]() {
		this->world->reset();
	});
}

void Game::handleEvent(const sf::Event& event) {
	ImGui::SFML::ProcessEvent(event);

	if (this->ui->isHovered())
		return;

	switch (event.type) {
		case sf::Event::MouseWheelMoved:
		{
			float factor = 1.0f - ZOOM_FACTOR * event.mouseWheel.delta;
			this->world->getCamera().zoom(factor);
			break;
		}
		case sf::Event::MouseButtonPressed:
		{
			this->mouse.origin = this->world->screenToWorld(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

			if (event.mouseButton.button == sf::Mouse::Right)
				this->mouse.rightClick = true;

			if (event.mouseButton.button == sf::Mouse::Left) {
				this->mouse.leftClick = true;
				this->world->getBrush().paint();
			}

			break;
		}
		case sf::Event::MouseButtonReleased:
		{
			if (event.mouseButton.button == sf::Mouse::Right)
				this->mouse.rightClick = false;

			if (event.mouseButton.button == sf::Mouse::Left)
				this->mouse.leftClick = false;

			break;
		}
		case sf::Event::MouseMoved:
		{
			sf::Vector2f worldCoords = this->world->screenToWorld(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
			this->world->getBrush().setCenter(worldCoords);

			if (this->mouse.rightClick)
				this->world->getCamera().move(mouse.origin - worldCoords);

			if (this->mouse.leftClick)
				this->world->getBrush().paint();

			break;
		}
	}
}

void Game::update(const sf::Time& deltaTime) {
	this->world->update(deltaTime);
	this->ui->update(deltaTime);

	WorldDebugInfo info = this->world->getDebugInfo();
	this->debugInfo->setRenderFPS(this->renderFPS.getFPS());
	this->debugInfo->setUpdateFPS(this->updateFPS.getFPS());
	this->debugInfo->setNumChunks(info.numChunks);
	this->debugInfo->setNumTiles(info.numTiles);
	this->debugInfo->setWorldCoords(info.worldCoords);
	this->debugInfo->setChunkCoords(info.chunkCoords);
	this->debugInfo->setGlobalCoords(info.globalCoords);
	this->debugInfo->setLocalCoords(info.localCoords);

	this->updateFPS.update();
}

void Game::draw(sf::RenderTarget& target) {
	target.draw(*this->world);
	target.draw(*this->ui);

	if(this->ui->getGlobalOptions().debug)
		target.draw(*this->debugInfo);

	this->renderFPS.update();
}

Game::~Game() {
	delete this->debugInfo;
	delete this->ui;
	delete this->world;
}
