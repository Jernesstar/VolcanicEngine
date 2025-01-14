#include "UILoader.h"

#include <fstream>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/Algo.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/JSONSerializer.h>

namespace fs = std::filesystem;

namespace Lava {

static void LoadElement(UIPage& page, const rapidjson::Value& elementNode);

void UILoader::Load(UIPage& page, const std::string& path) {
	using namespace rapidjson;

	if(path == "")
		return;

	std::string name;
	std::string jsonPath;
	fs::path filePathName = path;
	if(fs::path(filePathName).has_extension()) {
		name = filePathName.stem().stem().stem().string();
		jsonPath = filePathName.string() + ".magma.ui.json";
	}
	else {
		name = filePathName.stem().string();
		jsonPath = path;
	}

	if(!FileUtils::FileExists(jsonPath)) {
		VOLCANICORE_LOG_ERROR(
			"Could not find .magma.ui.json file with name %s",
			filePathName.string().c_str());
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

	if(doc.HasMember("Theme"))
		page.SetTheme(LoadTheme(jsonPath));

	if(doc.HasMember("Elements")) {
		const auto& elements = doc["Elements"];
		for(const auto& element : elements.GetArray())
			LoadElement(page, element);
	}
}

template<typename TUIElement>
static void Serialize(TUIElement* ui, JSONSerializer& serializer);

template<>
void Serialize(Window* ui, JSONSerializer& serializer) {
	serializer
		.WriteKey("BorderWidth").Write(ui->BorderWidth)
		.WriteKey("BorderHeight").Write(ui->BorderHeight)
		.WriteKey("BorderColor").Write(ui->BorderColor);
}

template<>
void Serialize(Button* ui, JSONSerializer& serializer) {

}

template<>
void Serialize(Dropdown* ui, JSONSerializer& serializer) {

}

template<>
void Serialize(Text* ui, JSONSerializer& serializer) {
	serializer
		.WriteKey("Text").Write(ui->Content);
}

template<>
void Serialize(TextInput* ui, JSONSerializer& serializer) {
	serializer
		.WriteKey("Text").Write(ui->Text);
}

template<>
void Serialize(Image* ui, JSONSerializer& serializer) {
	serializer.WriteKey("Path").Write(ui->Content->GetPath());
}

template<>
void Serialize(UIElement* ui, JSONSerializer& serializer) {
	serializer
	.BeginMapping()
		.WriteKey("Type");

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
					}
				}())
		.WriteKey("Color").Write(ui->Color);

	switch(ui->GetType()) {
		case UIElementType::Window:
			Serialize(ui->As<Window>(), serializer);
			break;
		case UIElementType::Button:
			Serialize(ui->As<Button>(), serializer);
			break;
		case UIElementType::Dropdown:
			Serialize(ui->As<Dropdown>(), serializer);
			break;
		case UIElementType::Text:
			Serialize(ui->As<Text>(), serializer);
			break;
		case UIElementType::TextInput:
			Serialize(ui->As<TextInput>(), serializer);
			break;
		case UIElementType::Image:
			Serialize(ui->As<Image>(), serializer);
			break;
	}

	serializer.WriteKey("Children").BeginSequence();
	for(auto child : ui->GetChildren())
		Serialize(child, serializer);
	serializer.EndSequence();

	serializer.EndMapping();
}

void UILoader::Save(const UIPage& page, const std::string& path) {
	JSONSerializer serializer;
	serializer.BeginMapping(); // File

	// TODO(Implement): Serialize theme

	serializer.WriteKey("Elements")
	.BeginSequence();

	for(UIElement* element : page.GetFirstOrderElements())
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
	if(!doc.HasMember("Theme"))
		return res;

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

static void GenFiles(const std::string& name, const std::string& funcPath);
static void CompleteFiles(const std::string& name);
static void CompileElement(const std::string& name, const std::string& funcPath,
						   const rapidjson::Value& elementNode);

void UILoader::Compile(const std::string& filePathName) {
	using namespace rapidjson;

	if(filePathName == "") {
		VOLCANICORE_LOG_ERROR("Filename was empty");
		return;
	}

	auto name = fs::path(filePathName).stem().string();
	auto projPath = fs::path(filePathName)
					.parent_path().parent_path().parent_path().string();
	auto jsonPath = filePathName + ".magma.ui.json";
	auto funcPath = filePathName + ".magma.ui.func";
	if(!FileUtils::FileExists(jsonPath)) {
		VOLCANICORE_LOG_ERROR(
			"Could not find .magma.ui.json file with name %s",
			filePathName.c_str());
		return;
	}

	std::string file = FileUtils::ReadFile(jsonPath);
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
		GenFiles(name, funcPath);

		const auto& elements = doc["Elements"];
		for(const auto& element : elements.GetArray())
			CompileElement(name, funcPath, element);

		CompleteFiles(name);
	}
}

Ref<DLL> UILoader::GetDLL(const std::string& pageName) {
	std::string path = "Lava/projects/UI/build/lib/";
#ifdef VOLCANICENGINE_WINDOWS
	path += pageName + ".dll";
#elif VOLCANICENGINE_LINUX
	path = "./" + path + "lib" + pageName + ".so";
#endif
	return CreateRef<DLL>(path);
}

template<typename T>
T TryGet(const rapidjson::Value& node, const std::string& name, const T& alt)
{
	if(node.HasMember(name))
		return node[name].Get<T>();
	return alt;
}

void LoadElement(UIPage& page, const rapidjson::Value& elementNode) {
	std::string id = elementNode["ID"].GetString();
	std::string typeStr = elementNode["Type"].GetString();
	std::string parent = "";

	if(elementNode.HasMember("Parent"))
		parent = elementNode["Parent"].Get<std::string>();

	UINode node;
	UIElement* element;
	ThemeElement* theme;

	if(typeStr == "Window") {
		node = page.Add(UIElementType::Window, id);
		element = page.Get(node);
		theme = &page.GetTheme()[element->GetType()];
		auto* window = element->As<Window>();

		if(elementNode.HasMember("Border")) {
			const auto& borderNode = elementNode["Border"];
			window->BorderWidth
				= TryGet<uint32_t>(borderNode, "Width", theme->BorderWidth);
			window->BorderHeight
				= TryGet<uint32_t>(borderNode, "Height", theme->BorderHeight);

			window->BorderColor = theme->BorderColor;
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
		theme = &page.GetTheme()[element->GetType()];

		auto* button = element->As<Button>();

		if(elementNode.HasMember("Image"))
			button->Display
				= CreateRef<Image>(elementNode["Image"].Get<std::string>());
		else if(elementNode.HasMember("Text"))
			button->Display
				= CreateRef<Text>(elementNode["Text"].Get<std::string>());
		else
			button->Display = CreateRef<Text>(button->GetID());
	}
	if(typeStr == "Dropdown") {
		node = page.Add(UIElementType::Dropdown, id);
		element = page.Get(node);
		theme = &page.GetTheme()[element->GetType()];
	}
	if(typeStr == "Text") {
		node = page.Add(UIElementType::Text, id);
		element = page.Get(node);
		theme = &page.GetTheme()[element->GetType()];
	}
	if(typeStr == "TextInput") {
		node = page.Add(UIElementType::TextInput, id);
		element = page.Get(node);
		theme = &page.GetTheme()[element->GetType()];
	}
	if(typeStr == "Image") {
		node = page.Add(UIElementType::Image, id);
		element = page.Get(node);
		theme = &page.GetTheme()[element->GetType()];
	}

	if(parent == "")
		page.Add(node);
	else
		page.Get(parent)->Add(node);

	element->Width = TryGet<uint32_t>(elementNode, "Width", theme->Width);
	element->Height = TryGet<uint32_t>(elementNode, "Height", theme->Height);

	element->x = TryGet<int32_t>(elementNode, "x", theme->x);
	element->y = TryGet<int32_t>(elementNode, "y", theme->y);

	element->xAlignment = theme->xAlignment;
	if(elementNode.HasMember("xAlignment")) {
		std::string xAlign = elementNode["xAlignment"].Get<std::string>();
		if(xAlign == "Left")
			element->xAlignment = XAlignment::Left;
		else if(xAlign == "Center")
			element->xAlignment = XAlignment::Center;
		else if(xAlign == "Right")
			element->xAlignment = XAlignment::Right;
	}

	element->yAlignment = theme->yAlignment;
	if(elementNode.HasMember("yAlignment")) {
		std::string xAlign = elementNode["yAlignment"].Get<std::string>();
		if(xAlign == "Top")
			element->yAlignment = YAlignment::Top;
		else if(xAlign == "Center")
			element->yAlignment = YAlignment::Center;
		else if(xAlign == "Bottom")
			element->yAlignment = YAlignment::Bottom;
	}

	element->Color = theme->Color;
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

void CompileElement(const std::string& name, const std::string& funcPath,
					const rapidjson::Value& elementNode)
{
	if(!elementNode.HasMember("OnUpdate")
	&& !elementNode.HasMember("OnClick")
	&& !elementNode.HasMember("OnHover")
	&& !elementNode.HasMember("OnMouseUp")
	&& !elementNode.HasMember("OnMouseDown"))
		return;

	auto genPath = fs::path("Lava") / "projects" / "UI" / "gen";
	auto hFile = File((genPath / name).string() + ".h");
	auto cppFile = File((genPath / name).string() + ".cpp");

	std::string funcFileStr = "";
	if(FileUtils::FileExists(funcPath))
		funcFileStr = File(funcPath).Get();

	std::string id = elementNode["ID"].Get<std::string>();

	cppFile.Write("\tm_Objects[\"" + id + "\"] = new " + id + ";");
	cppFile.Write("\tm_Objects[\"" + id + "\"]->SetPage(page);");

	hFile
	.Write("class " + id + " : public UIObject {")
	.Write("public:");

	for(std::string name :
		{ "OnUpdate", "OnClick", "OnHover", "OnMouseUp", "OnMouseDown" })
	{
		if(!elementNode.HasMember(name))
			continue;

		const auto& element = elementNode[name];

		hFile.Write("\tvoid " + name + "(" +
					(name == "OnUpdate" ? "TimeStep ts" : "") + ") override {");
		hFile.Write("\t\tUIObject::" + name + "(" +
					(name == "OnUpdate" ? "ts" : "") + ");");

		if(element.IsObject()) {
			// TODO(Implement): Animation
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
			else if(string == "Default" && funcFileStr != "") {
				auto elementIdx = funcFileStr.find(id);
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

void GenFiles(const std::string& name, const std::string& funcPath) {
	auto genPath = fs::path("Lava") / "projects" / "UI" / "gen";

	auto hFile = File((genPath / name).string() + ".h");
	auto cppFile = File((genPath / name).string() + ".cpp");
	auto templateFile = File("Lava/projects/UI/template.lua");
	auto premakeFile = File("Lava/projects/UI/pages.lua");
	auto contextIncludes = File("Lava/projects/UI/gen/Context.h");

	if(FileUtils::FileExists(funcPath)) {
		auto funcFileStr = File(funcPath).Get();
		uint64_t elementsIdx = funcFileStr.find("namespace UIObjects");
		std::string includes = funcFileStr.substr(0, elementsIdx - 1);
		contextIncludes.Write(includes);
	}

	hFile
	.Write("#include \"Context.h\"")
	.Write("\n")
	.Write("namespace UIObjects {\n");

	cppFile
	.Write("#include \"" + fs::path(hFile.Path).filename().string() + "\"\n")
	.Write("namespace UIObjects {\n")
	.Write("static Map<std::string, UIObject*> m_Objects;\n")
	.Write("extern \"C\" EXPORT UIObject* GetObject(const std::string& id) {")
	.Write("\tif(!m_Objects.count(id)) { return nullptr; }")
	.Write("\treturn m_Objects[id];")
	.Write("}\n")
	.Write("extern \"C\" EXPORT void LoadObjects(UIPage* page) {");

	std::string templateStr = templateFile.Get();
	Replace(templateStr, "{name}", name);
	premakeFile.Write(templateStr);
}

void CompleteFiles(const std::string& name) {
	auto genPath = fs::path("Lava") / "projects" / "UI" / "gen";
	auto hFile = File((genPath / name).string() + ".h");
	auto cppFile = File((genPath / name).string() + ".cpp");

	cppFile.Write("}"); // LoadObjects
	cppFile.Write("\n}"); // namespace UIElements

	hFile.Write("\n}"); // namespace UIElements
}

}