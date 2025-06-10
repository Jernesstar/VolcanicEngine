#include "LogPanel.h"

#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

namespace fs = std::filesystem;

using namespace VolcaniCore;

namespace Magma {

LogPanel::LogPanel()
	: Panel("Log")
{

}

void LogPanel::Update(TimeStep ts) {

}

void LogPanel::Draw() {
	ImGui::Begin("Log", &Open);

	
	ImGui::End();
}

}