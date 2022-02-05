#include <UI.h>

UI::UI() {
	this->textures = {
		&Context::getResourceManager().get<sf::Texture>("previews/0"),
		&Context::getResourceManager().get<sf::Texture>("previews/1"),
		&Context::getResourceManager().get<sf::Texture>("previews/2"),
		&Context::getResourceManager().get<sf::Texture>("previews/3")
	};

	this->brushIcon = &Context::getResourceManager().get<sf::Texture>("icons/brush");
	this->sizeIcon = &Context::getResourceManager().get<sf::Texture>("icons/size");

	ImGui::SFML::Init(Context::getWindow());
}

void UI::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.setView(Context::getWindow().getDefaultView());
	ImGui::SFML::Render(target);
}

void UI::update(const sf::Time& deltaTime) {
	ImGui::SFML::Update(Context::getWindow(), deltaTime);

	ImGui::Begin(
		"UI",
		0,
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse
	);

	ImGui::SetWindowSize(ImVec2(0, 0));

	ImGui::SetWindowPos(ImVec2(0, 0));

	if (this->brushOptions.active) {
		if (ImGui::Button("Generate", ImVec2(-1, ImGui::GetWindowSize().y * 0.1f)))
			onGenerateCallback();

		ImVec2 padding(2, 2);
		ImVec2 spacing(0, 5);

		ImVec2 textureSize(64, 64);

		int scrollOffset = (this->textures.size() * (textureSize.y + spacing.y) > (textureSize.y + spacing.y) * 4 ? 20 : 0);
		ImVec2 listSize(textureSize.x + scrollOffset, (textureSize.y + spacing.y) * 4);

		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, padding);
		ImGui::BeginTable("brush_table", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Image(*this->brushIcon, ImVec2(32, 32));

		ImGui::TableNextColumn();
		ImGui::Image(*this->sizeIcon, ImVec2(32, 32));

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		ImGui::BeginChild("tile_list", listSize, false);

		for (int i = 0; i < this->textures.size(); i++) {
			sf::Texture * texture = this->textures.at(i);

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);
			ImGui::Image(*texture, textureSize);
			ImGui::PopStyleVar();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (textureSize.y + spacing.y));

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

			if (ImGui::Selectable(("##" + std::to_string(i)).c_str(), (this->brushOptions.type == i), ImGuiSelectableFlags_None, ImVec2(64, 64))) {
				this->brushOptions.type = i;
				this->onBrushChangedCallback(this->brushOptions);
			}

			ImGui::PopStyleVar();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + spacing.y);
		}

		ImGui::EndChild();
		ImGui::TableNextColumn();

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2 - ImGui::GetWindowSize().x * 0.10f / 2);
		if (ImGui::VSliderInt("##brushSize", ImVec2(ImGui::GetWindowSize().x * 0.10f, listSize.y), (int*)&brushOptions.brushSize, 1, 5))
			this->onBrushChangedCallback(this->brushOptions);

		ImGui::EndTable();
		ImGui::PopStyleVar();
	}

	if (ImGui::Checkbox("Edit", &this->brushOptions.active))
		this->onBrushChangedCallback(this->brushOptions);

	if (ImGui::Checkbox("Lighting", &this->globalOptions.lighting))
		this->onOptionsChangedCallback(this->globalOptions);

	if (ImGui::Checkbox("Grid", &this->globalOptions.grid))
		this->onOptionsChangedCallback(this->globalOptions);

	if (ImGui::Checkbox("Debug", &this->globalOptions.debug))
		this->onOptionsChangedCallback(this->globalOptions);

	ImGui::End();

	ImGui::EndFrame();
}

const BrushOptions& UI::getBrushOptions() const {
	return this->brushOptions;
}

const GlobalOptions& UI::getGlobalOptions() const {
	return this->globalOptions;
}

bool UI::isHovered() const {
	return (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
		|| ImGui::IsAnyItemHovered()
		|| ImGui::IsAnyItemActive()
		|| ImGui::IsAnyItemFocused());
}

void UI::onGenerate(std::function<void()> callback) {
	this->onGenerateCallback = callback;
}

void UI::onBrushChanged(std::function<void(const BrushOptions&)> callback) {
	this->onBrushChangedCallback = callback;
}

void UI::onOptionsChanged(std::function<void(const GlobalOptions&)> callback) {
	this->onOptionsChangedCallback = callback;
}

UI::~UI() {
	ImGui::SFML::Shutdown();
}