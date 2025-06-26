#include "ComponentEditorPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <angelscript/add_on/scriptarray/scriptarray.h>

#include <VolcaniCore/Core/Log.h>

#include <Magma/UI/UIRenderer.h>
#include <Magma/Scene/Component.h>

#include <Lava/Types/GridSet.h>
#include <Lava/Types/GridSet3D.h>
#include <Lava/Types/Timer.h>

#include "Editor/EditorApp.h"
#include "Editor/AssetManager.h"
#include "Editor/AssetImporter.h"
#include "Editor/ScriptManager.h"

#include "Editor/Project/ContentBrowserPanel.h"

#undef near
#undef far

#define FOCUS_COMPONENT(Name) \
template<> \
void ComponentEditorPanel::SetFocus<Name##Component>() { \
	ClearFocus(); \
	focus.component.Name = true; \
} \
template<> \
bool ComponentEditorPanel::IsFocused<Name##Component>(Entity& entity) { \
	return m_Context == entity && focus.component.Name; \
}

using namespace Magma::Physics;

namespace Magma {

ComponentEditorPanel::ComponentEditorPanel()
	: Panel("ComponentEditor")
{

}

void ComponentEditorPanel::SetContext(Entity& entity) {
	m_Context = entity;
	auto panel =
		Editor::GetProjectTab()->
				GetPanel("ContentBrowser")->As<ContentBrowserPanel>();
	panel->CancelSelect();
}

void ComponentEditorPanel::Update(TimeStep ts) {

}

struct {
	struct {
		bool Camera	   = false;
		bool Tag	   = false;
		bool Transform = false;
		bool Audio	   = false;
		bool Mesh	   = false;
		bool Skybox	   = false;
		bool Script	   = false;
		bool RigidBody = false;
		bool DirectionalLight = false;
		bool PointLight = false;
		bool Spotlight = false;
		bool ParticleEmitter = false;
	} component;
} focus;

void ComponentEditorPanel::ClearFocus() {
	focus.component.Camera = false;
	focus.component.Tag = false;
	focus.component.Transform = false;
	focus.component.Audio = false;
	focus.component.Mesh = false;
	focus.component.Skybox = false;
	focus.component.Script = false;
	focus.component.RigidBody = false;
	focus.component.DirectionalLight = false;
	focus.component.PointLight = false;
	focus.component.Spotlight = false;
	focus.component.ParticleEmitter = false;
}

FOCUS_COMPONENT(Camera)
FOCUS_COMPONENT(Tag)
FOCUS_COMPONENT(Transform)
FOCUS_COMPONENT(Audio)
FOCUS_COMPONENT(Mesh)
FOCUS_COMPONENT(Skybox)
FOCUS_COMPONENT(Script)
FOCUS_COMPONENT(RigidBody)
FOCUS_COMPONENT(DirectionalLight)
FOCUS_COMPONENT(PointLight)
FOCUS_COMPONENT(Spotlight)
FOCUS_COMPONENT(ParticleEmitter)

template<typename TComponent>
static void DrawComponent(Entity& entity);

template<>
void DrawComponent<CameraComponent>(Entity& entity) {
	if(!entity.Has<CameraComponent>())
		return;

	auto& component = entity.Set<CameraComponent>();
	auto& camera = component.Cam;
	ImGui::SeparatorText("CameraComponent");

	if(!camera) {
		if(ImGui::Button("Create Stereographic"))
			camera = Camera::Create(Camera::Type::Stereo);
		if(ImGui::Button("Create Orthographic"))
			camera = Camera::Create(Camera::Type::Ortho);
		return;
	}

	auto typeStr = camera->GetType() == Camera::Type::Ortho ?
		"Type: Orthographic" : "Type: Stereographic";
	ImGui::Text(typeStr); ImGui::SameLine();
	if(ImGui::Button("Switch")) {
		if(camera->GetType() == Camera::Type::Stereo)
			camera = Camera::Create(Camera::Type::Ortho);
		else if(camera->GetType() == Camera::Type::Ortho)
			camera = Camera::Create(Camera::Type::Stereo);
		return;
	}

	ImGui::SetNextItemWidth(150);
	auto pos = camera->GetPosition();
	if(ImGui::DragFloat3("Position", &pos.x, 1.0f, -FLT_MAX/2.0f, +FLT_MAX/2.0f))
		camera->SetPosition(pos);
	ImGui::SetNextItemWidth(150);
	auto dir = camera->GetDirection();
	if(ImGui::DragFloat3("Direction", &dir.x, 1.0f, -FLT_MAX/2.0f, +FLT_MAX/2.0f))
		camera->SetDirection(dir);

	uint32_t max = 3000;
	uint32_t min = 0;
	uint32_t vW = camera->GetViewportWidth();
	ImGui::SetNextItemWidth(50);
	bool w =
		ImGui::DragScalar("Viewport Width", ImGuiDataType_U32,
			&vW, 1.0f, &min, &max);
	uint32_t vH = camera->GetViewportHeight();
	ImGui::SetNextItemWidth(50);
	bool h =
		ImGui::DragScalar("Viewport Height", ImGuiDataType_U32,
			&vH, 1.0f, &min, &max);
	if(w || h)
		camera->Resize(vW, vH);

	float near = camera->GetNear();
	float far = camera->GetFar();
	ImGui::SetNextItemWidth(50);
	bool newNear =
		ImGui::DragFloat("Near", &near, 1.0f, 0.001f, 1000.0f, "%.4f");
	ImGui::SetNextItemWidth(50);
	bool newFar =
		ImGui::DragFloat("Far", &far, 1.0f, 0.001f, 1000.0f, "%.4f");
	if(newNear || newFar)
		camera->SetProjection(near, far);

	if(camera->GetType() == Camera::Type::Stereo) {
		auto cam = camera->As<StereographicCamera>();
		float fov = cam->GetVerticalFOV();
		ImGui::SetNextItemWidth(50);
		if(ImGui::DragFloat("FOV", &fov, 1.0f, 0.001f, 180.0f, "%.4f"))
			cam->SetVerticalFOV(fov);
	}
}

template<>
void DrawComponent<TagComponent>(Entity& entity) {
	if(!entity.Has<TagComponent>())
		return;

	auto& component = entity.Set<TagComponent>();
	ImGui::SeparatorText("TagComponent");
	ImGui::InputText("##Tag", &component.Tag);
}

template<>
void DrawComponent<TransformComponent>(Entity& entity) {
	if(!entity.Has<TransformComponent>())
		return;

	auto& component = entity.Set<TransformComponent>();
	ImGui::SeparatorText("TransformComponent");

	auto tr = glm::value_ptr(component.Translation);
	glm::vec3 roD = glm::degrees(component.Rotation);
	auto ro = glm::value_ptr(roD);
	auto sc = glm::value_ptr(component.Scale);
	ImGui::Text("Translation"); ImGui::SameLine(120.0f);
	ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("##Translation", tr, 0.5f, -FLT_MAX, +FLT_MAX, "%.2f");
	ImGui::Text("Rotation"); ImGui::SameLine(120.0f);
	ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("##Rotation", ro, 0.5f, 0.0001f, 360.0f, "%.2f");
	ImGui::Text("Scale"); ImGui::SameLine(120.0f);
	ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("##Scale", sc, 0.5f, 0.0001f, +FLT_MAX, "%.2f");

	component.Rotation = glm::radians(roD);
}

template<>
void DrawComponent<AudioComponent>(Entity& entity) {
	if(!entity.Has<AudioComponent>())
		return;

	auto& component = entity.Set<AudioComponent>();
	ImGui::SeparatorText("AudioComponent");

	ImGui::Text("Asset ID: %llu", (uint64_t)component.AudioAsset.ID);
	auto text = component.AudioAsset.ID ? "Change Asset" : "Set Asset";

	auto panel =
		Editor::GetProjectTab()->
				GetPanel("ContentBrowser")->As<ContentBrowserPanel>();

	if(ImGui::Button(text))
		panel->Select(AssetType::Audio);
	if(panel->HasSelection())
		component.AudioAsset = panel->GetSelected();
}

template<>
void DrawComponent<MeshComponent>(Entity& entity) {
	if(!entity.Has<MeshComponent>())
		return;

	auto& component = entity.Set<MeshComponent>();
	ImGui::SeparatorText("MeshComponent");

	auto panel =
		Editor::GetProjectTab()->
				GetPanel("ContentBrowser")->As<ContentBrowserPanel>();

	ImGui::Text("Mesh Source: %llu", (uint64_t)component.MeshSourceAsset.ID);
	auto text = component.MeshSourceAsset.ID ? "Change Asset" : "Set Asset";

	if(ImGui::Button(text))
		panel->Select(AssetType::Mesh);
	if(panel->HasSelection())
		component.MeshSourceAsset = panel->GetSelected();

	auto assets = AssetManager::Get()->As<EditorAssetManager>();
	if(assets->IsNativeAsset(component.MeshSourceAsset)) {
		ImGui::Text("Material: %llu", (uint64_t)component.MaterialAsset.ID);
		std::string text =
			component.MaterialAsset.ID ? "Change Asset" : "Set Asset";

		if(ImGui::Button((text + "##2").c_str())) {
			panel->CancelSelect();
			panel->Select(AssetType::Material, 1);
		}
		if(panel->HasSelection(1))
			component.MaterialAsset = panel->GetSelected();
	}
}

template<>
void DrawComponent<SkyboxComponent>(Entity& entity) {
	if(!entity.Has<SkyboxComponent>())
		return;

	auto& component = entity.Set<SkyboxComponent>();
	ImGui::SeparatorText("SkyboxComponent");

	ImGui::Text("Asset ID: %llu", (uint64_t)component.CubemapAsset.ID);
	auto text = component.CubemapAsset.ID ? "Change Asset" : "Set Asset";

	auto panel =
		Editor::GetProjectTab()->
				GetPanel("ContentBrowser")->As<ContentBrowserPanel>();

	if(ImGui::Button(text))
		panel->Select(AssetType::Cubemap);
	if(panel->HasSelection())
		component.CubemapAsset = panel->GetSelected();
}

static bool s_SelectingClass = false;

std::string SelectScriptClass(Ref<ScriptModule> mod) {
	static std::string select = "";

	ImGui::OpenPopup("Select Script Class");
	if(ImGui::BeginPopupModal("Select Script Class"))
	{
		for(const auto& [name, _class] : mod->GetClasses()) {
			if(!_class->Implements("IEntityController"))
				continue;

			bool pressed = ImGui::Button(name.c_str());
			if(pressed) {
				select = name;
				s_SelectingClass = false;
				ImGui::CloseCurrentPopup();
			}
		}
		if(ImGui::Button("Close")) {
			s_SelectingClass = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if(select != "") {
		std::string val = select;
		select = "";
		return val;
	}

	return "";
}

static bool s_GridSetEdit = false;

static void ListEditorPopup(Ref<ScriptObject> obj, const std::string& name) {

}

static void GridSetEditorPopup(Ref<ScriptObject> obj, const std::string& name) {
	auto* data = obj->GetProperty(name).As<Lava::GridSet>();
	if(!data)
		return;

	ImGui::OpenPopup("GridSet Editor");
	if(ImGui::BeginPopupModal("GridSet Editor")) {
		if(ImGui::Button("Close")) {
			s_GridSetEdit = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if(ImGui::Button("Clear"))
			data->Clear();

		uint32_t width = data->GetWidth();
		uint32_t height = data->GetHeight();
		ImGui::SetNextItemWidth(50);
		bool w = ImGui::InputScalar("Width", ImGuiDataType_U32, &width);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50);
		bool h = ImGui::InputScalar("Height", ImGuiDataType_U32, &height);

		if(w)
			data->ResizeX(width);
		if(h)
			data->ResizeY(height);

		UI::Button button;
		button.Display = CreateRef<UI::Text>();
		// button.Color = { };
		button.SetSize(18.0f, 18.0f);

		if(*data)
			for(uint32_t y = 0; y < height; y++) {
				for(uint32_t x = 0; x < width; x++) {
					uint8_t& val = *data->At(x, y);

					button.Display->As<UI::Text>()->Content =
						std::to_string(val);
					button.x = 12 + x * 23.0f;
					button.y = 120 + y * 23.0f;
					button.Render();

					if(ImGui::IsMouseClicked(0) && ImGui::IsItemHovered())
						val++;
					else if(ImGui::IsMouseClicked(1)
						&& ImGui::IsItemHovered() && val > 0)
					{
						val--;
					}
				}
			}

		ImGui::EndPopup();
	}
}

template<>
void DrawComponent<ScriptComponent>(Entity& entity) {
	if(!entity.Has<ScriptComponent>())
		return;

	auto& component = entity.Set<ScriptComponent>();
	ImGui::SeparatorText("ScriptComponent");

	ImGui::Text("Asset ID: %llu", (uint64_t)component.ModuleAsset.ID);
	auto text = component.ModuleAsset.ID ? "Change Module" : "Set Module";

	auto panel =
		Editor::GetProjectTab()->
				GetPanel("ContentBrowser")->As<ContentBrowserPanel>();

	if(ImGui::Button(text))
		panel->Select(AssetType::Script);
	if(panel->HasSelection())
		component.ModuleAsset = panel->GetSelected();

	if(!component.ModuleAsset)
		return;

	auto& editor = Application::As<EditorApp>()->GetEditor();
	auto& assetManager = editor.GetAssetManager();

	if(!component.Instance) {
		std::string name;
		if(ImGui::Button("Create Instance")) {
			assetManager.Load(component.ModuleAsset);
			s_SelectingClass = true;
		}
		if(s_SelectingClass) {
			auto mod = assetManager.Get<ScriptModule>(component.ModuleAsset);
			name = SelectScriptClass(mod);
			if(name != "") {
				auto _class = mod->GetClass(name);
				component.Instance = _class->Construct();
			}
		}

		return;
	}
	ImGui::Text("Class: %s", component.Instance->GetClass()->Name.c_str());

	ImGui::SeparatorText("Fields");

	auto* handle = component.Instance->GetHandle();
	for(uint32_t i = 0; i < handle->GetPropertyCount(); i++) {
		ScriptField field = component.Instance->GetProperty(i);
		bool editorField =
			ScriptManager::FieldHasMetadata(
				component.Instance->GetClass()->Name, field.Name, "EditorField");
		if(!editorField)
			continue;

		ImGui::PushID(i);

		if(field.Type) {
			std::string typeName = field.Type->GetName();
			ImGui::Text(typeName.c_str()); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);

			if(typeName == "string") {
				ImGui::SetNextItemWidth(150);
				ImGui::InputText("##String", field.As<std::string>());
			}
			else if(typeName == "array") {
				
			}
			else if(typeName == "Asset") {
				Asset asset = *field.As<Asset>();
				ImGui::Text("Type: %s", AssetTypeToString(asset.Type).c_str());
				ImGui::SameLine(280.0f, 0.0f);
				ImGui::Text("ID: %llu", (uint64_t)asset.ID);

				if(ImGui::Button("Edit"))
					panel->Select(AssetType::None, i + 1);
				if(panel->HasSelection(i + 1))
					*field.As<Asset>() = panel->GetSelected();
			}
			if(typeName == "Vec3") {
				ImGui::SetNextItemWidth(150);
				ImGui::DragFloat3("##Vec3", &field.As<Vec3>()->r);
			}
			else if(typeName == "GridSet") {
				if(ImGui::Button("Edit GridSet"))
					s_GridSetEdit = true;
				if(s_GridSetEdit)
					GridSetEditorPopup(component.Instance, field.Name);
			}
			else
				ImGui::NewLine();
		}
		else if(field.TypeID == asTYPEID_BOOL) {
			ImGui::Text("bool"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::Checkbox("##Bool", field.As<bool>());
		}
		else if(field.TypeID == asTYPEID_INT8) {
			ImGui::Text("int8"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##S8", ImGuiDataType_S16, field.Data);
		}
		else if(field.TypeID == asTYPEID_INT16) {
			ImGui::Text("int16"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##S16", ImGuiDataType_S16, field.Data);
		}
		else if(field.TypeID == asTYPEID_INT32) {
			ImGui::Text("int32"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##S32", ImGuiDataType_S32, field.Data);
		}
		else if(field.TypeID == asTYPEID_INT64) {
			ImGui::Text("int64"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##S64", ImGuiDataType_S64, field.Data);
		}
		else if(field.TypeID == asTYPEID_UINT8) {
			ImGui::Text("uint8"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##U8", ImGuiDataType_U8, field.Data);
		}
		else if(field.TypeID == asTYPEID_UINT16) {
			ImGui::Text("uint16"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##U16", ImGuiDataType_U16, field.Data);
		}
		else if(field.TypeID == asTYPEID_UINT32) {
			ImGui::Text("uint32"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##U32", ImGuiDataType_U32, field.Data);
		}
		else if(field.TypeID == asTYPEID_UINT64) {
			ImGui::Text("uint64"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##U64", ImGuiDataType_U64, field.Data);
		}
		else if(field.TypeID == asTYPEID_DOUBLE) {
			ImGui::Text("double"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##Double", ImGuiDataType_Double,
				field.As<double>());
		}
		else if(field.TypeID == asTYPEID_FLOAT) {
			ImGui::Text("float"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat("##Float",
							 field.As<float>(), 0.1f, 0.0f, 0.0f, "%.3f");
		}

		ImGui::PopID();
	}
}

template<>
void DrawComponent<RigidBodyComponent>(Entity& entity) {
	if(!entity.Has<RigidBodyComponent>())
		return;

	auto& component = entity.Set<RigidBodyComponent>();
	ImGui::SeparatorText("RigidBodyComponent");

	Ref<RigidBody> body = component.Body;
	if(!body) {
		if(ImGui::Button("Create RigidBody")) {

		}

		return;
	}

	ImGui::Text("Type"); ImGui::SameLine(100.0f);
	switch(body->GetType()) {
		case RigidBody::Type::Static:
		{
			ImGui::Text("Static");
			if(ImGui::Button("Switch to Dynamic")) {

			}

			break;
		}
		case RigidBody::Type::Dynamic:
		{
			ImGui::Text("Dynamic");
			if(ImGui::Button("Switch to Static")) {

			}

			break;
		}
	}

	ImGui::Text("Shape Type"); ImGui::SameLine(100.0f);
	auto shape = body->GetShape();
	switch(shape->GetType()) {
		case Shape::Type::Box:
			ImGui::Text("Box");
			break;
		case Shape::Type::Sphere:
			ImGui::Text("Sphere");
			break;
		case Shape::Type::Plane:
			ImGui::Text("Plane");
			break;
		case Shape::Type::Capsule:
			ImGui::Text("Capsule");
			break;
		case Shape::Type::Mesh:
			ImGui::Text("Mesh");
			break;
	}
	if(ImGui::Button("Switch Shape Type")) {

	}

	auto tr = body->GetTransform();
	ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("##Translation", &tr.Translation.x, 0.1f,
		-FLT_MAX, +FLT_MAX, "%.4f");
	ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("##Rotation", &tr.Rotation.x, 0.1f,
		0.0001f, 360.0f, "%.4f");
	ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("##Scale", &tr.Scale.x, 0.5f, 0.0001f, +FLT_MAX, "%.4f");
}

template<>
void DrawComponent<DirectionalLightComponent>(Entity& entity) {
	if(!entity.Has<DirectionalLightComponent>())
		return;

	auto& component = entity.Set<DirectionalLightComponent>();
	ImGui::SeparatorText("DirectionalLightComponent");

	ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("Position", &component.Position.x, 0.1f,
		-FLT_MAX, +FLT_MAX, "%.4f");
	ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("Direction", &component.Direction.x, 0.1f,
		-FLT_MAX, +FLT_MAX, "%.4f");
	ImGui::ColorEdit3("Ambient", &component.Ambient.x);
	ImGui::ColorEdit3("Diffuse", &component.Diffuse.x);
	ImGui::ColorEdit3("Specular", &component.Specular.x);
}

template<>
void DrawComponent<PointLightComponent>(Entity& entity) {
	if(!entity.Has<PointLightComponent>())
		return;

	auto& component = entity.Set<PointLightComponent>();
	ImGui::SeparatorText("PointLightComponent");

	ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("Position", &component.Position.x, 0.1f,
		-FLT_MAX, +FLT_MAX, "%.4f");
	ImGui::ColorEdit3("Ambient", &component.Ambient.x);
	ImGui::ColorEdit3("Diffuse", &component.Diffuse.x);
	ImGui::ColorEdit3("Specular", &component.Specular.x);
	ImGui::SetNextItemWidth(50);
	ImGui::DragFloat("Constant", &component.Constant);
	ImGui::SetNextItemWidth(50);
	ImGui::DragFloat("Linear", &component.Linear);
	ImGui::SetNextItemWidth(50);
	ImGui::DragFloat("Quadratic", &component.Quadratic);
	ImGui::SetNextItemWidth(50);
	ImGui::Checkbox("Bloom", &component.Bloom);
}

template<>
void DrawComponent<SpotlightComponent>(Entity& entity) {
	if(!entity.Has<SpotlightComponent>())
		return;

	auto& component = entity.Set<SpotlightComponent>();
	ImGui::SeparatorText("SpotlightComponent");

	ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("Position", &component.Position.x, 0.1f,
		-FLT_MAX / 2.0f, +FLT_MAX / 2.0f, "%.4f");
	ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("Direction", &component.Direction.x, 0.1f,
		-FLT_MAX / 2.0f, +FLT_MAX / 2.0f, "%.4f");
	ImGui::ColorEdit3("Ambient", &component.Ambient.x);
	ImGui::ColorEdit3("Diffuse", &component.Diffuse.x);
	ImGui::ColorEdit3("Specular", &component.Specular.x);
	ImGui::SetNextItemWidth(50);
	ImGui::DragFloat("Cutoff Angle", &component.CutoffAngle, 1.0f, 0.1f,
		component.OuterCutoffAngle);
	ImGui::SetNextItemWidth(50);
	ImGui::DragFloat("Outer Cutoff Angle", &component.OuterCutoffAngle, 1.0f,
		component.CutoffAngle, 2*PI);
}

template<>
void DrawComponent<ParticleEmitterComponent>(Entity& entity) {
	if(!entity.Has<ParticleEmitterComponent>())
		return;

	auto& component = entity.Set<ParticleEmitterComponent>();
	ImGui::SeparatorText("ParticleEmitterComponent");

	ImGui::Text("Position"); ImGui::SameLine(120.0f);
	ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("##Position", &component.Position.x, 0.1f,
		-FLT_MAX, +FLT_MAX, "%.1f");

	uint64_t min = 3, max = 1000;
	ImGui::Text("Particle Max Count"); ImGui::SameLine(200.0f);
	ImGui::SetNextItemWidth(50);
	ImGui::SliderScalar("##MaxCount", ImGuiDataType_U64,
		&component.MaxParticleCount, &min, &max);

	ImGui::Text("Particle Life Time (ms)"); ImGui::SameLine(200.0f);
	ImGui::SetNextItemWidth(50);
	ImGui::DragFloat("##LifeTime", &component.ParticleLifetime,
		1.0f, 1.0f, 99000.0f, "%.0f");

	ImGui::Text("Particle Spawn Interval (ms)"); ImGui::SameLine(200.0f);
	ImGui::SetNextItemWidth(50);
	ImGui::DragFloat("##SpawnInterval", &component.SpawnInterval,
		1.0f, 1.0f, 99000.0f, "%.0f");

	ImGui::Text("Offset"); ImGui::SameLine(200.0f);
	ImGui::SetNextItemWidth(50);
	ImGui::DragFloat("##Offset", &component.Offset,
		0.01f, 0.0f, 1000.0f, "%.3f");

	ImGui::Text("Material: %llu", (uint64_t)component.MaterialAsset.ID);

	auto text = component.MaterialAsset.ID ? "Change Asset" : "Set Asset";

	auto panel =
		Editor::GetProjectTab()->
				GetPanel("ContentBrowser")->As<ContentBrowserPanel>();

	if(ImGui::Button(text))
		panel->Select(AssetType::Material);
	if(panel->HasSelection())
		component.MaterialAsset = panel->GetSelected();
}

void ComponentEditorPanel::Draw() {
	ImGui::Begin("Component Editor", &Open);
	{
		if(m_Context) {
			if(IsFocused<CameraComponent>(m_Context))
				DrawComponent<CameraComponent>(m_Context);
			else if(IsFocused<TagComponent>(m_Context))
				DrawComponent<TagComponent>(m_Context);
			else if(IsFocused<TransformComponent>(m_Context))
				DrawComponent<TransformComponent>(m_Context);
			else if(IsFocused<AudioComponent>(m_Context))
				DrawComponent<AudioComponent>(m_Context);
			else if(IsFocused<MeshComponent>(m_Context))
				DrawComponent<MeshComponent>(m_Context);
			else if(IsFocused<SkyboxComponent>(m_Context))
				DrawComponent<SkyboxComponent>(m_Context);
			else if(IsFocused<ScriptComponent>(m_Context))
				DrawComponent<ScriptComponent>(m_Context);
			else if(IsFocused<RigidBodyComponent>(m_Context))
				DrawComponent<RigidBodyComponent>(m_Context);
			else if(IsFocused<DirectionalLightComponent>(m_Context))
				DrawComponent<DirectionalLightComponent>(m_Context);
			else if(IsFocused<PointLightComponent>(m_Context))
				DrawComponent<PointLightComponent>(m_Context);
			else if(IsFocused<SpotlightComponent>(m_Context))
				DrawComponent<SpotlightComponent>(m_Context);
			else if(IsFocused<ParticleEmitterComponent>(m_Context))
				DrawComponent<ParticleEmitterComponent>(m_Context);
		}
	}
	ImGui::End();
}

}