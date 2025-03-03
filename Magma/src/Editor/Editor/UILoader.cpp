#include "UILoader.h"

#include <fstream>

#include <VolcaniCore/Core/Application.h>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/Algo.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/JSONSerializer.h>

#include <Magma/Core/BinaryWriter.h>
#include <Magma/Core/BinaryReader.h>

#include <Magma/UI/UI.h>

#include "EditorApp.h"
#include "AssetManager.h"

namespace fs = std::filesystem;

using namespace Magma;
using namespace Magma::UI;

namespace Magma {

static void LoadElement(UIPage& page, const rapidjson::Value& elementNode, 
						const Theme& theme);

void UILoader::EditorLoad(UIPage& page, const std::string& path,
						  const Theme& theme)
{
	using namespace rapidjson;

	if(path == "") {
		VOLCANICORE_LOG_ERROR("Filename was empty");
		return;
	}

	auto name = fs::path(path).stem().stem().stem().string();
	auto funcFolder = (fs::path(path).parent_path() / "Func").string();
	auto funcPath = funcFolder + ".magma.ui.func";
	if(!FileUtils::FileExists(path)) {
		VOLCANICORE_LOG_ERROR(
			"Could not find .magma.ui.json file with name %s", name.c_str());
		return;
	}

	std::string file = FileUtils::ReadFile(path);
	Document doc;
	doc.Parse(file);

	if(doc.HasParseError()) {
		VOLCANICORE_LOG_INFO("%i", (uint32_t)doc.GetParseError());
		return;
	}
	if(!doc.IsObject()) {
		VOLCANICORE_LOG_ERROR("File did not have root object");
		return;
	}

	if(doc.HasMember("Elements")) {
		const auto& elements = doc["Elements"];
		for(const auto& element : elements.GetArray())
			LoadElement(page, element, theme);
	}

	page.Name = name;
}

template<typename TUIElement>
static void Serialize(const TUIElement* ui, JSONSerializer& serializer) { }

template<>
void Serialize(const UI::Window* ui, JSONSerializer& serializer) {
	serializer
		.WriteKey("BorderWidth").Write(ui->BorderWidth)
		.WriteKey("BorderHeight").Write(ui->BorderHeight)
		.WriteKey("BorderColor").Write(ui->BorderColor);
}

template<>
void Serialize(const Dropdown* ui, JSONSerializer& serializer) {
	serializer.WriteKey("Options");
	serializer.Write(ui->Options);
}

template<>
void Serialize(const Text* ui, JSONSerializer& serializer) {
	serializer
		.WriteKey("Text").Write(ui->Content)
		.WriteKey("TextColor").Write(ui->Color);
}

template<>
void Serialize(const TextInput* ui, JSONSerializer& serializer) {
	serializer
		.WriteKey("Hint").Write(ui->Hint);
}

template<>
void Serialize(const Image* ui, JSONSerializer& serializer) {
	serializer.WriteKey("Asset").Write((uint64_t)ui->ImageID);
}

template<>
void Serialize(const Button* ui, JSONSerializer& serializer) {
	if(ui->Display->GetType() == UIElementType::Text)
		Serialize(ui->Display->As<Text>(), serializer);
	else
		Serialize(ui->Display->As<Image>(), serializer);
}

template<>
void Serialize(const UIElement* ui, JSONSerializer& serializer) {
	serializer
	.BeginMapping();

	serializer
		.WriteKey("ID").Write(ui->GetID());

	serializer.WriteKey("Type");

	switch(ui->GetType()) {
		case UIElementType::Window:
			serializer.Write("Window");
			break;
		case UIElementType::Button:
			serializer.Write("Button");
			break;
		case UIElementType::Dropdown:
			serializer.Write("Dropdown");
			break;
		case UIElementType::Text:
			serializer.Write("Text");
			break;
		case UIElementType::TextInput:
			serializer.Write("TextInput");
			break;
		case UIElementType::Image:
			serializer.Write("Image");
			break;
	}

	if(ui->GetParent())
		serializer.WriteKey("Parent").Write(ui->GetParent()->GetID());

	serializer
		.WriteKey("Width").Write(ui->Width)
		.WriteKey("Height").Write(ui->Height)
		.WriteKey("x").Write(ui->x)
		.WriteKey("y").Write(ui->y)
		.WriteKey("xAlignment")
			.Write(
				[&]() -> std::string
				{
					switch(ui->xAlignment) {
						case XAlignment::Left:
							return "Left";
						case XAlignment::Center:
							return "Center";
						case XAlignment::Right:
							return "Right";

						default:
							return "None";
					}
				}())
		.WriteKey("yAlignment")
			.Write(
				[&]() -> std::string
				{
					switch(ui->yAlignment) {
						case YAlignment::Top:
							return "Top";
						case YAlignment::Center:
							return "Center";
						case YAlignment::Bottom:
							return "Bottom";
						
						default:
							return "None";
					}
				}())
		.WriteKey("Color").Write(ui->Color);

	switch(ui->GetType()) {
		case UIElementType::Window:
			Serialize(ui->As<UI::Window>(), serializer);
			break;
		case UIElementType::Button:
			Serialize(ui->As<Button>(), serializer);
			break;
		case UIElementType::Image:
			Serialize(ui->As<Image>(), serializer);
			break;
		case UIElementType::Text:
			Serialize(ui->As<Text>(), serializer);
			break;
		case UIElementType::TextInput:
			Serialize(ui->As<TextInput>(), serializer);
			break;
		case UIElementType::Dropdown:
			Serialize(ui->As<Dropdown>(), serializer);
			break;
	}

	serializer.EndMapping();

	for(auto child : ui->GetChildren())
		Serialize(child, serializer);
}

void UILoader::EditorSave(const UIPage& page, const std::string& path) {
	JSONSerializer serializer;
	serializer.BeginMapping(); // File

	serializer.WriteKey("Elements")
	.BeginSequence();

	for(auto* element : page.GetFirstOrderElements())
		Serialize(element, serializer);

	serializer.EndSequence();

	serializer.EndMapping(); // File
	serializer.Finalize(path);
}

Theme UILoader::LoadTheme(const std::string& path) {
	std::string file = FileUtils::ReadFile(path);
	Document doc;
	doc.Parse(file);

	Theme res;
	if(!doc.HasMember("Theme")) {
		VOLCANICORE_LOG_WARNING("Failed to load theme '%s'", path.c_str());
		return res;
	}

	const auto& themeNode = doc["Theme"];

	if(themeNode.HasMember("Window")) {
		const auto& windowTheme = themeNode["Window"];
		auto& theme =
			res.emplace(UIElementType::Window, ThemeElement{ }).first->second;

		if(windowTheme.HasMember("Width"))
			theme.Width = windowTheme["Width"].Get<uint32_t>();
		if(windowTheme.HasMember("Height"))
			theme.Height = windowTheme["Height"].Get<uint32_t>();

		if(windowTheme.HasMember("x"))
			theme.x = windowTheme["x"].Get<int32_t>();
		if(windowTheme.HasMember("y"))
			theme.y = windowTheme["y"].Get<int32_t>();

		if(windowTheme.HasMember("xAlignment")) {
			const auto& xAlign = windowTheme["xAlignment"];
			if(xAlign.Get<std::string>() == "Left")
				theme.xAlignment = XAlignment::Left;
			if(xAlign.Get<std::string>() == "Center")
				theme.xAlignment = XAlignment::Center;
			if(xAlign.Get<std::string>() == "Right")
				theme.xAlignment = XAlignment::Right;
		}
		if(windowTheme.HasMember("yAlignment")) {
			const auto& yAlign = windowTheme["yAlignment"];
			if(yAlign.Get<std::string>() == "Top")
				theme.yAlignment = YAlignment::Top;
			if(yAlign.Get<std::string>() == "Center")
				theme.yAlignment = YAlignment::Center;
			if(yAlign.Get<std::string>() == "Bottom")
				theme.yAlignment = YAlignment::Bottom;
		}

		if(windowTheme.HasMember("Color"))
			theme.Color =
				glm::vec4
				{
					windowTheme["Color"][0].Get<float>(),
					windowTheme["Color"][1].Get<float>(),
					windowTheme["Color"][2].Get<float>(),
					windowTheme["Color"][3].Get<float>()
				};

		if(windowTheme.HasMember("Border")) {
			const auto& borderTheme = windowTheme["Border"];

			if(borderTheme.HasMember("Width"))
				theme.BorderWidth = borderTheme["Width"].Get<uint32_t>();
			if(borderTheme.HasMember("Height"))
				theme.BorderHeight = borderTheme["Height"].Get<uint32_t>();
			if(borderTheme.HasMember("Color"))
				theme.BorderColor =
					glm::vec4
					{
						borderTheme["Color"][0].Get<float>(),
						borderTheme["Color"][1].Get<float>(),
						borderTheme["Color"][2].Get<float>(),
						borderTheme["Color"][3].Get<float>()
					};
		}
	}

	return res;
}

}

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
	Write((uint64_t)image.ImageID);

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

}

namespace Magma {

void UILoader::RuntimeSave(const UIPage& page, const std::string& projectPath,
						   const std::string& exportPath)
{
	auto pagePath =
		(fs::path(projectPath) / "Visual" / "UI" / "Page" / page.Name
		).string() + ".magma.ui.json";
	auto funcPath =
		(fs::path(projectPath) / "Visual" / "UI" / "Func" / page.Name
		).string() + ".magma.ui.func";

	auto dataPath =
		(fs::path(exportPath) / "UI" / "Data" / page.Name).string() + ".bin";
	BinaryWriter writer(dataPath);

	writer.Write(page.Name);

	writer
		.Write(page.Windows)
		.Write(page.Buttons)
		.Write(page.Dropdowns)
		.Write(page.Texts)
		.Write(page.TextInputs)
		.Write(page.Images);

	writer.Write(page.FirstOrders);
}

template<typename T>
T TryGet(const rapidjson::Value& node, const std::string& name, const T& alt) {
	if(node.HasMember(name))
		return node[name].Get<T>();
	return alt;
}

void LoadElement(UIPage& page, const rapidjson::Value& elementNode,
				 const Theme& pageTheme)
{
	std::string id = elementNode["ID"].Get<std::string>();
	std::string typeStr = elementNode["Type"].Get<std::string>();
	std::string parent = "";

	if(elementNode.HasMember("Parent"))
		parent = elementNode["Parent"].Get<std::string>();

	UINode node;
	UIElement* element;
	ThemeElement theme;

	if(typeStr == "Window") {
		node = page.Add(UIElementType::Window, id);
		element = page.Get(node);
		if(pageTheme.count(element->GetType()))
			theme = pageTheme.at(element->GetType());
		else
			theme = { };
		auto* window = element->As<UI::Window>();

		if(elementNode.HasMember("Border")) {
			const auto& borderNode = elementNode["Border"];
			window->BorderWidth
				= TryGet<uint32_t>(borderNode, "Width", theme.BorderWidth);
			window->BorderHeight
				= TryGet<uint32_t>(borderNode, "Height", theme.BorderHeight);

			window->BorderColor = theme.BorderColor;
			if(borderNode.HasMember("Color"))
				window->BorderColor =
				glm::vec4
				{
					borderNode["Color"][0].Get<float>(),
					borderNode["Color"][1].Get<float>(),
					borderNode["Color"][2].Get<float>(),
					borderNode["Color"][3].Get<float>()
				};
		}
	}
	if(typeStr == "Button") {
		node = page.Add(UIElementType::Button, id);
		element = page.Get(node);
		if(pageTheme.count(element->GetType()))
			theme = pageTheme.at(element->GetType());
		else
			theme = { };
		auto* button = element->As<Button>();

		if(elementNode.HasMember("Image")) {
			auto& assetManager =
				Application::As<EditorApp>()->GetEditor().GetAssetManager();
			auto id =
				assetManager.GetFromPath(
					elementNode["Image"].Get<std::string>());
			auto asset = Asset{ id, AssetType::Texture };
			auto image = assetManager.Get<Texture>(asset);

			button->Display = CreateRef<Image>(image);
			button->ImageID = asset;
		}
		else if(elementNode.HasMember("Text"))
			button->Display
				= CreateRef<Text>(elementNode["Text"].Get<std::string>());
		else
			button->Display = CreateRef<Text>(button->GetID());
	}
	if(typeStr == "Image") {
		node = page.Add(UIElementType::Image, id);
		element = page.Get(node);
		if(pageTheme.count(element->GetType()))
			theme = pageTheme.at(element->GetType());
		else
			theme = { };
	}
	if(typeStr == "Text") {
			node = page.Add(UIElementType::Text, id);
			element = page.Get(node);
			
		if(pageTheme.count(element->GetType()))
			theme = pageTheme.at(element->GetType());
		else
			theme = { };
		auto text = element->As<Text>();
		text->Content = elementNode["Text"].Get<std::string>();
	}
	if(typeStr == "TextInput") {
		node = page.Add(UIElementType::TextInput, id);
		element = page.Get(node);
		if(pageTheme.count(element->GetType()))
			theme = pageTheme.at(element->GetType());
		else
			theme = { };
	}
	if(typeStr == "Dropdown") {
		node = page.Add(UIElementType::Dropdown, id);
		element = page.Get(node);
		if(pageTheme.count(element->GetType()))
			theme = pageTheme.at(element->GetType());
		else
			theme = { };
	}

	if(parent == "")
		page.Add(node);
	else
		page.Get(parent)->Add(node);

	element->Width = TryGet<uint32_t>(elementNode, "Width", theme.Width);
	element->Height = TryGet<uint32_t>(elementNode, "Height", theme.Height);

	element->x = TryGet<int32_t>(elementNode, "x", theme.x);
	element->y = TryGet<int32_t>(elementNode, "y", theme.y);

	element->xAlignment = theme.xAlignment;
	if(elementNode.HasMember("xAlignment")) {
		std::string xAlign = elementNode["xAlignment"].Get<std::string>();
		if(xAlign == "Left")
			element->xAlignment = XAlignment::Left;
		else if(xAlign == "Center")
			element->xAlignment = XAlignment::Center;
		else if(xAlign == "Right")
			element->xAlignment = XAlignment::Right;
	}

	element->yAlignment = theme.yAlignment;
	if(elementNode.HasMember("yAlignment")) {
		std::string xAlign = elementNode["yAlignment"].Get<std::string>();
		if(xAlign == "Top")
			element->yAlignment = YAlignment::Top;
		else if(xAlign == "Center")
			element->yAlignment = YAlignment::Center;
		else if(xAlign == "Bottom")
			element->yAlignment = YAlignment::Bottom;
	}

	element->Color = theme.Color;
	if(elementNode.HasMember("Color"))
		element->Color =
			glm::vec4
			{
				elementNode["Color"][0].Get<float>(),
				elementNode["Color"][1].Get<float>(),
				elementNode["Color"][2].Get<float>(),
				elementNode["Color"][3].Get<float>()
			};
}

}