#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include <Context.h>
#include <PageGenerator.h>
#include <PageImporter.h>
#include <PageExporter.h>
#include <filesystem>

#define PAGES_KEY "pages/"
#define TEMP_DIR "tmp"

class PageManager {
	NoiseOptions options;
	std::size_t pageSize;

	std::function<void(unsigned int, const sf::Uint8[])> onLoadCompletedCallback;
	std::function<void(unsigned int)> onSaveCompletedCallback;
public:
	PageManager(std::size_t pageSize);

	void load(unsigned int id);

	void generate(unsigned int id, const sf::IntRect& bounds);

	void save(unsigned int id, const sf::Uint8 data[]);

	void clear();

	bool exists(unsigned int id);

	void setNoiseOptions(const NoiseOptions& options);

	void onLoadCompleted(std::function<void(unsigned int, const sf::Uint8[])> callback);

	void onSaveCompleted(std::function<void(unsigned int)> callback);

	~PageManager();
};
#endif