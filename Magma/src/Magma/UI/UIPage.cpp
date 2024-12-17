#include "UIPage.h"

#include <filesystem>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/FileUtils.h>

#include "UI.h"
#include "UIObject.h"

#define GET_LIST(TUIElement) \
template<> \
List<TUIElement>& UIPage::GetList<TUIElement>() { \
	return TUIElement##s; \
}

#define GET_TYPE(TUIElement) \
template<> \
UIElementType UIPage::GetType<TUIElement>() { \
	return UIElementType::TUIElement; \
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
	if(filePathName == "")
		return;

	auto jsonPath = filePathName + ".magma.ui.json";
	auto funcPath = filePathName + ".magma.ui.func";
	if(!FileUtils::FileExists(jsonPath)) {
		VOLCANICORE_LOG_INFO(
			"Could not find .magma.ui.json file with name %s",
			filePathName.c_str());
		return;
	}

	using namespace rapidjson;
	namespace fs = std::filesystem;
	m_Path = filePathName;
	m_Name = fs::path(m_Path).filename().string();

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

	if(doc.HasMember("Elements")) {
		auto genPath = "Magma/projects/UI/gen/" + m_Name;
		GenFiles(genPath, funcPath);

		const auto& elements = doc["Elements"];
		for(const auto& element : elements.GetArray()) {
			LoadElement(this, element);
			CompileElement(genPath, funcPath, element);
		}

		CompleteFiles(genPath);
	}
}

void UIPage::Reload() {
	std::string buildPath = "Magma/projects/UI/build/bin/";

	m_GenFile = CreateRef<DLL>(buildPath + "UI.dll");
	auto load = m_GenFile->GetFunction<void>("LoadObjects");
	load();
}

void UIPage::Update(TimeStep ts) {
	for(auto* element : GetFirstOrderElements())
		UpdateElement(element, ts);
}

void UIPage::UpdateElement(UIElement* element, TimeStep ts) {
	auto get = m_GenFile->GetFunction<UIObject*, std::string>("GetObject");
	auto* object = get(element->GetID());

	if(!object)
		return;

	object->OnUpdate(ts);

	for(auto* child : element->GetChildren())
		UpdateElement(child, ts);
}

void UIPage::Render() {
	for(auto* element : GetFirstOrderElements())
		element->Render();
}

void UIPage::OnEvent(const std::string& id, const UIState& state) {
	auto get = m_GenFile->GetFunction<UIObject*, std::string>("GetObject");
	auto* object = get(id);

	if(!object)
		return;

	if(state.Clicked)
		object->OnClick();
	if(state.Hovered)
		object->OnHover();
	if(state.MouseUp)
		object->OnMouseUp();
	if(state.MouseDown)
		object->OnMouseDown();
}

UINode UIPage::Add(UIElementType type, const std::string& id) {
	switch(type) {
		case UIElementType::Window:
			Windows.emplace_back(id, this);
			return { type, Windows.size() - 1 };
		case UIElementType::Button:
			Buttons.emplace_back(id, this);
			return { type, Buttons.size() - 1 };
		case UIElementType::Dropdown:
			Dropdowns.emplace_back(id, this);
			return { type, Dropdowns.size() - 1 };
		case UIElementType::Text:
			Texts.emplace_back(id, this);
			return { type, Texts.size() - 1 };
		case UIElementType::TextInput:
			TextInputs.emplace_back(id, this);
			return { type, TextInputs.size() - 1 };
		case UIElementType::Image:
			Images.emplace_back(id, this);
			return { type, Images.size() - 1 };
	}

	return { UIElementType::Window, Windows.size() };
}

UIElement* UIPage::Get(const UINode& node) const {
	// TODO(Fix): The pointers might become invalid if the map reallocates
	switch(node.first) {
		case UIElementType::Window:
			return (UIElement*)&Windows[node.second];
		case UIElementType::Button:
			return (UIElement*)&Buttons[node.second];
		case UIElementType::Dropdown:
			return (UIElement*)&Dropdowns[node.second];
		case UIElementType::Text:
			return (UIElement*)&Texts[node.second];
		case UIElementType::TextInput:
			return (UIElement*)&TextInputs[node.second];
		case UIElementType::Image:
			return (UIElement*)&Images[node.second];
	}

	return nullptr;
}

void UIPage::Clear() {
	Windows.clear();
	Buttons.clear();
	Dropdowns.clear();
	Texts.clear();
	TextInputs.clear();
	Images.clear();
}

UIElement* UIPage::Get(const std::string& id) const {
	for(auto& element : Windows)
		if(element.GetID() == id)
			return (UIElement*)&element;
	for(auto& element : Buttons)
		if(element.GetID() == id)
			return (UIElement*)&element;
	for(auto& element : Dropdowns)
		if(element.GetID() == id)
			return (UIElement*)&element;
	for(auto& element : Texts)
		if(element.GetID() == id)
			return (UIElement*)&element;
	for(auto& element : TextInputs)
		if(element.GetID() == id)
			return (UIElement*)&element;
	for(auto& element : Images)
		if(element.GetID() == id)
			return (UIElement*)&element;

	return nullptr;
}

List<UIElement*> UIPage::GetFirstOrderElements() const {
	List<UIElement*> res;
	for(auto node : m_FirstOrders)
		res.push_back(Get(node));

	return res;
}

void LoadElement(UIPage* page, const rapidjson::Value& docElement) {
	std::string id = docElement["ID"].GetString();
	std::string typeStr = docElement["Type"].GetString();
	std::string parent = "";

	if(docElement.HasMember("Parent"))
		parent = docElement["Parent"].Get<std::string>();

	UINode node;
	UIElement* element;

	if(typeStr == "Window") {
		node = page->Add(UIElementType::Window, id);
		element = page->Get(node);
	}
	if(typeStr == "Button") {
		node = page->Add(UIElementType::Button, id);
		element = page->Get(node);
		auto* button = element->As<Button>();

		if(docElement.HasMember("Image"))
			button->Display
				= CreateRef<Image>(docElement["Image"].Get<std::string>());
		else if(docElement.HasMember("Text"))
			button->Display
				= CreateRef<Text>(docElement["Text"].Get<std::string>());
		else
			button->Display = CreateRef<Text>(button->GetID());
	}
	if(typeStr == "Dropdown") {
		node = page->Add(UIElementType::Dropdown, id);
		element = page->Get(node);
	}
	if(typeStr == "Text") {
		node = page->Add(UIElementType::Text, id);
		element = page->Get(node);
	}
	if(typeStr == "TextInput") {
		node = page->Add(UIElementType::TextInput, id);
		element = page->Get(node);
	}
	if(typeStr == "Image") {
		node = page->Add(UIElementType::Image, id);
		element = page->Get(node);
	}

	if(parent == "")
		page->Add(node);
	else
		page->Get(parent)->Add(node);

	// TODO(Implement): Element alignement
	auto width = docElement["Width"].Get<uint32_t>();
	auto height = docElement["Height"].Get<uint32_t>();
	auto x = docElement["x"].Get<uint32_t>();
	auto y = docElement["y"].Get<uint32_t>();
	const auto& array = docElement["Color"];
	auto color =
		glm::vec4
		{
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
	cppFile.Write("\tm_Objects[\"" + id + "\"] = new " + id + ";");
	hFile
	.Write("class " + id + " : public UIObject {")
	.Write("public:");

	std::string funcFileStr = funcFile.Get();
	auto elementIdx = funcFileStr.find(id);

	for(std::string name : { "OnClick", "OnHover", "OnMouseUp", "OnMouseDown" })
	{
		if(!docElement.HasMember(name))
			continue;

		const auto& element = docElement[name];

		hFile.Write("\tvoid " + name + "() override {");
		hFile.Write("\t\tUIObject::" + name + "();");

		if(element.IsObject()) {
			// Animation
		}
		if(element.IsString()) {
			auto string = element.Get<std::string>();

			if(string.substr(0, 4) == "@cpp") {
				uint32_t left = string.find_first_of('{');
				uint32_t right = string.find_last_of('}');
				if(string.find_first_not_of(' ') != right)
					hFile.Write("\t\t" +
								string.substr(left + 2, right - left - 3));
			}
			else if(string == "Default") {
				uint32_t funcIdx = funcFileStr.find(name, elementIdx);
				uint32_t start = funcFileStr.find_first_of('{', funcIdx);
				uint32_t left = start;
				uint32_t right = start;
				uint32_t max = left + 1;
				List<uint32_t> scopeStack;
				scopeStack.push_back(left);

				while(scopeStack.size()) {
					left = funcFileStr.find_first_of('{', max);
					right = funcFileStr.find_first_of('}', max);

					if(left < right) {
						scopeStack.push_back(left);
						max = scopeStack.back() + 1;
					}
					else {
						scopeStack.pop_back();
						max = right + 1;
					}
				}

				hFile.Write(funcFileStr.substr(start + 2, max - start - 5));
			}
		}

		hFile.Write("\t}");
	}

	hFile.Write("};");
}

void GenFiles(const std::string& genPath, const std::string& funcPath) {
	namespace fs = std::filesystem;

	FileUtils::CreateFile(genPath + ".h");
	FileUtils::CreateFile(genPath + ".cpp");

	auto hFile = File(genPath + ".h");
	auto cppFile = File(genPath + ".cpp");
	auto funcFile = File(funcPath);
	auto funcFileStr = funcFile.Get();
	auto elementsIdx = funcFileStr.find("namespace UIObjects");

	hFile
	.Write(funcFileStr.substr(0, elementsIdx - 1))
	.Write("#include <Magma/UI/UI.h>")
	.Write("#include <VolcaniCore/Core/Log.h>")
	.Write("")
	.Write("namespace UIObjects {\n");

	cppFile
	.Write("#include \"" + fs::path(hFile.Path).filename().string() + "\"\n")
	.Write("namespace UIObjects {\n")
	.Write("static Map<std::string, UIObject*> m_Objects;\n")
	.Write("extern \"C\" EXPORT UIObject* GetObject(const std::string& id) {")
	.Write("\tif(!m_Objects.count(id)) { return nullptr; }")
	.Write("\treturn m_Objects[id];")
	.Write("}\n")
	.Write("extern \"C\" EXPORT void LoadObjects() {");
}

void CompleteFiles(const std::string& genPath) {
	auto hFile = File(genPath + ".h");
	auto cppFile = File(genPath + ".cpp");

	cppFile.Write("}"); // LoadObjects
	cppFile.Write("\n}"); // namespace UIElements

	hFile.Write("\n}"); // namespace UIElements
}

}