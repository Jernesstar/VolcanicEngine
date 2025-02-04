#include "App.h"

#include <Magma/UI/UIRenderer.h>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/DLL.h>

#include "UILoader.h"
#include "UIObject.h"

using namespace Magma::UI;

namespace Lava {

void App::OnLoad() {
	UIRenderer::Init();

	// ScriptEngine::RegisterSingleton

	// auto list = UILoader::LoadFolder(project);
	// for(auto& screenName : project.Screens) {
	// 	auto& screen = m_Screens.Emplace(screenName);

	// 	auto dll = UILoader::GetDLL(screen.Name);
	// 	if(!dll) {
	// 		VOLCANICORE_LOG_WARNING(
	// 			"DLL Functions not found for page: %s", screen.Name);
	// 		continue;
	// 	}

	// 	auto load = dll->GetFunction<void, Screen*>("LoadObjects");
	// 	load(&screen);
	// 	s_DLLs[screen.Name] = dll;
	// }
}

void App::OnClose() {
	UIRenderer::Close();
}

void App::OnUpdate(TimeStep ts) {
	if(m_CurrentScreen == -1)
		return;

	auto& screen = m_Screens[m_CurrentScreen];

	// screen.GetScene().OnRender();

	UIRenderer::BeginFrame();

	// auto dll = s_DLLs[screen.Name];
	// if(!dll)
	// 	return;

	// auto get = dll->GetFunction<UIObject*, std::string>("GetObject");

	// screen.GetPage().Traverse(
	// 	[&](UIElement* element)
	// 	{
	// 		UIObject* object = get(element->GetID());
	// 		if(!object)
	// 			return;

	// 		object->OnUpdate(ts);

	// 		UIState state = element->GetState();
	// 		if(state.Clicked)
	// 			object->OnClick();
	// 		if(state.Hovered)
	// 			object->OnHover();
	// 		if(state.MouseUp)
	// 			object->OnMouseUp();
	// 		if(state.MouseDown)
	// 			object->OnMouseDown();
	// 	});

	// screen.GetPage().Render();

	UIRenderer::EndFrame();
}

void App::SetScreen(const std::string& name) {

}

}