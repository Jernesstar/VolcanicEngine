#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

// #include <filewatch/FileWatch.hpp>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/List.h>

#include <Magma/UI/UIRenderer.h>

#include "Editor/EditorApp.h"
#include "Editor/AssetImporter.h"

#include "Project/AssetEditorPanel.h"

namespace Magma {

// static List<filewatch::FileWatch<std::string>> s_Watchers;

ContentBrowserPanel::ContentBrowserPanel(const std::string& path)
	: Panel("ContentBrowser"), m_Path(path)
{
	m_FileIcon =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/FileIcon.png"));
	m_FolderIcon =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/FolderIcon.png"));

	// s_Watchers.Add(
	// 	filewatch::FileWatch<std::string>(
	// 		(fs::path(path) / "Visual" / "Asset").string(),
	// 		[](const std::string& path, const filewatch::Event event)
	// 		{
	// 			std::cout << path << " : ";
	// 			switch (event) {
	// 				case filewatch::Event::added:
	// 					std::cout << "The file was added to the directory." << '\n';
	// 					break;
	// 				case filewatch::Event::removed:
	// 					std::cout << "The file was removed from the directory." << '\n';
	// 					break;
	// 				case filewatch::Event::modified:
	// 					std::cout << "The file was modified. This can be a change in the time stamp or attributes." << '\n';
	// 					break;
	// 				case filewatch::Event::renamed_old:
	// 					std::cout << "The file was renamed and this is the old name." << '\n';
	// 					break;
	// 				case filewatch::Event::renamed_new:
	// 					std::cout << "The file was renamed and this is the new name." << '\n';
	// 					break;
	// 				};
	// 			}
	// 	)
	// );
}

ContentBrowserPanel::~ContentBrowserPanel() {
	// s_Watchers.Clear();
}

void ContentBrowserPanel::Update(VolcaniCore::TimeStep ts) {

}

static bool HasExtension(fs::path path,
	const VolcaniCore::List<std::string>& extensions)
{
	if(path.string() != "")
		for(const auto& ext : extensions)
			if(path.extension() == ext)
				return true;

	return false;
}

static ImRect Traverse(fs::path folder);

void ContentBrowserPanel::Draw() {
	ImGui::Begin("Content Browser", &Open);
	{
		auto windowFlags = ImGuiWindowFlags_MenuBar;
		auto childFlags = ImGuiChildFlags_Border;

		ImVec2 size = { 350, ImGui::GetContentRegionAvail().y };
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

			Traverse(m_Path);
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

			static float padding = 18.0f;
			static float thumbnailSize = 100.0f;
			static float cellSize = thumbnailSize + padding;

			float panelWidth = ImGui::GetContentRegionAvail().x;
			int32_t columnCount = (int32_t)(panelWidth / cellSize);
			columnCount = columnCount ? columnCount : 1;

			auto& assetManager =
				Application::As<EditorApp>()->GetEditor().GetAssetManager();

			if(ImGui::BeginTable("AssetsTable", columnCount))
			{
				static Asset s_Asset;

				for(auto& [asset, _] : assetManager.GetRegistry()) {
					if(!asset.Primary)
						continue;

					ImGui::TableNextColumn();

					std::string display = assetManager.GetPath(asset.ID);
					if(display != "")
						display = fs::path(display).filename().string();

					UI::Button button;
					button.Width = thumbnailSize;
					button.Height = thumbnailSize;
					button.Display = m_FileIcon;
					button.x = -1;
					button.y = -1;
					if(UI::UIRenderer::DrawButton(button).Clicked) {
						s_Asset = asset;
						auto panel =
							m_Tab->GetPanel("AssetEditor")
							->As<AssetEditorPanel>();
						panel->Select(asset);
					}

					if(ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("ASSET",
							&s_Asset, sizeof(Asset), ImGuiCond_Once);

						UI::Image image;
						image.Content = m_FileIcon->Content;
						image.Width = thumbnailSize;
						image.Height = thumbnailSize;
						image.x = -1;
						image.y = -1;
						UI::UIRenderer::DrawImage(image);

						ImGui::EndDragDropSource();
					}
					if(display != "")
						ImGui::TextWrapped(display.c_str());
					ImGui::Text("Asset %lu", asset.ID / (uint64_t)1e15);
				}

				ImGui::EndTable();
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

ImRect Traverse(fs::path path) {
	List<fs::path> folders;
	List<fs::path> files;

	ImRect nodeRect;
	if(fs::is_directory(path)) {
		auto flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		auto tag = path.stem().string();
		bool open = ImGui::TreeNodeEx(tag.c_str(), flags, tag.c_str());
		nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

		if(!open)
			return nodeRect;

		for(auto p : FileUtils::GetFiles(path.string())) {
			fs::path path(p);
			if(fs::is_directory(path))
				folders.Add(path);
			else
				files.Add(path);
		}

	}
	else {
		static std::string selected = "";
		static std::string type = "";

		auto flags = ImGuiTreeNodeFlags_SpanAvailWidth
					| ImGuiTreeNodeFlags_Bullet
					| ImGuiTreeNodeFlags_DefaultOpen;
		bool open = ImGui::TreeNodeEx(path.filename().string().c_str(), flags);

		nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		if(ImGui::IsMouseClicked(1) && ImGui::IsItemHovered()) {
			if(path.extension().string() == ".scene")
				type = "Scene";
			else if(path.extension().string() == ".json")
				type = "UI";

			if(type != "") {
				ImGui::OpenPopup("NewTab");
				selected = path.string();
			}
		}

		if(ImGui::BeginPopup("NewTab")) {
			if(ImGui::Button("Open file as new tab")) {
				auto& editor = Application::As<EditorApp>()->GetEditor();
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

		if(open)
			ImGui::TreePop();
	}

	ImColor TreeLineColor = ImGui::GetColorU32(ImGuiCol_Text);
	float SmallOffsetX = -8.5f;
	ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
	verticalLineStart.x += SmallOffsetX; // Line up with the arrow
	ImVec2 verticalLineEnd = verticalLineStart;

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	for(auto entries : { folders, files })
		for(auto entry : entries) {
			ImRect childRect = Traverse(entry);
			float horizontalLineSize = 8.0f; // Arbitrary
			float midpoint = (childRect.Min.y + childRect.Max.y) / 2.0f;
			auto p0 = ImVec2(verticalLineStart.x, midpoint);
			auto p1 = ImVec2(verticalLineStart.x + horizontalLineSize, midpoint);
			drawList->AddLine(p0, p1, TreeLineColor);
			verticalLineEnd.y = midpoint;
		}

	drawList->AddLine(verticalLineStart, verticalLineEnd, TreeLineColor);

	if(fs::is_directory(path))
		ImGui::TreePop();

	return nodeRect;
}

}