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
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <OpenGL/Texture2D.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/List.h>

#include <VolcaniCore/Event/Events.h>
#include <VolcaniCore/Graphics/RendererAPI.h>

using namespace VolcaniCore;

namespace Magma::UI {

enum class UIType {
	Window,
	ChildWindow,
	DummyWindow,
	MenuBar,
	Menu,
	TabBar,
	Tab
};

static List<UIType> s_Stack;

UIState UIRenderer::DrawWindow(UI::Window& window) {
	if(window.Width == 0 || window.Height == 0) {
		s_Stack.Add(UIType::DummyWindow);
		return { };
	}

	int32_t alignX = 0;
	int32_t alignY = 0;
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	float width = viewport->Size.x;
	float height = viewport->Size.y;
	float x = viewport->Pos.x;
	float y = viewport->Pos.y;

	if(window.GetParent()) {
		UIElement* parent = window.GetParent();
		width = parent->Width;
		height = parent->Height;
		x = parent->x;
		y = parent->y;
	}

	switch(window.xAlignment) {
		case XAlignment::Center:
			alignX = width / 2;
			break;
		case XAlignment::Right:
			alignX = width;
	}
	switch(window.yAlignment) {
		case YAlignment::Center:
			alignY = height / 2;
			break;
		case YAlignment::Bottom:
			alignY = height;
	}

	ImGui::SetNextWindowPos(
		ImVec2{ x + alignX + window.x, y + alignY + window.y });

	if(s_Stack
	&& (s_Stack[-1] == UIType::Window || s_Stack[-1] == UIType::DummyWindow
		|| s_Stack[-1] == UIType::ChildWindow))
	{
		auto childFlags = ImGuiChildFlags_Border
						| ImGuiChildFlags_FrameStyle
						| ImGuiChildFlags_ResizeX
						| ImGuiChildFlags_ResizeY;
		auto windowFlags = ImGuiWindowFlags_NoScrollbar
						 | ImGuiWindowFlags_NoScrollWithMouse
						 | ImGuiWindowFlags_NoTitleBar
						 | ImGuiWindowFlags_NoCollapse;
		ImVec2 size(window.Width, window.Height);

		ImGui::PushStyleColor(ImGuiCol_FrameBg, window.Color);
		ImGui::BeginChild(window.GetID().c_str(), size, childFlags, windowFlags);
		ImGui::PopStyleColor();

		s_Stack.Add(UIType::ChildWindow);
	}
	else {
		auto windowFlags = ImGuiWindowFlags_NoDocking
						 | ImGuiWindowFlags_NoTitleBar
						 | ImGuiWindowFlags_NoCollapse
						 | ImGuiWindowFlags_NoResize
						 | ImGuiWindowFlags_NoMove
						 | ImGuiWindowFlags_NoBringToFrontOnFocus
						 | ImGuiWindowFlags_NoNavFocus;

		ImGui::SetNextWindowSize(ImVec2(window.Width, window.Height));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 10.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGui::PushStyleColor(ImGuiCol_WindowBg, window.Color);
		ImGui::PushStyleColor(ImGuiCol_Border, window.BorderColor);
		ImGui::Begin(window.GetID().c_str(), nullptr, windowFlags);
		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar(3);

		s_Stack.Add(UIType::Window);
	}

	window.Width = ImGui::GetWindowSize().x;
	window.Height = ImGui::GetWindowSize().y;

	// TODO(Implement): WindowState
	return {
		ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered(),
		ImGui::IsWindowHovered(),
		ImGui::IsMouseReleased(0),
		ImGui::IsMouseDown(0),
	};
}

static void (*ButtonFunction)(Ref<UIElement>, ImVec2);

static void ButtonText(Ref<UIElement> element, ImVec2 dim) {
	std::string text = element->As<Text>()->Content;
	ImGui::Button(text.c_str(), dim);
}

static void ButtonImage(Ref<UIElement> element, ImVec2 dim) {
	auto tex = element->As<Image>()->Content->As<OpenGL::Texture2D>();
	auto id = (ImTextureID)(intptr_t)tex->GetID();
	ImGui::ImageButton(id, dim);
}

UIState UIRenderer::DrawButton(UI::Button& button) {
	if(button.Display->GetType() == UIElementType::Image)
		ButtonFunction = ButtonImage;
	else {
		ImGui::PushStyleColor(ImGuiCol_Button, button.Color);
		ButtonFunction = ButtonText;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
	if(button.x >= 0 && button.y >= 0)
		ImGui::SetCursorPos(ImVec2(button.x, button.y));
	ButtonFunction(button.Display, ImVec2(button.Width, button.Height));
	ImGui::PopStyleVar();

	if(button.Display->GetType() == UIElementType::Text)
		ImGui::PopStyleColor();

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseReleased(0),
		ImGui::IsMouseDown(0),
	};
}

UIState UIRenderer::DrawImage(UI::Image& image) {
	auto texture = image.Content->As<OpenGL::Texture2D>();

	ImVec2 dim = ImVec2(image.Width, image.Height);
	if(image.x != -1 && image.y != -1)
		ImGui::SetCursorPos(ImVec2(image.x, image.y));
	ImGui::Image((ImTextureID)(intptr_t)texture->GetID(), dim);

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseReleased(0),
		ImGui::IsMouseDown(0),
	};
}

UIState UIRenderer::DrawText(UI::Text& text) {
	if(text.Content == "")
		return { };

	ImVec2 size = ImGui::CalcTextSize(text.Content.c_str());
	text.Width = size.x;
	text.Height = size.y;

	ImGui::PushStyleColor(ImGuiCol_Text, text.Color);
	ImGui::SetCursorPos(ImVec2(text.x, text.y));
	ImGui::Text(text.Content.c_str());
	ImGui::PopStyleColor();

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseReleased(0),
		ImGui::IsMouseDown(0),
	};
}

UIState UIRenderer::DrawTextInput(TextInput& textInput) {
	// char input[textInput.GetMaxCharCount()]{""};
	char input[255]{ "" };

	ImGui::InputText(textInput.GetID().c_str(), input, sizeof(input));
	textInput.Text = std::string(input);

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseReleased(0),
		ImGui::IsMouseDown(0),
	};
}

UIState UIRenderer::DrawDropdown(Dropdown& dropdown) {
	if(ImGui::BeginCombo("##Combo", dropdown.GetSelected().c_str()))
	{
		for(uint32_t i = 0; i < dropdown.Options.Count(); i++) {
			auto item = dropdown.Options[i];
			bool isSelected = dropdown.CurrentItem == i;
			if(ImGui::Selectable(item.c_str(), isSelected))
				dropdown.CurrentItem = i;
			if(isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	return {
		// ImGui::IsItemClicked(),
		// ImGui::IsItemHovered(),
		// ImGui::IsMouseReleased(0),
		// ImGui::IsMouseDown(0),
	};
}

UIState UIRenderer::DrawMenuBar(const std::string& name) {
	ImGui::BeginMainMenuBar();

	s_Stack.Add(UIType::MenuBar);

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseReleased(0),
		ImGui::IsMouseDown(0),
	};
}

UIState UIRenderer::DrawMenu(const std::string& name) {
	ImGui::BeginMenu(name.c_str());

	s_Stack.Add(UIType::Menu);

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseReleased(0),
		ImGui::IsMouseDown(0),
	};
}

UIState UIRenderer::DrawTabBar(const std::string& name) {
	s_Stack.Add(UIType::TabBar);

	ImGui::BeginTabBar(name.c_str(), ImGuiTabBarFlags_Reorderable);

	return {
		ImGui::IsItemClicked(),
		ImGui::IsItemHovered(),
		ImGui::IsMouseReleased(0),
		ImGui::IsMouseDown(0),
	};
}

TabState UIRenderer::DrawTab(const std::string& name, bool closeButton) {
	s_Stack.Add(UIType::Tab);

	ImVec2 size = ImGui::CalcTextSize(name.c_str());
	float padding = closeButton ? 24.0f : 10.0f;
	float tabHeight = 7.0f;
	float radius = closeButton ? tabHeight * 0.5f : 0;

	ImGui::SetNextItemWidth(size.x + (2.0f*radius + padding));

	ImGui::PushID(s_Stack.Count());
	bool tabItem = ImGui::BeginTabItem(name.c_str());
	ImGui::PopID();

	ImVec2 pos;
	pos.x = ImGui::GetItemRectMax().x - 6.0f*radius;
	pos.y = ImGui::GetItemRectMin().y + radius;

	ImGuiTabBar* tabBar = ImGui::GetCurrentTabBar();

	TabState state;
	if(tabItem) {
		state.Clicked = ImGui::IsItemClicked(0);
		state.Hovered = ImGui::IsItemHovered();
		ImGui::EndTabItem();
	}

	if(closeButton) {
		auto closeButtonID = ImGui::GetID((int*)s_Stack.Count());
		state.Closed = ImGui::CloseButton(closeButtonID, pos);
	}

	return state;
}

void UIRenderer::ShowPopupLabel(const std::string& str) {
	ImGui::SetItemTooltip(str.c_str());
}

void UIRenderer::Pop(uint32_t count) {
	count = count ? count : s_Stack.Count();
	while(count--) {
		auto type = s_Stack.Pop();

		switch(type) {
			case UIType::Window:
				ImGui::End();
				break;
			case UIType::ChildWindow:
				ImGui::EndChild();
				break;
			case UIType::DummyWindow:
				break;
			case UIType::MenuBar:
				ImGui::EndMainMenuBar();
				break;
			case UIType::Menu:
				ImGui::EndMenu();
				break;
			case UIType::TabBar:
				ImGui::EndTabBar();
				break;
		}
	}
}

void UIRenderer::BeginFrame() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
}

void UIRenderer::EndFrame() {
	Pop(0);

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
}

void UIRenderer::Close() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

}