#include "UILoader.h"

#include <fstream>

#include <VolcaniCore/Core/Application.h>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/Algo.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/BinaryWriter.h>
#include <Magma/Core/BinaryReader.h>

#include "Runtime.h"
#include "AssetManager.h"

using namespace Magma;
using namespace Magma::UI;

namespace Magma {

static void ReadUI(BinaryReader* reader, UIElement* element) {
	std::string id;
	reader->Read(id);
	element->SetID(id);
	reader->Read(element->Width);
	reader->Read(element->Height);
	reader->Read(element->x);
	reader->Read(element->y);
	reader->Read((uint32_t&)element->xAlignment);
	reader->Read((uint32_t&)element->yAlignment);
	reader->Read(element->Color.r);
	reader->Read(element->Color.g);
	reader->Read(element->Color.b);
	reader->Read(element->Color.a);
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::Window& window) {
	ReadUI(this, &window);
	Read(window.BorderWidth);
	Read(window.BorderHeight);
	Read(window.BorderColor.r);
	Read(window.BorderColor.g);
	Read(window.BorderColor.b);
	Read(window.BorderColor.a);

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
	auto& assetManager =
		Application::As<Lava::Runtime>()->GetApp()->GetAssetManager();
	image.Content = assetManager.Get<Texture>(Asset{ id, AssetType::Texture });

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::Button& button) {
	ReadUI(this, &button);
	bool isText;
	Read(isText);
	if(isText) {
		auto display = CreateRef<UI::Text>();
		Read(*display);
		button.Display = display;
	}
	else {
		auto display = CreateRef<UI::Image>();
		Read(*display);
		button.Display = display;
	}

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::UINode& node) {
	Read((uint32_t&)node.first);
	Read(node.second);

	return *this;
}

}

namespace Lava {

Ref<ScriptModule> UILoader::Load(UIPage& page, const std::string& path) {
	namespace fs = std::filesystem;

	// auto pagePath =
	// 	(fs::path(projectPath) / "UI" / "Data" / page.Name).string() + ".bin";
	// auto funcPath =
	// 	(fs::path(projectPath) / "UI" / "Func" / page.Name).string() + ".class";

	// BinaryReader reader(pagePath);

	// reader.Read(page.Name);

	// reader
	// 	.Read(page.Windows, "", &page)
	// 	.Read(page.Buttons, "", &page)
	// 	.Read(page.Dropdowns, "", &page)
	// 	.Read(page.Texts, "", &page)
	// 	.Read(page.TextInputs, "", &page)
	// 	.Read(page.Images, "", &page);

	// reader.Read(page.FirstOrders);

	// auto mod = CreateRef<ScriptModule>(page.Name);
	// mod->Reload(funcPath);
	// return mod;
}

}