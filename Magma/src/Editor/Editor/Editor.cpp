#include "Editor.h"

#include <cstdlib>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/FileUtils.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/Input.h>
#include <VolcaniCore/Graphics/RendererAPI.h>

#include <Magma/Script/ScriptEngine.h>

#include <Magma/Physics/Physics.h>

#include <Magma/UI/UIRenderer.h>

#include <Lava/ScriptGlue.h>

#include "Project/ProjectTab.h"

#include "SceneLoader.h"
#include "UILoader.h"

using namespace VolcaniCore;
using namespace Magma::UI;

namespace fs = std::filesystem;

namespace Magma {
struct {
	struct {
		bool newProject    = false;
		bool openProject   = false;
		bool runProject    = false;
		bool saveProject   = false;
		bool exportProject = false;
	} project;

	struct {
		bool newTab    = false;
		bool openTab   = false;
		bool reopenTab = false;
		bool closeTab  = false;
	} tab;
} static menu;

Editor::Editor() {
	Physics::Init();
	ScriptEngine::Init();
	Lava::ScriptGlue::RegisterInterface();
}

void Editor::Load(const CommandLineArgs& args) {
	if(args["--project"]) {
		NewProject(args["--project"]);

		if(args["--export"]) {
			ExportProject(args["--export"]);
			VOLCANICORE_LOG_INFO("Exported project successfully");
			Application::Close();
		}

		NewTab(CreateRef<ProjectTab>(m_Project.Path));
	}
	for(auto& path : args["--scene"])
		NewTab(CreateRef<SceneTab>(path));
	for(auto& path : args["--ui"])
		NewTab(CreateRef<UITab>(path));
}

Editor::~Editor() {
	m_Tabs.Clear();

	ScriptEngine::Shutdown();
	Physics::Close();
}

void Editor::Update(TimeStep ts) {
	for(auto tab : m_Tabs)
		tab->Update(ts);
}

void Editor::Render() {
	bool dockspaceOpen = true;
	ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;

	windowFlags |= ImGuiWindowFlags_NoDocking
				 | ImGuiWindowFlags_NoCollapse
				 | ImGuiWindowFlags_NoNavFocus
				 | ImGuiWindowFlags_NoTitleBar
				 | ImGuiWindowFlags_NoResize
				 | ImGuiWindowFlags_NoMove
				 | ImGuiWindowFlags_NoBackground
				 | ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("DockSpaceWindow", &dockspaceOpen, windowFlags);
	{
		ImGui::PopStyleVar(3);

		ImGui::BeginMainMenuBar();
		{
			if(ImGui::BeginMenu("Project")) {
				if(ImGui::MenuItem("New", "Ctrl+N"))
					menu.project.newProject = true;
				if(ImGui::MenuItem("Open", "Ctrl+P"))
					menu.project.openProject = true;
				if(ImGui::MenuItem("Save", "Ctrl+S")
				|| Input::KeysPressed(Key::Ctrl, Key::S))
					menu.project.saveProject = true;
				if(ImGui::MenuItem("Run", "Ctrl+R"))
					menu.project.runProject = true;

				ImGui::Separator();
				if(ImGui::MenuItem("Export"))
					menu.project.exportProject = true;

				ImGui::EndMenu();
			}

			if(ImGui::BeginMenu("Tab")) {
				if(ImGui::MenuItem("New", "Ctrl+T")
				|| Input::KeysPressed(Key::Ctrl, Key::T))
					menu.tab.newTab = true;
				if(ImGui::MenuItem("New Scene Tab")
				|| Input::KeysPressed(Key::Ctrl, Key::T))
					menu.tab.newTab = true;
				if(ImGui::MenuItem("New UI Tab")
				|| Input::KeysPressed(Key::Ctrl, Key::T))
					menu.tab.newTab = true;
				ImGui::Separator();

				if(ImGui::MenuItem("Open", "Ctrl+O")
				|| Input::KeyPressed(Key::O))
					menu.tab.openTab = true;
				if(ImGui::MenuItem("Reopen", "Ctrl+Shift+T")
				|| Input::KeysPressed(Key::Ctrl, Key::Shift, Key::T))
					menu.tab.reopenTab = true;
				if(ImGui::MenuItem("Close", "Ctrl+W")
				|| Input::KeysPressed(Key::Ctrl, Key::W))
					menu.tab.closeTab = true;

				ImGui::EndMenu();
			}
		}
		ImGui::EndMainMenuBar();

		auto tabBarFlags = ImGuiTabBarFlags_Reorderable
						 | ImGuiTabBarFlags_TabListPopupButton;
		ImGui::BeginTabBar("Tabs", tabBarFlags);
		{
			auto leadingFlags = ImGuiTabItemFlags_Trailing
							  | ImGuiTabItemFlags_NoReorder;
			if(ImGui::TabItemButton("+", leadingFlags))
				menu.tab.newTab = true;

			Ref<Tab> tabToDelete = nullptr;
			for(auto tab : m_Tabs) {
				bool closeButton = tab->Type != TabType::Project;
				TabState state =
					UIRenderer::DrawTab(tab->GetName(), closeButton);
				if(state.Closed)
					tabToDelete = tab;
				else if(state.Clicked)
					SetTab(tab);
			}

			if(tabToDelete != nullptr)
				CloseTab(tabToDelete);
		}
		ImGui::EndTabBar();
		// ImGui::SameLine();
		// if(m_Tabs)
		// 	GetProjectTab()->RenderButtons();

		ImGuiID dockspaceID = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

		if(m_Tabs)
			GetProjectTab()->RenderEssentialPanels();
		if(m_CurrentTab) {
			if(m_CurrentTab->Type != TabType::None)
				m_CurrentTab->Render();
			else
				RenderEmptyTab();
		}
	}
	ImGui::End();

	if(menu.project.newProject)
		NewProject();
	if(menu.project.openProject)
		OpenProject();
	if(menu.project.runProject)
		RunProject();
	if(menu.project.saveProject)
		SaveProject();
	if(menu.project.exportProject)
		ExportProject();

	if(menu.tab.newTab)
		NewTab();
	if(menu.tab.openTab)
		OpenTab();
	if(menu.tab.reopenTab)
		ReopenTab();
	if(menu.tab.closeTab)
		CloseTab(m_CurrentTab);
}

void Editor::RenderEmptyTab() {

}

void Editor::RenderBackground() {

}

void Editor::SetTab(Ref<Tab> tab) {
	m_CurrentTab = tab;
	auto title = "Magma Editor: " + m_Project.Name;
	if(tab)
		title += " - " + tab->GetName();

	Application::GetWindow()->SetTitle(title);
}

void Editor::NewTab(Ref<Tab> tab) {
	m_Tabs.Add(tab);
	SetTab(tab);
}

void Editor::NewTab() {
	menu.tab.newTab = false;
	Ref<Tab> newTab = CreateRef<Tab>();
	newTab->SetName("New Tab");
	NewTab(newTab);
}

void Editor::OpenTab() {
	IGFD::FileDialogConfig config;
	config.path = m_Project.Path;
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File",
						 ".magma.scene, .magma.ui.json", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			fs::path path = instance->GetFilePathName();
			if(path.extension() == ".json")
				NewTab(CreateRef<UITab>(path.string()));
			else
				NewTab(CreateRef<SceneTab>(path.string()));
		}

		instance->Close();
		menu.tab.openTab = false;
	}
}

void Editor::ReopenTab() {
	menu.tab.reopenTab = false;

	if(m_ClosedTabs)
		NewTab(m_ClosedTabs.Pop());
}

void Editor::CloseTab(Ref<Tab> tabToDelete) {
	menu.tab.closeTab = false;
	if(tabToDelete == nullptr)
		return;

	auto [_, idx] = m_Tabs.Find([&](auto& val) { return val == tabToDelete; });
	m_ClosedTabs.Add(m_Tabs.Pop(idx));

	if(tabToDelete == m_CurrentTab)
		SetTab(idx > 0 ? m_Tabs[idx - 1] : nullptr);
}

void Editor::NewProject() {
	menu.project.newProject = false;
	m_Project = Project();
	Application::GetWindow()->SetTitle("Magma Editor");
}

void Editor::NewProject(const std::string& volcPath) {
	m_Project.Load(volcPath);

	m_AssetManager.Load(volcPath);
	m_AssetManager.Reload();

	auto rootPath = fs::path(volcPath).parent_path();
	m_App = CreateRef<Lava::App>(m_Project);

	m_App->ScreenLoad =
		[&](Ref<ScriptModule> script)
		{
			auto scriptPath = rootPath / "Project" / "Screen" / script->Name;
			script->Load(scriptPath.string() + ".as");
		};
	m_App->SceneLoad =
		[&](Scene& scene)
		{
			auto scenePath = rootPath / "Visual" / "Scene" / scene.Name;
			SceneLoader::EditorLoad(scene, scenePath.string() + ".magma.scene");
		};
	m_App->UILoad =
		[&](UIPage& page)
		{
			auto uiPath = rootPath / "Visual" / "UI" / page.Name;
			UILoader::EditorLoad(page, uiPath.string() + ".magma.ui.json",
				UITab::GetTheme());
		};

	auto themePath = rootPath / "Visual" / "UI" / "theme.magma.ui.json";
	if(fs::exists(themePath))
		UITab::GetTheme() = UILoader::LoadTheme(themePath.string());
}

void Editor::OpenProject() {
	IGFD::FileDialogConfig config;
	config.path = m_Project.Path;
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", ".magma.proj", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			std::string path = instance->GetFilePathName();
			NewProject(path);
			NewTab(CreateRef<ProjectTab>(m_Project.Path));
		}

		instance->Close();
		menu.project.openProject = false;
	}
}

void Editor::RunProject() {
	menu.project.runProject = false;

	std::string command;
#ifdef VOLCANICENGINE_WINDOWS
	command = ".\\build\\Lava\\bin\\Runtime --project ";
	command += m_Project.ExportPath + "\\.volc.proj";
#elif VOLCANICENGINE_LINUX
	command = "./build/Lava/bin/Runtime --project ";
	command += m_Project.ExportPath + "/.volc.proj";
#endif
	system(command.c_str());
}

void Editor::SaveProject() {
	menu.project.saveProject = false;

}

void Editor::ExportProject() {
	IGFD::FileDialogConfig config;
	config.path = ".";
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseDir", "Choose Directory", nullptr, config);

	std::string exportPath = "";
	if(instance->Display("ChooseDir")) {
		if(instance->IsOk())
			exportPath = instance->GetCurrentPath();

		instance->Close();
		menu.project.exportProject = false;
	}

	if(exportPath == "")
		return;

	ExportProject(exportPath);
}

void Editor::ExportProject(const std::string& exportPath) {
	fs::create_directories(exportPath);
	fs::create_directories(fs::path(exportPath) / "Class");
	fs::create_directories(fs::path(exportPath) / "Scene");
	fs::create_directories(fs::path(exportPath) / "UI");

	m_Project.ExportPath = exportPath;
	m_Project.Save((fs::path(exportPath) / ".volc.proj").string());

	for(auto& screen : m_Project.Screens) {
		auto mod = CreateRef<ScriptModule>(screen.Name);
		mod->Load(
			(fs::path(m_Project.Path) / "Project" / "Screen" / screen.Name
			).string() + ".as");
		mod->Save(
			(fs::path(exportPath) / "Class" / screen.Name).string() + ".class");

		if(screen.Scene != "") {
			auto path =
				fs::path(m_Project.Path) / "Visual" / "Scene" / screen.Scene;
			Scene scene;
			SceneLoader::EditorLoad(scene, path.string() + ".magma.scene");
			SceneLoader::RuntimeSave(scene, m_Project.Path, exportPath);
		}
		if(screen.UI != "") {
			auto path = fs::path(m_Project.Path) / "Visual" / "UI" / screen.UI;
			UIPage uiPage;
			UILoader::EditorLoad(uiPage, path.string() + ".magma.ui.json",
								 UITab::GetTheme());
			UILoader::RuntimeSave(uiPage, m_Project.Path, exportPath);
		}
	}

	auto mod = CreateRef<ScriptModule>(m_Project.App);
	mod->Load(
		(fs::path(m_Project.Path) / "Project" / "App" / m_Project.App
		).string() + ".as");
	mod->Save((fs::path(exportPath) / ".volc.class").string());

	m_AssetManager.RuntimeSave(exportPath);

	auto runtimeEnv = getenv("VOLC_RUNTIME");
	VOLCANICORE_ASSERT(runtimeEnv);
	std::string runtimePath = runtimeEnv;
	auto target = (fs::path(exportPath) / m_Project.App).string() + ".exe";

	if(FileUtils::FileExists(target))
		fs::remove(target);

	fs::copy_file(runtimePath, target, fs::copy_options::overwrite_existing);
}

}