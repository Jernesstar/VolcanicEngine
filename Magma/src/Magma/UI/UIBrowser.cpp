#include "UIBrowser.h"

#include <filesystem>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/FileUtils.h>

#include "Core/JSONSerializer.h"

namespace Magma::UI {

static List<UIPage> s_Pages;
static UIPage* s_CurrentPage;
static Map<UIElementType, ThemeElement> s_Theme;

static void SetTheme(const std::string& path);

void UIBrowser::OnUpdate(TimeStep ts) {
	for(auto& page : s_Pages)
		page.Update(ts);
}

void UIBrowser::OnRender() {
	s_CurrentPage->Render();
}

void UIBrowser::Load(const std::string& folderPath) {
	namespace fs = std::filesystem;

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
			s_Pages.emplace_back(filePathName.string());
	}

	s_CurrentPage = &s_Pages[0];
}

void UIBrowser::Reload() {
	for(auto& page : s_Pages)
		page.Reload();
}

void UIBrowser::SetPage(const std::string& name) {
	s_CurrentPage = GetPage(name);

	// if(s_CurrentPage) {
	// 	namespace fs = std::filesystem;
	// 	fs::path p(s_CurrentPage->GetPath());
	// 	auto path = p.parent_path() / name;

	// 	s_CurrentPage = &s_Pages.emplace_back(path.string() + ".magma.ui.json");
	// }
}

UIPage* UIBrowser::GetPage(const std::string& name) {
	if(name == "")
		return nullptr;

	for(auto& page : s_Pages)
		if(page.GetName() == name)
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

}