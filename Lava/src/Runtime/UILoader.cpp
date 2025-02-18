#include "UILoader.h"

#include <fstream>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/Algo.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/BinaryWriter.h>
#include <Magma/Core/BinaryReader.h>

using namespace Magma;
using namespace Magma::UI;

namespace Magma {

static void WriteUI(BinaryWriter* writer, const UIElement* element) {
	writer->Write(element->GetID());
	writer->Write(element->Width);
	writer->Write(element->Height);
	writer->Write(element->x);
	writer->Write(element->y);
	writer->Write((uint32_t)element->xAlignment);
	writer->Write((uint32_t)element->yAlignment);
	writer->Write(element->Color.r);
	writer->Write(element->Color.g);
	writer->Write(element->Color.b);
	writer->Write(element->Color.a);
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const UI::Window& window) {
	WriteUI(this, &window);
	Write(window.BorderWidth);
	Write(window.BorderHeight);
	Write(window.BorderColor.r);
	Write(window.BorderColor.g);
	Write(window.BorderColor.b);
	Write(window.BorderColor.a);
	
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const UI::Dropdown& dropdown) {
	WriteUI(this, &dropdown);
	Write(dropdown.CurrentItem);
	Write(dropdown.Options);
	
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const UI::Text& text) {
	WriteUI(this, &text);
	Write(text.Content);
	
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const UI::TextInput& textInput) {
	WriteUI(this, &textInput);
	Write(textInput.Text);
	Write(textInput.Hint);
	
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const UI::Image& image) {
	WriteUI(this, &image);
	// Write((uint64_t)image.ImageAsset.ID);
	
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const UI::Button& button) {
	WriteUI(this, &button);
	auto display = button.Display;
	Write(display->Is(UI::UIElementType::Text));
	if(display->Is(UI::UIElementType::Text))
		Write(*display->As<UI::Text>());
	else
		Write(*display->As<UI::Image>());

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const UI::UINode& node) {
	Write((uint32_t)node.first);
	Write(node.second);
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
	// image.ImageAsset = { id, AssetType::Texture };

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

void UILoader::Save(const UIPage& page, const std::string& path) {
	namespace fs = std::filesystem;

	// auto dataPath =
	// 	(fs::path(path) / "UI" / "Data" / page.Name).string() + ".bin";
	// BinaryWriter writer(dataPath);

	// writer.Write(page.Name);

	// writer
	// 	.Write(page.Windows)
	// 	.Write(page.Buttons)
	// 	.Write(page.Dropdowns)
	// 	.Write(page.Texts)
	// 	.Write(page.TextInputs)
	// 	.Write(page.Images);

	// writer.Write(page.FirstOrders);
}

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