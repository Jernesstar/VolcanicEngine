#include "UIPage.h"

#include <filesystem>

#include <rapidjson/rapidjson.h>

#include <VolcaniCore/Core/FileUtils.h>

#define GET_LIST(TUIElement) \
template<> \
List<TUIElement>& UIPage::GetList<TUIElement>() { \
	return TUIElement##s; \
}

#define GET_TYPE(TUIElement) \
template<> \
UIElement::Type UIPage::GetType<TUIElement>() { \
	return UIElement::Type::TUIElement; \
}

namespace Magma::UI {

static void LoadElement(UIPage* page, const rapidjson::Value& docElement);
static void CompileElement(const rapidjson::Value& docElement);
static void CompilePage(const std::string& fileNamePath);

GET_LIST(Window)
GET_LIST(Button)
GET_LIST(Dropdown)
GET_LIST(Text)
GET_LIST(TextInput)
GET_LIST(Image)

GET_TYPE(Window)
GET_TYPE(Button)
GET_TYPE(Dropdown)
GET_TYPE(Text)
GET_TYPE(TextInput)
GET_TYPE(Image)

UIPage::UIPage(const std::string& filePathName) {
	Load(filePathName);
}

void UIPage::Render() {
	for(auto* element : GetFirstOrderElements())
		element->Render();
}

void UIPage::Load(const std::string& filePathName) {
	using namespace rapidjson;

	auto jsonPath = filePathName + ".magma.ui.json";
	if(!FileUtils::FileExists(jsonPath))
		return;
	CompilePage(filePathName);

	std::string file = FileUtils::ReadFile(jsonPath);
	Document doc;
	doc.Parse(file);

	if(doc.HasParseError())
		return;
	if(!doc.IsObject())
		return;

	if(doc.HasMember("Theme")) {

	}

	if(doc.HasMember("Elements")) {
		const auto& elements = doc["Elements"];
		for(const auto& element : elements.GetArray()) {
			LoadElement(this, element);
			CompileElement(element);
		}
	}
}

UIElement* UIPage::Get(const UINode& node) const {
	// TODO(Fix): The pointers might become invalid if the map reallocates
	switch(node.first) {
		case UIElement::Type::Window:
			return (UIElement*)&Windows[node.second];
		case UIElement::Type::Button:
			return (UIElement*)&Buttons[node.second];
		case UIElement::Type::Dropdown:
			return (UIElement*)&Dropdowns[node.second];
		case UIElement::Type::Text:
			return (UIElement*)&Texts[node.second];
		case UIElement::Type::TextInput:
			return (UIElement*)&TextInputs[node.second];
		case UIElement::Type::Image:
			return (UIElement*)&Images[node.second];
	}
}

UIElement* UIPage::Get(const std::string& id) const {

}

List<UIElement*> UIPage::GetFirstOrderElements() const {
	List<UIElement*> res(m_FirstOrders.size());
	for(auto node : m_FirstOrders)
		res.push_back(Get(node));

	return res;
}

void LoadElement(UIPage* page, const rapidjson::Value& docElement) {
	std::string id = docElement["ID"].GetString();
	std::string typeStr = docElement["Type"].GetString();
	UIElement* element;
	if(typeStr == "Window") {
		element = page->Get(page->Add(UIElement::Type::Window, id));

	}
	if(typeStr == "Button") {
		element = page->Get(page->Add(UIElement::Type::Button, id));

	}
	if(typeStr == "Dropdown") {
		element = page->Get(page->Add(UIElement::Type::Dropdown, id));
		
	}
	if(typeStr == "Text") {
		element = page->Get(page->Add(UIElement::Type::Text, id));
		
	}
	if(typeStr == "TextInput") {
		element = page->Get(page->Add(UIElement::Type::TextInput, id));
		
	}
	if(typeStr == "Image") {
		element = page->Get(page->Add(UIElement::Type::Image, id));
		
	}

	// TODO(Implement): Element alignement
	auto width = docElement["Width"].Get<uint32_t>();
	auto height = docElement["Height"].Get<uint32_t>();
	auto x = docElement["x"].Get<uint32_t>();
	auto y = docElement["y"].Get<uint32_t>();
	const auto& array = docElement["Color"];
	auto color = glm::vec4{
		array[0].Get<float>(),
		array[1].Get<float>(),
		array[2].Get<float>(),
		array[3].Get<float>()
	};

	element->Color = color;
	element->SetSize(width, height);
	element->SetPosition(x, y);
}

void CompileElement(const rapidjson::Value& docElement) {
	if(!docElement.HasMember("OnClick")
	|| !docElement.HasMember("OnHover")
	|| !docElement.HasMember("OnMouseUp"))
		return;

	
}

void CompilePage(const std::string& fileNamePath) {
	namespace fs = std::filesystem;
	auto name = fs::path(fileNamePath).filename();
	auto genPath = ("projects/gen" / name).string();

	FileUtils::CreateFile(genPath + ".h");
	FileUtils::CreateFile(genPath + ".cpp");

	std::string hContent =
	"#pragma once\n"
	""
	;
}

}