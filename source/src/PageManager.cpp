#include <PageManager.h>

PageManager::PageManager(std::size_t pageSize) {
	this->pageSize = pageSize;
	this->options = {};
	std::filesystem::create_directory(TEMP_DIR);
}

void PageManager::load(unsigned int id) {
	TaskScheduler& scheduler = Context::getTaskScheduler();
	
	std::string path = TEMP_DIR + std::string("/") + std::to_string(id) + ".png";

	if (scheduler.isInProgress(id))
		return;

	scheduler.dispatch<const sf::Uint8 *>(
		id,
		std::bind(PageImporter(), path),
		[this, id](const sf::Uint8 data[]) {
			if (this->onLoadCompletedCallback) this->onLoadCompletedCallback(id, data);
			delete data;
		}
	);
}

void PageManager::generate(unsigned int id, const sf::IntRect& bounds) {
	TaskScheduler& scheduler = Context::getTaskScheduler();

	std::string path = TEMP_DIR + std::string("/") + std::to_string(id) + ".png";

	if (scheduler.isInProgress(id))
		return;

	scheduler.dispatch<const sf::Uint8 *>(
		id,
		std::bind(PageGenerator(this->options), bounds),
		[this, id](const sf::Uint8 data[]) {
			if(this->onLoadCompletedCallback) this->onLoadCompletedCallback(id, data);
			delete data;
		}
	);
}

void PageManager::save(unsigned int id, const sf::Uint8 data[]) {
	TaskScheduler& scheduler = Context::getTaskScheduler();

	std::string path = TEMP_DIR + std::string("/") + std::to_string(id) + ".png";

	if (scheduler.isInProgress(id))
		return;

	sf::Uint8* copy = new sf::Uint8[this->pageSize * this->pageSize];
	for (int i = 0; i < this->pageSize * this->pageSize; i++)
		copy[i] = data[i];

	scheduler.dispatch<int>(
		id,
		std::bind(PageExporter(), path, this->pageSize, copy),
		[this, id, copy](int result) {
			if(this->onSaveCompletedCallback) this->onSaveCompletedCallback(id);
			delete copy;
		},
		[copy](const std::exception& e) {
			delete copy;
		}
	);
}

void PageManager::clear() {
	for (auto& path : std::filesystem::directory_iterator(TEMP_DIR))
		std::filesystem::remove_all(path);
}

bool PageManager::exists(unsigned int id) {
	std::string path = TEMP_DIR + std::string("/") + std::to_string(id) + ".png";
	return std::filesystem::exists(path);
}

void PageManager::setNoiseOptions(const NoiseOptions& options) {
	this->options = options;
}

void PageManager::onLoadCompleted(std::function<void(unsigned int, const sf::Uint8[])> callback) {
	this->onLoadCompletedCallback = callback;
}

void PageManager::onSaveCompleted(std::function<void(unsigned int)> callback) {
	this->onSaveCompletedCallback = callback;
}

PageManager::~PageManager() {
	std::filesystem::remove_all(TEMP_DIR);
}
