#include "UIRenderer.h"

#define IM_VEC2_CLASS_EXTRA \
	constexpr ImVec2(glm::vec2& v) : x(v.x), y(v.y) {} \
	operator glm::vec2() const { return glm::vec2(x, y); }

#define IM_VEC3_CLASS_EXTRA \
	constexpr ImVec3(glm::vec3& v) : x(v.x), y(v.y), z(v.z) {} \
	operator glm::vec3() const { return glm::vec3(x, y, z); }

#define IM_VEC4_CLASS_EXTRA \
	constexpr ImVec4(const glm::vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {} \
	operator glm::vec4() const { return glm::vec4(x, y, z, w); }

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <OpenGL/Texture2D.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Event/Events.h>
#include <VolcaniCore/Renderer/RendererAPI.h>

using namespace VolcaniCore;

namespace Magma::UI {

static void ButtonText(Ref<UIElement>, ImVec2);
static void ButtonImage(Ref<UIElement>, ImVec2);

static void (*ButtonFunction)(Ref<UIElement>, ImVec2);

enum class UIElementType { Window, MenuBar, Menu, TabBar, Tab };

static List<UIElementType> s_Types; // Elements that need ImGui::End to be called

UIState UIRenderer::DrawWindow(UI::Window& window) {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + window.x, viewport->Pos.y + window.y));
	ImGui::SetNextWindowSize(ImVec2(window.Width, window.Height));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
						ImVec2(window.BorderWidth, window.BorderHeight));

	ImGui::PushStyleColor(ImGuiCol_WindowBg, window.Color);

	ImGuiWindowFlags windowFlags;
	windowFlags |= ImGuiWindowFlags_NoDocking
				 | ImGuiWindowFlags_NoTitleBar
				 | ImGuiWindowFlags_NoCollapse
				 | ImGuiWindowFlags_NoResize
				 | ImGuiWindowFlags_NoMove
				 | ImGuiWindowFlags_NoBringToFrontOnFocus
				 | ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin(window.GetID().c_str(), &window.Open, windowFlags);
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(3);
	s_Types.push_back(UIElementType::Window);

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseDown(0),
		// ImGui::IsItemDeactivated()
	};
}

void ButtonText(Ref<UIElement> element, ImVec2 dim) {
	std::string text = element->As<Text>()->Content;
	ImGui::Button(text.c_str(), dim);
}

void ButtonImage(Ref<UIElement> element, ImVec2 dim) {
	// auto tex = element->As<Image>()->Content->As<OpenGL::Texture2D>();
	// auto id = (ImTextureID)(intptr_t)tex->GetID();
	// ImGui::ImageButton(id, dim);
}

UIState UIRenderer::DrawButton(UI::Button& button) {
	if(button.Display->GetType() == UIElement::Type::Image)
		ButtonFunction = ButtonImage;
	else
		ButtonFunction = ButtonText;

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Button, button.Color);
	ImGui::SetCursorPos(ImVec2(button.x, button.y));

	ButtonFunction(button.Display, ImVec2(button.Width, button.Height));
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseDown(0),
		// ImGui::IsItemDeactivated()
	};
}

UIState UIRenderer::DrawImage(UI::Image& image) {
	auto texture = image.Content->As<OpenGL::Texture2D>();

	auto v0 = ImVec2(0.0f, 1.0f);
	auto v1 = ImVec2(1.0f, 0.0f);
	ImVec2 dim = ImVec2(image.Width, image.Height);
	ImGui::SetCursorPos(ImVec2(image.x, image.y));
	ImGui::Image((ImTextureID)(intptr_t)texture->GetID(), dim, v0, v1);

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseDown(0),
		// ImGui::IsItemDeactivated()
	};
}

UIState UIRenderer::DrawText(UI::Text& text) {
	ImVec2 size = ImGui::CalcTextSize(text.Content.c_str());
	text.Width = size.x;
	text.Height = size.y;

	ImVec4 color{ text.Color.r, text.Color.g, text.Color.b, text.Color.a };
	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::SetCursorPos(ImVec2(text.x, text.y));

	ImGui::Text(text.Content.c_str());
	ImGui::PopStyleColor();

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseDown(0),
		// ImGui::IsItemDeactivated()
	};
}

UIState UIRenderer::DrawTextInput(TextInput& textInput) {
	char input[textInput.MaxCharCount]{""};
	// char input[255]{ "" };

	ImGui::InputText(textInput.GetID().c_str(), input, sizeof(input));
	textInput.Text = std::string(input);

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseDown(0),
		// ImGui::IsItemDeactivated()
	};
}

UIState UIRenderer::DrawDropdown(Dropdown& dropdown) {
	uint32_t n = dropdown.Options.size();
	//const char* items[n];
	const char** items = new const char*[255];
	// TODO(Implement): Images
	for(uint32_t i = 0; i < n; i++)
		items[i] = dropdown.Options[i].c_str();

	if(ImGui::BeginCombo("##Combo", dropdown.m_CurrentItem))
	{
		for(uint32_t i = 0; i < n; i++) {
			bool isSelected = (dropdown.m_CurrentItem == items[i]);
			if(ImGui::Selectable(items[i], isSelected))
				dropdown.m_CurrentItem = items[i];
			if(isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	return {
		// ImGui::IsItemClicked(),
		// ImGui::IsItemHovered(),
		// ImGui::IsMouseDown(0),
		// ImGui::IsItemDeactivated()
	};
}

UIState UIRenderer::DrawMenuBar(const std::string& name) {
	ImGui::BeginMainMenuBar();

	s_Types.push_back(UIElementType::MenuBar);

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseDown(0),
		// ImGui::IsItemDeactivated()
	};
}

UIState UIRenderer::DrawMenu(const std::string& name) {
	ImGui::BeginMenu(name.c_str());

	s_Types.push_back(UIElementType::Menu);

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseDown(0),
		// ImGui::IsItemDeactivated()
	};
}

UIState UIRenderer::DrawTabBar(const std::string& name) {
	ImGui::BeginTabBar(name.c_str(), ImGuiTabBarFlags_Reorderable);

	s_Types.push_back(UIElementType::TabBar);

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseDown(0),
		// ImGui::IsItemDeactivated()
	};
}

UIState UIRenderer::DrawTab(const std::string& name) {
	s_Types.push_back(UIElementType::Tab);

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseDown(0),
		// ImGui::IsItemDeactivated()
	};
}

void UIRenderer::BeginFrame() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
}

void UIRenderer::EndFrame() {
	while(s_Types.size() > 0) {
		auto type = s_Types.back();
		s_Types.pop_back();

		switch(type) {
			case UIElementType::Window:
				ImGui::End();
				break;
			case UIElementType::MenuBar:
				ImGui::EndMainMenuBar();
				break;
			case UIElementType::Menu:
				ImGui::EndMenu();
				break;
			case UIElementType::TabBar:
				ImGui::EndTabBar();
				break;
			case UIElementType::Tab:
				ImGui::EndTabItem();
		}
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	GLFWwindow* backup_current_context = glfwGetCurrentContext();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	glfwMakeContextCurrent(backup_current_context);
}

void UIRenderer::Init() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	auto window = Application::GetWindow();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable
					| ImGuiConfigFlags_ViewportsEnable
					| ImGuiConfigFlags_NavEnableKeyboard
					| ImGuiConfigFlags_NavEnableSetMousePos;
	io.DisplaySize = ImVec2(window->GetWidth(), window->GetHeight());

	if(RendererAPI::Get()->GetBackend() == RendererAPI::Backend::OpenGL) {
		ImGui_ImplGlfw_InitForOpenGL(window->GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}

	Events::RegisterListener<KeyPressedEvent>(
		[](KeyPressedEvent& event)
		{
			ImGuiIO& io = ImGui::GetIO();
			// event.Handled = io.WantCaptureKeyboard;
		});
	Events::RegisterListener<KeyReleasedEvent>(
		[](KeyReleasedEvent& event)
		{
			ImGuiIO& io = ImGui::GetIO();
			// event.Handled = io.WantCaptureKeyboard;
		});
	Events::RegisterListener<MouseButtonPressedEvent>(
		[](MouseButtonPressedEvent& event)
		{
			ImGuiIO& io = ImGui::GetIO();
			// event.Handled = io.WantCaptureMouse;
		});
	Events::RegisterListener<MouseButtonReleasedEvent>(
		[](MouseButtonReleasedEvent& event)
		{
			ImGuiIO& io = ImGui::GetIO();
			// event.Handled = io.WantCaptureMouse;
		});
	Events::RegisterListener<MouseScrolledEvent>(
		[](MouseScrolledEvent& event)
		{
			ImGuiIO& io = ImGui::GetIO();
			// event.Handled = io.WantCaptureMouse;
		});
	Events::RegisterListener<MouseMovedEvent>(
		[](MouseMovedEvent& event)
		{
			ImGuiIO& io = ImGui::GetIO();
			// event.Handled = io.WantCaptureMouse;
		});
	Events::RegisterListener<WindowResizedEvent>(
		[](const WindowResizedEvent& event)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2(event.Width, event.Height);
		});

	ImGui::StyleColorsDark();

	Application::PushDir();
	float fontSize = 18.0f;
	io.Fonts->AddFontFromFileTTF(
		"VolcaniCore/assets/fonts/JetBrainsMono-Bold.ttf", fontSize);
	io.FontDefault =
		io.Fonts->AddFontFromFileTTF(
			"VolcaniCore/assets/fonts/JetBrainsMono-Regular.ttf", fontSize);
	Application::PopDir();
}

void UIRenderer::Close() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

}