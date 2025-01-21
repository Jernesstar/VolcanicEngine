#pragma once

using namespace Magma::UI;

namespace Demo {

class UI : public Application {
public:
	UI();
	~UI();

	void OnUpdate(TimeStep ts);

private:
	UIPage* page;
	UINode node;
};

UI::UI()
	: Application(1920, 1080)
{
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	UIRenderer::Init();

	page = new UIPage();

	node = page->Add(UIElementType::Window, "TEST_PANEL");
	page->ClearFirstOrders();
	page->Add(node);

	auto node1 = page->Add(UIElementType::Window, "Image");
	auto node2 = page->Add(UIElementType::Button, "Button");

	page->Get(node)->Add(node1);
	page->Get(node1)->Add(node2);

	auto root = page->Get(node);

	root->GetChild("Image")->GetParent()->GetID();
	root->GetChild("Image")->GetChild("Button")->GetParent()->GetParent()->GetID();
}

UI::~UI() {
	UIRenderer::Close();
}

void UI::OnUpdate(TimeStep ts) {
	Renderer::Clear();

	UIRenderer::BeginFrame();
	ImGui::ShowDemoWindow();
	UIRenderer::EndFrame();
}

}