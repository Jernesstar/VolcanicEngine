#include "UIPage.h"

#include <filesystem>

#include <rapidjson/rapidjson.h>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/FileUtils.h>

#include "UIClickable.h"

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

static void GenFiles(const std::string& genPath, const std::string& funcPath);
static void CompleteFiles(const std::string& genPath);
static void LoadElement(UIPage* page, const rapidjson::Value& docElement);
static void CompileElement(const std::string& genPath,
						   const std::string& funcPath,
						   const rapidjson::Value& docElement);

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

void UIPage::Load(const std::string& filePathName) {
	using namespace rapidjson;

	m_Path = filePathName;
	auto jsonPath = m_Path + ".magma.ui.json";
	auto funcPath = m_Path + ".magma.ui.func";
	if(!FileUtils::FileExists(jsonPath))
		return;

	std::string file = FileUtils::ReadFile(jsonPath);
	Document doc;
	doc.Parse(file);

	if(doc.HasParseError()) {
		VOLCANICORE_LOG_INFO("%i", (uint32_t)doc.GetParseError());
		return;
	}
	if(!doc.IsObject())
		return;

	if(doc.HasMember("Theme")) {

	}

	namespace fs = std::filesystem;
	auto name = fs::path(m_Path).filename();
	auto genPath = "Magma/projects/Project/gen/" + name.string();

	GenFiles(genPath, funcPath);

	Windows.reserve(10);
	Buttons.reserve(10);
	Dropdowns.reserve(10);
	Texts.reserve(10);
	TextInputs.reserve(10);
	Images.reserve(10);

	if(doc.HasMember("Elements")) {
		const auto& elements = doc["Elements"];
		for(const auto& element : elements.GetArray()) {
			LoadElement(this, element);
			CompileElement(genPath, funcPath, element);
		}
	}

	CompleteFiles(genPath);
}

void UIPage::Reload() {
	namespace fs = std::filesystem;
	auto name = fs::path(m_Path).filename();
	auto genPath = "Magma/projects/Project/gen/" + name.string();

	m_GenFile = CreateRef<DLL>(genPath + ".dll");
	auto func = m_GenFile->GetFunction<void>("LoadElements");
	func();
}

void UIPage::Render() {
	for(auto* element : GetFirstOrderElements())
		element->Render();
}

void UIPage::OnEvent(const UIState& state, const std::string& id) {
	auto func = m_GenFile->GetFunction<UIClickable*, std::string>("GetElement");
	auto* element = func(id);
	if(state.Clicked)
		element->OnClick();
	if(state.Hovered)
		element->OnHover();
	if(state.MouseUp)
		element->OnMouseUp();
	if(state.MouseDown)
		element->OnMouseDown();
}

UINode UIPage::Add(UIElement::Type type, const std::string& id) {
	switch(type) {
		case UIElement::Type::Window:
			Windows.emplace_back(id, this);
			return { type, Windows.size() };
		case UIElement::Type::Button:
			Buttons.emplace_back(id, this);
			return { type, Buttons.size() };
		case UIElement::Type::Dropdown:
			Dropdowns.emplace_back(id, this);
			return { type, Dropdowns.size() };
		case UIElement::Type::Text:
			Texts.emplace_back(id, this);
			return { type, Texts.size() };
		case UIElement::Type::TextInput:
			TextInputs.emplace_back(id, this);
			return { type, TextInputs.size() };
		case UIElement::Type::Image:
			Images.emplace_back(id, this);
			return { type, Images.size() };
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

void CompileElement(const std::string& genPath, const std::string& funcPath,
					const rapidjson::Value& docElement)
{
	if(!docElement.HasMember("OnClick")
	&& !docElement.HasMember("OnHover")
	&& !docElement.HasMember("OnMouseUp")
	&& !docElement.HasMember("OnMouseDown"))
		return;

	auto hFile = File(genPath + ".h");
	auto cppFile = File(genPath + ".cpp");
	auto funcFile = File(funcPath);

	std::string id = docElement["ID"].Get<std::string>();
	cppFile.Write("m_Elements[\"" + id + "\"] = new " + id + ";");
	hFile
	.Write("class " + id + " : public UIClickable {")
		.Write("public:");

	std::string funcFileStr = funcFile.Get();
	VOLCANICORE_LOG_INFO(id.c_str());
	auto elementIdx = funcFileStr.find_first_of(id);

	for(auto func : { "OnClick", "OnHover", "OnMouseUp", "OnMouseDown" }) {
		std::string name = func;
		if(!docElement.HasMember(name))
			continue;

		const auto& element = docElement[name];

		hFile.Write("void " + name + "() override {");
		hFile.Write("UIClickable::" + name + "();");

		if(element.IsObject()) {
			// Animation
		}
		if(element.IsString()) {
			auto string = element.Get<std::string>();
			if(string.substr(0, 4) == "@cpp") {
				uint32_t left = string.find_first_of("{");
				uint32_t right = string.find_last_of("}");
				hFile.Write(string.substr(left + 2, right - 6));
			}
			else if(string == "Default") {
				uint32_t funcIdx = funcFileStr.find_first_of(name, elementIdx);
				uint32_t start = funcFileStr.find_first_of("{", funcIdx);
				uint32_t left = start;
				uint32_t right = start;
				uint32_t max = left + 1;
				List<uint32_t> scopeStack;
				scopeStack.push_back(left);

				// while(scopeStack.size()) {
				// 	left = funcFileStr.find_first_of("{", max);
				// 	right = funcFileStr.find_first_of("}", max);

				// 	VOLCANICORE_LOG_INFO("Size %i", scopeStack.size());
				// 	VOLCANICORE_LOG_INFO("Left %i", left);
				// 	VOLCANICORE_LOG_INFO("Right %i", right);

				// 	if(left < right) {
				// 		scopeStack.push_back(left);
				// 		max = scopeStack.back() + 1;
				// 	}
				// 	else {
				// 		scopeStack.pop_back();
				// 		max = right + 1;
				// 	}
				// }

				hFile.Write(funcFileStr.substr(elementIdx + 1, 40));
			}
		}

		hFile.Write("}");
	}

	hFile.Write("};");
}

void GenFiles(const std::string& genPath, const std::string& funcPath) {
	FileUtils::CreateFile(genPath + ".h");
	FileUtils::CreateFile(genPath + ".cpp");

	auto hFile = File(genPath + ".h");
	auto cppFile = File(genPath + ".cpp");

	hFile
	.Write("#pragma once")
	.Write("#include <Magma/UI/UI.h>")
	.Write("#include <Magma/UI/UIClickable.h>")
	.Write("#include <VolcaniCore/Core/Defines.h>")
	.Write("using namespace VolcaniCore;")
	.Write("using namespace Magma::UI;")
	.Write("namespace UIElements {");

	auto str = 
	"#include \"" + hFile.Path + "\"\n"
	"namespace UIElements {\n"
	"static Map<std::string, UIClickable*> m_Elements;\n"
	"extern \"C\" EXPORT UIClickable* GetElement(const std::string& id) {\n"
		"return m_Elements[id];\n"
	"}\n"
	"extern \"C\" EXPORT void LoadElements() {";

	cppFile.Write(str);
}

void CompleteFiles(const std::string& genPath) {
	auto hFile = File(genPath + ".h");
	auto cppFile = File(genPath + ".cpp");

	cppFile.Write("}"); // LoadElements
	cppFile.Write("}"); // namespace UIElements

	hFile.Write("}"); // namespace UIElements
}

}