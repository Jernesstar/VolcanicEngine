// #include "SceneHierarchyPanel.h"

// #include <cstring>

// #include <imgui/imgui.h>
// #include <imgui/imgui_internal.h>
// #include <imgui/misc/cpp/imgui_stdlib.h>

// #include <glm/gtc/type_ptr.hpp>

// #include "ECS/Component.h"

// using namespace Magma::ECS;

// namespace Magma {

// static void DrawVec3Control(const std::string& label, glm::vec3& values,
// 							float resetValue = 0.0f,
// 							float columnWidth = 190.0f);

// void SceneHierarchyPanel::Draw() {
// 	ImGui::Begin("Scene Hierarchy");
// 	{
// 		m_Context->EntityWorld
// 		.ForEach(
// 			[&](Entity entity)
// 			{
// 				DrawEntityNode(entity);
// 			});

// 		if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
// 			m_Selected = Entity{ };

// 		// TODO(Fix): Multiple entities in Hierarchy panel
// 		if(ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems)) {
// 			if(ImGui::MenuItem("Create Empty Entity"))
// 				m_Selected = m_Context->EntityWorld.AddEntity("Empty Entity");

// 			ImGui::EndPopup();
// 		}
// 	}

// 	if(m_Selected) {
// 		ImGui::Begin("Properties");
// 		DrawComponents(m_Selected);
// 		ImGui::End();
// 	}

// 	ImGui::End();
// }

// void DrawComponents(ECS::Entity& entity) {
// 	if(!entity.IsValid())
// 		return;

// 	// if(entity.Has<TagComponent>()) {
// 	// 	auto& tag = entity.Get<TagComponent>().Tag;

// 	// 	char buffer[256];
// 	// 	memset(buffer, 0, sizeof(buffer));
// 	// 	strcpy(buffer, tag.c_str());
// 	// 	if(ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
// 	// 		tag = std::string(buffer);
// 	// 	}
// 	// }

// 	ImGui::SameLine();
// 	ImGui::PushItemWidth(-1);

// 	if(ImGui::Button("Add Component"))
// 		ImGui::OpenPopup("AddComponent");

// 	if(ImGui::BeginPopup("AddComponent")) {
// 		DisplayAddComponentEntry<TransformComponent>("Transform Component");
// 		ImGui::EndPopup();
// 	}

// 	ImGui::PopItemWidth();

// 	DrawComponent<TransformComponent>("Transform", entity,
// 		[&](auto& component)
// 		{
// 			DrawVec3Control("Translation", component.Translation);
// 			glm::vec3 rotation = glm::degrees(component.Rotation);
// 			DrawVec3Control("Rotation", rotation);
// 			component.Rotation = glm::radians(rotation);
// 			DrawVec3Control("Scale", component.Scale, 1.0f);
// 		});

// 	// DrawComponent<MeshComponent>("Texture", entity,
// 	// [&](auto& component) {
// 	// 	ImGui::Button("Texture", ImVec2(120.0f, 0.0f));
// 	// 	if(ImGui::BeginDragDropTarget()) {
// 	// 		if(const ImGuiPayload* payload =
// 	// 					ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
// 	// 		{
// 	// 			const wchar_t* path = (const wchar_t*)payload->Data;
// 	// 			std::filesystem::path texturePath(path);
// 	// 			Ref<Texture> texture = Texture::Create(texturePath.string());
// 	// 			component.Texture = texture;
// 	// 		}

// 	// 		ImGui::EndDragDropTarget();
// 	// 	}
// 	// });
// }

// void SceneHierarchyPanel::DrawEntityNode(ECS::Entity& entity) {
// 	auto& tag = entity.Get<TagComponent>().Tag;
	
// 	ImGuiTreeNodeFlags flags;
// 	if(m_Selected)
// 		flags = ((m_Selected == entity) ? ImGuiTreeNodeFlags_Selected : 0)
// 										| ImGuiTreeNodeFlags_OpenOnArrow;

// 	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
// 	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entity, flags, tag.c_str());

// 	if(ImGui::IsItemClicked())
// 		m_Selected = entity;

// 	bool entityDeleted = false;
// 	if(ImGui::BeginPopupContextItem()) {
// 		if(ImGui::MenuItem("Delete Entity"))
// 			entityDeleted = true;

// 		ImGui::EndPopup();
// 	}

// 	if(opened)
// 		ImGui::TreePop();

// 	if(entityDeleted) {
// 		entity.Kill();
// 		if(m_Selected && m_Selected == entity)
// 			m_Selected = Entity{ };
// 	}
// }

// template<typename TComponent>
// void DisplayAddComponentEntry(const std::string& entryName)
// {
// 	if(!m_Selected.Has<TComponent>()) {
// 		if(ImGui::MenuItem(entryName.c_str())) {
// 			m_Selected.Add<TComponent>();
// 		}
// 		ImGui::CloseCurrentPopup();
// 	}
// }

// template<typename TComponent, typename TUIFunction>
// void DrawComponent(const std::string& name,
// 										ECS::Entity& entity,
// 										TUIFunction uiFunction)
// {
// 	if(!entity.Has<TComponent>())
// 		return;

// 	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen
// 							 | ImGuiTreeNodeFlags_Framed
// 							 | ImGuiTreeNodeFlags_SpanAvailWidth
// 							 | ImGuiTreeNodeFlags_AllowItemOverlap
// 							 | ImGuiTreeNodeFlags_FramePadding;
// 	auto& component = entity.Get<TComponent>();
// 	ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

// 	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
// 	float lineHeight = GImGui->Font->FontSize
// 					 + GImGui->Style.FramePadding.y * 2.0f;
// 	ImGui::Separator();
// 	bool open =
// 		ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, name.c_str());
// 	ImGui::PopStyleVar();
// 	ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

// 	if(ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) {
// 		ImGui::OpenPopup("ComponentSettings");
// 	}

// 	bool removeComponent = false;
// 	if(ImGui::BeginPopup("ComponentSettings")) {
// 		if(ImGui::MenuItem("Remove component"))
// 			removeComponent = true;

// 		ImGui::EndPopup();
// 	}

// 	if(open) {
// 		uiFunction(component);
// 		ImGui::TreePop();
// 	}

// 	if(removeComponent)
// 		entity.Remove<TComponent>();
// }

// static void DrawVec3Control(const std::string& label, glm::vec3& values,
// 							float resetValue, float columnWidth)
// {
// 	ImGuiIO& io = ImGui::GetIO();
// 	auto boldFont = io.Fonts->Fonts[0];

// 	ImGui::PushID(label.c_str());

// 	ImGui::Columns(2);
// 	ImGui::SetColumnWidth(0, columnWidth);
// 	ImGui::Text(label.c_str());
// 	ImGui::NextColumn();

// 	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
// 	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

// 	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
// 	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

// 	ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.0f });
// 	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.0f });
// 	ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
// 	ImGui::PushFont(boldFont);
// 	if(ImGui::Button("X", buttonSize))
// 		values.x = resetValue;
// 	ImGui::PopFont();
// 	ImGui::PopStyleColor(3);

// 	ImGui::SameLine();
// 	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
// 	ImGui::PopItemWidth();
// 	ImGui::SameLine();

// 	ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.0f });
// 	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
// 	ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.0f });
// 	ImGui::PushFont(boldFont);
// 	if(ImGui::Button("Y", buttonSize))
// 		values.y = resetValue;
// 	ImGui::PopFont();
// 	ImGui::PopStyleColor(3);

// 	ImGui::SameLine();
// 	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
// 	ImGui::PopItemWidth();
// 	ImGui::SameLine();

// 	ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.25f, 0.8f, 1.0f });
// 	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });
// 	ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.25f, 0.8f, 1.0f });
// 	ImGui::PushFont(boldFont);
// 	if(ImGui::Button("Z", buttonSize))
// 		values.z = resetValue;
// 	ImGui::PopFont();
// 	ImGui::PopStyleColor(3);

// 	ImGui::SameLine();
// 	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
// 	ImGui::PopItemWidth();

// 	ImGui::PopStyleVar();

// 	ImGui::Columns(1);

// 	ImGui::PopID();
// }

// }