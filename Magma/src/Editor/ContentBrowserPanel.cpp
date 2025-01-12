#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <Magma/UI/UIRenderer.h>

#include <VolcaniCore/Core/Log.h>

namespace Magma {

ContentBrowserPanel::ContentBrowserPanel(const std::string& path)
	: Panel("ContentBrowser"), m_Path(path)
{
	m_FileIcon = CreateRef<UI::Image>("Magma/assets/icons/FileIcon.png");
	m_FolderIcon = CreateRef<UI::Image>("Magma/assets/icons/FolderIcon.png");

	m_CurrentPath = m_Path;
	m_AssetPath = m_Path / "asset";
}

void ContentBrowserPanel::Update(TimeStep ts) {

}

static bool HasExtension(fs::path path, const List<std::string>& extensions) {
	for(const auto& ext : extensions)
		if(path.extension() == ext)
			return true;

	return false;
}

void ContentBrowserPanel::Draw() {
	ImGui::Begin("Content Browser", &m_Open);
	{
		auto windowFlags = ImGuiWindowFlags_MenuBar;
		auto childFlags = ImGuiChildFlags_Border;
		ImVec2 size = { 200, ImGui::GetContentRegionAvail().y };

		ImGui::BeginChild("##File-Hierarchy", size, childFlags, windowFlags);
		{
			ImVec2 windowStart = ImGui::GetCursorPos();
			ImGuiStyle& style = ImGui::GetStyle();
			const ImGuiWindow* window = ImGui::GetCurrentWindow();
			const ImRect titleBarRect = window->TitleBarRect();

			ImGui::PushClipRect(titleBarRect.Min, titleBarRect.Max, false);
			{
				float x = style.FramePadding.x
						+ ImGui::GetCurrentContext()->FontSize
						+ style.ItemInnerSpacing.x;
				ImGui::SetCursorPos({ x, style.FramePadding.y });
				ImGui::Text("File Hierarchy");
			}
			ImGui::PopClipRect();
			ImGui::SetCursorPos(windowStart);
		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("##Assets", { }, childFlags, windowFlags);
		{
			static float padding = 16.0f;
			static float thumbnailSize = 128.0f;
			static float cellSize = thumbnailSize + padding;

			float panelWidth = ImGui::GetContentRegionAvail().x;
			int32_t columnCount = (int32_t)(panelWidth / cellSize);
			columnCount = columnCount ? columnCount : 1;

			static int32_t change = 0;
			fs::path newPath;
			if(change)
				if(ImGui::Button("<-"))
					change--;

			if(ImGui::BeginTable("AssetsTable", columnCount))
			{
				for(auto p : FileUtils::GetFiles(m_CurrentPath.string())) {
					fs::path path(p);
					UI::Button button;
					button.Width = thumbnailSize;
					button.Height = thumbnailSize;

					ImGui::TableNextColumn();
					if(fs::is_directory(path)) {
						button.Display = m_FolderIcon;
						if(UI::UIRenderer::DrawButton(button).Clicked) {
							change++;
							newPath = path;
						}
					}
					else {
						button.Display = m_FileIcon;
						UI::UIRenderer::DrawButton(button);
					}
					ImGui::Text(path.stem().string().c_str());

					// std::string type;
					// if(HasExtension(path, { ".png", ".jpeg", ".jpg", ".webp" })) {
					// 	type = "Image";
					// }
					// if(ImGui::BeginDragDropSource())
					// {
					// 	ImGui::SetDragDropPayload("Image",
					// 		(void*)p.c_str(), p.size() * sizeof(const char*),
					// 		ImGuiCond_Once);

					// 	ImGui::EndDragDropSource();
					// }
				}

				ImGui::EndTable();
			}

			if(change < 0) {
				m_CurrentPath = m_CurrentPath.parent_path();
			}
			else if(change > 0) {
				m_CurrentPath /= newPath;
			}
		}
		ImGui::EndChild();

	}
	ImGui::End();
}

}