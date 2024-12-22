#include "UIBrowser.h"

#include <filesystem>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/DLL.h>
#include <Magma/Core/JSONSerializer.h>

#include "UIObject.h"

using namespace Magma;
using namespace Magma::UI;

namespace Lava {

static std::string s_UIFolderPath;
static Map<std::string, Ref<DLL>> s_DLLs;
static List<UIPage> s_Pages;
static UIPage* s_CurrentPage;
static Map<UIElementType, ThemeElement> s_Theme;

static void SetTheme(const std::string& path);
static void LoadPage(const std::string& name);

void UIBrowser::Load(const std::string& folderPath) {
	namespace fs = std::filesystem;

	s_UIFolderPath = folderPath;

	auto themePath = (fs::path(folderPath) / "theme.magma.ui.json").string();
	if(FileUtils::FileExists(themePath))
		SetTheme(themePath);

	for(auto filePath : FileUtils::GetFiles(folderPath, { ".json" })) {
		fs::path p(filePath);
		auto name = p.stem().stem().stem();
		auto filePathName = folderPath / name;

		if(name.string() == "theme")
			continue;
		else
			LoadPage(name.string());
	}

	s_CurrentPage = &s_Pages[0];
}

void UIBrowser::Reload() {
	for(auto& page : s_Pages) {
		std::string buildPath = "Magma/projects/UI/build/bin/";

		s_DLLs[page.Name] = CreateRef<DLL>(buildPath + "UI.dll");
		auto load = s_DLLs[page.Name]->GetFunction<void>("LoadObjects");
		load();
	}
}

void UIBrowser::OnUpdate(TimeStep ts) {
	for(auto& page : s_Pages) {
		auto gen = s_DLLs[page.Name];
		auto get = gen->GetFunction<UIObject*, std::string>("GetObject");

		page.Traverse(
			[&](UIElement* element)
			{
				UIObject* object = get(element->GetID());
				if(!object)
					return;

				object->OnUpdate(ts);

				UIState state = element->GetState();
				if(state.Clicked)
					object->OnClick();
				if(state.Hovered)
					object->OnHover();
				if(state.MouseUp)
					object->OnMouseUp();
				if(state.MouseDown)
					object->OnMouseDown();
			});
	}

	if(s_CurrentPage)
		s_CurrentPage->Render();
}

void UIBrowser::SetPage(const std::string& name) {
	s_CurrentPage = GetPage(name);
}

UIPage* UIBrowser::GetPage(const std::string& name) {
	if(name == "")
		return nullptr;

	for(auto& page : s_Pages)
		if(page.Name == name)
			return &page;

	return nullptr;
}

void SetTheme(const std::string& path) {
	std::string file = FileUtils::ReadFile(path);
	Document doc;
	doc.Parse(file);

	const auto& themeDocElement = doc["Theme"];

	if(themeDocElement.HasMember("Window")) {
		const auto& windowTheme = themeDocElement["Window"];
		s_Theme[UIElementType::Window] = ThemeElement();
		auto& theme = s_Theme[UIElementType::Window];

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

}

static void GenFiles(const std::string& genPath, const std::string& funcPath);
static void CompleteFiles(const std::string& genPath);
static void LoadElement(UIPage& page, const rapidjson::Value& docElement);
static void CompileElement(const std::string& genPath,
						   const std::string& funcPath,
						   const rapidjson::Value& docElement);

void LoadPage(const std::string& name) {
	using namespace rapidjson;
	namespace fs = std::filesystem;

	if(name == "")
		return;

	auto folder = fs::path(s_UIFolderPath);
	auto pathName = (folder / name).string();
	auto jsonPath = pathName + ".magma.ui.json";
	auto funcPath = pathName + ".magma.ui.func";
	if(!FileUtils::FileExists(jsonPath)) {
		VOLCANICORE_LOG_ERROR(
			"Could not find .magma.ui.json file with name %s", name.c_str());
		return;
	}

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

	UIPage& page = s_Pages.emplace_back(name);

	if(doc.HasMember("Elements")) {
		auto genPath = fs::path("Magma") / "projects" / "UI" / "gen" / name;
		GenFiles(genPath.string(), funcPath);

		const auto& elements = doc["Elements"];
		for(const auto& element : elements.GetArray()) {
			LoadElement(page, element);
			CompileElement(genPath.string(), funcPath, element);
		}

		CompleteFiles(genPath.string());
	}
}

void LoadElement(UIPage& page, const rapidjson::Value& docElement) {
	std::string id = docElement["ID"].GetString();
	std::string typeStr = docElement["Type"].GetString();
	std::string parent = "";

	if(docElement.HasMember("Parent"))
		parent = docElement["Parent"].Get<std::string>();

	UINode node;
	UIElement* element;

	if(typeStr == "Window") {
		node = page.Add(UIElementType::Window, id);
		element = page.Get(node);
	}
	if(typeStr == "Button") {
		node = page.Add(UIElementType::Button, id);
		element = page.Get(node);
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
		node = page.Add(UIElementType::Dropdown, id);
		element = page.Get(node);
	}
	if(typeStr == "Text") {
		node = page.Add(UIElementType::Text, id);
		element = page.Get(node);
	}
	if(typeStr == "TextInput") {
		node = page.Add(UIElementType::TextInput, id);
		element = page.Get(node);
	}
	if(typeStr == "Image") {
		node = page.Add(UIElementType::Image, id);
		element = page.Get(node);
	}

	if(parent == "")
		page.Add(node);
	else
		page.Get(parent)->Add(node);

	// TODO(Implement): Element alignement
	element->Width = docElement["Width"].Get<uint32_t>();
	element->Height = docElement["Height"].Get<uint32_t>();
	element->xAlignment = XAlignment::Left;
	if(docElement.HasMember("xAlignment")) {
		const auto& xAlign = docElement["xAlignment"];
		if(xAlign.Get<std::string>() == "Center")
			element->xAlignment = XAlignment::Center;
		else if(xAlign.Get<std::string>() == "Right")
			element->xAlignment = XAlignment::Right;
	}
	element->yAlignment = YAlignment::Top;
	if(docElement.HasMember("yAlignment")) {
		const auto& yAlign = docElement["yAlignment"];
		if(yAlign.Get<std::string>() == "Center")
			element->yAlignment = YAlignment::Center;
		else if(yAlign.Get<std::string>() == "Bottom")
			element->yAlignment = YAlignment::Bottom;
	}
	element->x = docElement["x"].Get<int32_t>();
	element->y = docElement["y"].Get<int32_t>();
	element->Color =
		glm::vec4
		{
			docElement["Color"][0].Get<float>(),
			docElement["Color"][1].Get<float>(),
			docElement["Color"][2].Get<float>(),
			docElement["Color"][3].Get<float>()
		};
}

void CompileElement(const std::string& genPath, const std::string& funcPath,
					const rapidjson::Value& docElement)
{
	if(!docElement.HasMember("OnUpdate")
	&& !docElement.HasMember("OnClick")
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

	for(std::string name :
		{ "OnUpdate", "OnClick", "OnHover", "OnMouseUp", "OnMouseDown" })
	{
		if(!docElement.HasMember(name))
			continue;

		const auto& element = docElement[name];

		hFile.Write("\tvoid " + name + "(" +
					(name == "OnUpdate" ? "TimeStep ts" : "") + ") override {");
		hFile.Write("\t\tUIObject::" + name + "(" +
					(name == "OnUpdate" ? "ts" : "") + ");");

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
	std::string funcFileStr = funcFile.Get();
	uint64_t elementsIdx = funcFileStr.find("namespace UIObjects");

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