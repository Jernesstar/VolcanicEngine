#include "UIPage.h"

#include <filesystem>

#include <rapidjson/rapidjson.h>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/FileUtils.h>

#include "UIObject.h"

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
	auto jsonPath = filePathName + ".magma.ui.json";
	auto funcPath = filePathName + ".magma.ui.func";
	if(!FileUtils::FileExists(jsonPath))
		return;

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

	// Windows.reserve(20);
	// Buttons.reserve(20);
	// Dropdowns.reserve(20);
	// Texts.reserve(20);
	// TextInputs.reserve(20);
	// Images.reserve(20);

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
	auto genPath = "Magma/projects/UI/gen/" + m_Name;

	m_GenFile = CreateRef<DLL>(genPath + ".dll");
	auto func = m_GenFile->GetFunction<void>("LoadObjects");
	func();
}

void UIPage::Update(TimeStep ts) {
	for(auto* element : GetFirstOrderElements())
		UpdateElement(ts, element);
}

void UIPage::UpdateElement(TimeStep ts, UIElement* element) {
	auto get = m_GenFile->GetFunction<UIObject*, std::string>("GetObject");
	auto* object = get(element->GetID());

	object->OnUpdate(TimeStep ts);

	for(auto* child : element->GetChildren())
		UpdateElement(child);
}

void UIPage::Render() {
	for(auto* element : GetFirstOrderElements())
		element->Render();
}

void UIPage::OnEvent(const std::string& id, const UIState& state) {
	if(!m_GenFile)
		return;

	auto get = m_GenFile->GetFunction<UIObject*, std::string>("GetObject");
	auto* clickable = get(id);

	if(state.Clicked)
		clickable->OnClick();
	if(state.Hovered)
		clickable->OnHover();
	if(state.MouseUp)
		clickable->OnMouseUp();
	if(state.MouseDown)
		clickable->OnMouseDown();
}

UINode UIPage::Add(UIElement::Type type, const std::string& id) {
	switch(type) {
		case UIElement::Type::Window:
			Windows.emplace_back(id, this);
			return { type, Windows.size() - 1 };
		case UIElement::Type::Button:
			Buttons.emplace_back(id, this);
			return { type, Buttons.size() - 1 };
		case UIElement::Type::Dropdown:
			Dropdowns.emplace_back(id, this);
			return { type, Dropdowns.size() - 1 };
		case UIElement::Type::Text:
			Texts.emplace_back(id, this);
			return { type, Texts.size() - 1 };
		case UIElement::Type::TextInput:
			TextInputs.emplace_back(id, this);
			return { type, TextInputs.size() - 1 };
		case UIElement::Type::Image:
			Images.emplace_back(id, this);
			return { type, Images.size() - 1 };
	}

	return { UIElement::Type::Window, 0 };
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

	return nullptr;
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
		node = page->Add(UIElement::Type::Window, id);
		element = page->Get(node);
	}
	if(typeStr == "Button") {
		node = page->Add(UIElement::Type::Button, id);
		element = page->Get(node);
		element->As<Button>()->Display = CreateRef<Text>("Button1");
	}
	if(typeStr == "Dropdown") {
		node = page->Add(UIElement::Type::Dropdown, id);
		element = page->Get(node);
	}
	if(typeStr == "Text") {
		node = page->Add(UIElement::Type::Text, id);
		element = page->Get(node);
	}
	if(typeStr == "TextInput") {
		node = page->Add(UIElement::Type::TextInput, id);
		element = page->Get(node);
	}
	if(typeStr == "Image") {
		node = page->Add(UIElement::Type::Image, id);
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
	cppFile.Write("\tm_Elements[\"" + id + "\"] = new " + id + ";");
	hFile
	.Write("class " + id + " : public UIObject {")
	.Write("public:");

	std::string funcFileStr = funcFile.Get();
	auto elementIdx = funcFileStr.find(id);

	for(auto func : { "OnClick", "OnHover", "OnMouseUp", "OnMouseDown" }) {
		std::string name = func;
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
				hFile.Write("\t\t" + string.substr(left + 2, right - left - 3));
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

				hFile.Write("\t\t" + funcFileStr.substr(start + 1, max - start - 2));
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
	.Write(funcFileStr.substr(0, elementsIdx))
	.Write("#include <Magma/UI/UI.h>")
	.Write("namespace UIObjects {\n");

	cppFile
	.Write("#include \"" + fs::path(hFile.Path).filename().string() + "\"\n")
	.Write("namespace UIObjects {\n")
	.Write("static Map<std::string, UIObject*> m_Objects;\n")
	.Write("extern \"C\" EXPORT UIObject* GetObject(const std::string& id) {")
	.Write(	"\treturn m_Objects[id];")
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