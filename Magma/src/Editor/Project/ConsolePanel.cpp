#include "ConsolePanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/UI/UIRenderer.h>

#include "Editor/AssetImporter.h"

namespace fs = std::filesystem;

using namespace VolcaniCore;
using namespace Magma::UI;

namespace Magma {

ConsolePanel::ConsolePanel()
	: Panel("Console")
{
	Application::PushDir();
	m_Info.Content = AssetImporter::GetTexture("Magma/assets/icons/Info.png");
	m_Warn.Content = AssetImporter::GetTexture("Magma/assets/icons/Warn.png");
	m_Error.Content = AssetImporter::GetTexture("Magma/assets/icons/Error.png");

	m_Info.SetSize(35, 35);
	m_Warn.SetSize(35, 35);
	m_Error.SetSize(35, 35);
	Application::PopDir();

	Log(ConsoleLevel::Info, "The console works");
}

void ConsolePanel::Log(ConsoleLevel level, const std::string& message) {
	m_Log.Add({ level, message });
}

void ConsolePanel::Update(TimeStep ts) {

}

void ConsolePanel::Draw() {
	auto windowFlags = ImGuiWindowFlags_NoScrollbar
					 | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::Begin("Console", &Open, windowFlags);

	static ConsoleType s_Type = ConsoleType::Log;

	ImVec2 size = ImGui::GetContentRegionAvail();
	size.y -= 21.0f;
	ImGui::BeginChild("##Window", size, ImGuiChildFlags_Border);

	if(s_Type == ConsoleType::Log) {
		UI::Image* image = nullptr;
		for(auto& [level, message] : m_Log) {
			if(level == ConsoleLevel::Info)
				image = &m_Info;
			else if(level == ConsoleLevel::Warn)
				image = &m_Warn;
			else
				image = &m_Error;

			image->Render();
			ImGui::SameLine();
			ImGui::Text(message.c_str());
		}
	}
	else if(s_Type == ConsoleType::Debug) {
		ImGui::Text("Debug");

	}
	ImGui::EndChild();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

	auto tabBarFlags = ImGuiTabBarFlags_Reorderable
					 | ImGuiTabBarFlags_AutoSelectNewTabs;

	if(ImGui::BeginTabBar("ConsoleTabs", tabBarFlags))
	{

		TabState s1 = UIRenderer::DrawTab("Log", false);
		TabState s2 = UIRenderer::DrawTab("Debug", false);
		if(s1.Clicked)
			s_Type = ConsoleType::Log;
		if(s2.Clicked)
			s_Type = ConsoleType::Debug;

		ImGui::EndTabBar();
	}
	ImGui::PopStyleVar();

	ImGui::End();
}

}