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
	if(path.string() != "")
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

		ImGui::BeginChild("File-Hierarchy-View", size, childFlags, windowFlags);
		{
			ImVec2 windowStart = ImGui::GetCursorPos();
			ImGuiStyle& style = ImGui::GetStyle();
			const ImGuiWindow* window = ImGui::GetCurrentWindow();
			ImRect rect = window->MenuBarRect();

			ImGui::PushClipRect(rect.Min, rect.Max, false);
			{
				float x = style.FramePadding.x
						+ ImGui::GetCurrentContext()->FontSize
						+ style.ItemInnerSpacing.x;
				float y = style.FramePadding.y;
				ImGui::SetCursorPos({ x, y });
				ImGui::Text("File Hierarchy");
			}
			ImGui::PopClipRect();
			ImGui::SetCursorPos(windowStart);

		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Assets-View", { }, childFlags, windowFlags);
		{
			ImVec2 windowStart = ImGui::GetCursorPos();
			ImGuiStyle& style = ImGui::GetStyle();
			const ImGuiWindow* window = ImGui::GetCurrentWindow();
			ImRect rect = window->MenuBarRect();

			ImGui::PushClipRect(rect.Min, rect.Max, false);
			{
				float x = style.FramePadding.x
						+ ImGui::GetCurrentContext()->FontSize
						+ style.ItemInnerSpacing.x;
				float y = style.FramePadding.y;
				ImGui::SetCursorPos({ x, y });
				ImGui::Text("Assets");
			}
			ImGui::PopClipRect();
			ImGui::SetCursorPos(windowStart);

			static float padding = 16.0f;
			static float thumbnailSize = 128.0f;
			static float cellSize = thumbnailSize + padding;

			float panelWidth = ImGui::GetContentRegionAvail().x;
			int32_t columnCount = (int32_t)(panelWidth / cellSize);
			columnCount = columnCount ? columnCount : 1;

			static fs::path m_CurrentAssetPath = m_AssetPath;

			int32_t delta = 0;
			static fs::path newPath;
			if(m_CurrentAssetPath != m_AssetPath)
				if(ImGui::Button("<-"))
					m_CurrentAssetPath = m_CurrentAssetPath.parent_path();

			if(ImGui::BeginTable("AssetsTable", columnCount))
			{
				for(auto p : FileUtils::GetFiles(m_CurrentAssetPath.string())) {
					fs::path path(p);
					UI::Button button;
					button.Width = thumbnailSize;
					button.Height = thumbnailSize;

					ImGui::TableNextColumn();
					if(fs::is_directory(path)) {
						button.Display = m_FolderIcon;
						if(UI::UIRenderer::DrawButton(button).Clicked) {
							delta++;
							newPath = path;
						}
					}
					else {
						button.Display = m_FileIcon;
						if(UI::UIRenderer::DrawButton(button).Clicked)
							newPath = path;
					}

					std::string type = "";
					if(HasExtension(newPath, { ".png", ".jpeg", ".jpg", ".webp" })) {
						type = "Image";
					}
					else if(HasExtension(newPath, { ".obj", ".gltf" })) {
						type = "Model";
					}

					if(type != "" && newPath.string() != "")
						if(ImGui::BeginDragDropSource())
						{
							ImGui::SetDragDropPayload(type.c_str(),
								newPath.string().c_str(),
								(newPath.string().size() + 1) * sizeof(char),
								ImGuiCond_Once);

							ImGui::Text(newPath.filename().string().c_str());
							ImGui::EndDragDropSource();
						}
					ImGui::TextWrapped(path.filename().string().c_str());
				}

				ImGui::EndTable();
			}

			if(delta > 0)
				m_CurrentAssetPath = newPath;
		}
		ImGui::EndChild();

	}
	ImGui::End();
}

}