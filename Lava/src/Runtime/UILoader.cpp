#include "UILoader.h"

#include <fstream>

#include <VolcaniCore/Core/Application.h>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/Algo.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/BinaryWriter.h>
#include <Magma/Core/BinaryReader.h>

#include <Lava/App.h>

#include "Runtime.h"
#include "AssetManager.h"

using namespace Magma;
using namespace Magma::UI;
using namespace Lava;

static UIPage* s_CurrentPage;

namespace Magma {

template<>
BinaryReader& BinaryReader::ReadObject(UI::UINode& node) {
	uint32_t type;
	Read(type);
	node.first = (UI::UIElementType)type;
	Read(node.second);

	return *this;
}

static void ReadUI(BinaryReader* reader, UIElement* element) {
	std::string id;
	reader->Read(id);
	element->SetID(id);
	reader->Read(element->Width);
	reader->Read(element->Height);
	reader->Read(element->x);
	reader->Read(element->y);
	uint32_t xAlign, yAlign;
	reader->Read(xAlign);
	reader->Read(yAlign);
	element->xAlignment = (XAlignment)xAlign;
	element->yAlignment = (YAlignment)yAlign;
	reader->Read(element->UsePosition);
	reader->ReadData(&element->Color.r, sizeof(glm::vec4));
	reader->Read(element->Children);
	uint64_t assetID;
	reader->Read(assetID);
	element->ModuleID = assetID;
	reader->Read(element->Class);
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::Window& window) {
	ReadUI(this, &window);
	Read(window.BorderWidth);
	Read(window.BorderHeight);
	ReadData(&window.BorderColor.r, sizeof(glm::vec4));

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::Dropdown& dropdown) {
	ReadUI(this, &dropdown);
	Read(dropdown.CurrentItem);
	Read(dropdown.Options);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::Text& text) {
	ReadUI(this, &text);
	Read(text.Content);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::TextInput& textInput) {
	ReadUI(this, &textInput);
	Read(textInput.Text);
	Read(textInput.Hint);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::Image& image) {
	ReadUI(this, &image);
	uint64_t id;
	Read(id);
	Asset asset = { id, AssetType::Texture };
	auto* assetManager = App::Get()->GetAssetManager();
	image.Content = assetManager->Get<Texture>(asset);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::Button& button) {
	ReadUI(this, &button);
	bool isText;
	Read(isText);
	if(isText) {
		auto display = CreateRef<UI::Text>("", s_CurrentPage);
		Read(*display);
		button.Display = display;
	}
	else {
		auto display = CreateRef<UI::Image>("", s_CurrentPage);
		Read(*display);
		button.Display = display;
	}

	return *this;
}

}

namespace Lava {

void UILoader::Load(UIPage& page, const std::string& path) {
	namespace fs = std::filesystem;

	BinaryReader reader(path);

	s_CurrentPage = &page;
	reader.Read(page.Name);
	reader
		.Read(page.Windows, "", &page)
		.Read(page.Buttons, "", &page)
		.Read(page.Dropdowns, "", &page)
		.Read(page.Texts, "", &page)
		.Read(page.TextInputs, "", &page)
		.Read(page.Images, "", &page);

	reader.Read(page.LayerNodes);
	s_CurrentPage = nullptr;
}

}