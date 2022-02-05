#ifndef UI_H
#define UI_H

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <functional>
#include <Context.h>
#include <TileBrush.h>

struct GlobalOptions {
	bool lighting;
	bool grid;
	bool debug;
};

class UI : public sf::Drawable {
	BrushOptions brushOptions = { 0, 1, false };
	GlobalOptions globalOptions = { false, false, false };

	std::vector<sf::Texture *> textures;
	sf::Texture * brushIcon;
	sf::Texture * sizeIcon;

	std::function<void()> onGenerateCallback;
	std::function<void(const BrushOptions&)> onBrushChangedCallback;
	std::function<void(const GlobalOptions&)> onOptionsChangedCallback;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
	UI();

	void update(const sf::Time& deltaTime);

	const BrushOptions& getBrushOptions() const;

	const GlobalOptions& getGlobalOptions() const;
	
	bool isHovered() const;

	void onGenerate(std::function<void()> callback);

	void onBrushChanged(std::function<void(const BrushOptions&)> callback);

	void onOptionsChanged(std::function<void(const GlobalOptions&)> callback);

	~UI();
};

#endif