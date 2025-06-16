#include "ConsolePanel.h"

#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

namespace fs = std::filesystem;

using namespace VolcaniCore;

namespace Magma {

ConsolePanel::ConsolePanel()
	: Panel("Console")
{

}

void ConsolePanel::Update(TimeStep ts) {

}

void ConsolePanel::Draw() {
	ImGui::Begin("Console", &Open);


	ImGui::End();
}

}