#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <Magma/UI/UIRenderer.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Log.h>

#include "Editor.h"

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

static void TraverseFolder(fs::path folder);

void ContentBrowserPanel::Draw() {
	ImGui::Begin("Content Browser", &m_Open);
	{
		auto windowFlags = ImGuiWindowFlags_MenuBar;
		auto childFlags = ImGuiChildFlags_Border;

		ImVec2 size = { 300, ImGui::GetContentRegionAvail().y };
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

			TraverseFolder(m_Path);
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
						+ ImGui::GetCurrentContext()->FontSize;
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

struct Node {

};

// ImRect RenderTree(Node* n)
// {
// 	bool open = ImGui::TreeNodeEx();
// 	ImRect nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

// 	if(open) {
// 		ImColor TreeLineColor = ImGui::GetColorU32(ImGuiCol_Text);
// 		float SmallOffsetX = 11.0f; // for now, a hardcoded value; should take into account tree indent size
// 		ImDrawList* drawList = ImGui::GetWindowDrawList();

// 		ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
// 		verticalLineStart.x += SmallOffsetX; // to nicely line up with the arrow symbol
// 		ImVec2 verticalLineEnd = verticalLineStart;

// 		for (Node* child : *n)
// 		{
// 			float horizontalLineSize = 8.0f; //chosen arbitrarily
// 			ImRect childRect = RenderTree(child);
// 			float midpoint = (childRect.Min.y + childRect.Max.y) / 2.0f;
// 			auto p0 = ImVec2(verticalLineStart.x, midpoint);
// 			auto p1 = ImVec2(verticalLineStart.x + horizontalLineSize, midpoint);
// 			drawList->AddLine(p0, p1, TreeLineColor);
// 			verticalLineEnd.y = midpoint;
// 		}

// 		drawList->AddLine(verticalLineStart, verticalLineEnd, TreeLineColor);

// 		ImGui::TreePop();
// 	}

// 	return nodeRect;
// }

void TraverseFolder(fs::path folder) {
	List<fs::path> folders;
	List<fs::path> files;

	auto flags = ImGuiTreeNodeFlags_SpanAvailWidth;
	auto tag = folder.stem().string();
	bool open = ImGui::TreeNodeEx(tag.c_str(), flags, tag.c_str());
	if(!open)
		return;

	for(auto p : FileUtils::GetFiles(folder.string())) {
		fs::path path(p);
		if(fs::is_directory(path))
			folders.push_back(path);
		else
			files.push_back(path);
	}

	for(auto folder : folders)
		TraverseFolder(folder);

	static std::string selected = "";
	static std::string type = "";
	for(auto file : files) {
		auto fileFlags =
			flags | ImGuiTreeNodeFlags_Bullet
				  | ImGuiTreeNodeFlags_DefaultOpen;
		if(ImGui::TreeNodeEx(file.filename().string().c_str(), fileFlags)) {
			if(ImGui::IsMouseClicked(1) && ImGui::IsItemHovered()) {
				if(file.extension().string() == ".scene")
					type = "Scene";
				else if(file.extension().string() == ".json")
					type = "UI";

				if(type != "") {
					ImGui::OpenPopup("NewTab");
					selected = file.string();
				}
			}

			if(ImGui::BeginPopup("NewTab")) {
				if(ImGui::Button("Open file as new tab")) {
					auto& editor = Application::As<Editor>()->GetEditor();
					if(type == "Scene")
						editor.NewTab(CreateRef<SceneTab>(selected));
					if(type == "UI")
						editor.NewTab(CreateRef<UITab>(selected));

					selected = "";
					type = "";
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::TreePop();
		}
	}

	ImGui::TreePop();
}

}