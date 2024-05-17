#include "EditorLayer.h"

namespace Magma {

EditorLayer::EditorLayer() {

}

EditorLayer::~EditorLayer() {
	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_MenuBar);
	{
		if(ImGui::BeginMenuBar()) {
			if(ImGui::BeginMenu("File")) {
				if(ImGui::MenuItem("Open", "Ctrl+O")) {
					return;
				}
				if(ImGui::MenuItem("Save", "Ctrl+S")) {
					return;
				}
				if(ImGui::MenuItem("Close", "Ctrl+W")) {
					return;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	if (ImGui::Button("Play"))
		return;
}

}