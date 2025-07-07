#include "Editor.h"

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
#include <Magma/Audio/AudioEngine.h>
#include <Magma/Physics/Physics.h>
#include <Magma/UI/UIRenderer.h>

#include <Lava/Core/ScriptGlue.h>

#include "Project/ProjectTab.h"

#include "AssetImporter.h"
#include "ScriptManager.h"
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
		bool exportProject = false;
		bool exportProjectTo = false;

		bool addScreen     = false;
	} project;

	struct {
		bool newTab      = false;
		bool newScene    = false;
		bool newUI       = false;
		bool openTab     = false;
		bool openScene   = false;
		bool openUI      = false;
		bool reopenTab   = false;
		bool closeTab    = false;
	} tab;
} static menu;

void Editor::Open() {
	Physics::Init();
	AudioEngine::Init();
	ScriptEngine::Init();

	Lava::ScriptGlue::RegisterInterface();
	m_App = CreateRef<Lava::App>();

	Application::PushDir();
	m_WelcomeImage.Content =
		AssetImporter::GetTexture("Magma/assets/images/VolcanicDisplay.png");
	Application::PopDir();
}

void Editor::Close() {
	CloseProject();
	m_App.reset();

	ScriptEngine::Shutdown();
	AudioEngine::Shutdown();
	Physics::Close();
}

void Editor::Load(const CommandLineArgs& args) {
	if(args["--project"]) {
		NewProject(args["--project"]);

		if(args["--export"]) {
			auto path = (args["--export"].Args[0]);
			ExportProject(path);
			Application::Close();
			return;
		}

		NewTab(CreateRef<ProjectTab>());
	}
	for(auto& path : args["--scene"])
		NewTab(CreateRef<SceneTab>(path));
	for(auto& path : args["--ui"])
		NewTab(CreateRef<UITab>(path));
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
				if(ImGui::MenuItem("Run", "Ctrl+R"))
					menu.project.runProject = true;

				ImGui::Separator();
				if(ImGui::MenuItem("Export"))
					menu.project.exportProject = true;
				if(ImGui::MenuItem("Export To"))
					menu.project.exportProjectTo = true;

				ImGui::Separator();
				if(ImGui::MenuItem("Add Screen"))
					menu.project.addScreen = true;

				ImGui::EndMenu();
			}

			if(GetProjectTab() && ImGui::BeginMenu("Tab")) {
				if(ImGui::MenuItem("New", "Ctrl+T")
				|| Input::KeysPressed(Key::Ctrl, Key::T))
					menu.tab.newTab = true;
				if(ImGui::MenuItem("New Scene Tab")
				|| Input::KeysPressed(Key::Ctrl, Key::T))
					menu.tab.newScene = true;
				if(ImGui::MenuItem("New UI Tab")
				|| Input::KeysPressed(Key::Ctrl, Key::T))
					menu.tab.newUI = true;
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
		if(m_Tabs && ImGui::BeginTabBar("Tabs", tabBarFlags)) {
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

			ImGui::EndTabBar();

			GetProjectTab()->RenderButtons();
		}

		ImGuiID dockspaceID = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

		if(!m_Tabs)
			RenderWelcomeScreen();
		else {
			if(m_CurrentTab != 0)
				GetProjectTab()->RenderEssentialPanels();

			if(GetCurrentTab()->Type != TabType::None)
				GetCurrentTab()->Render();
			else
				RenderEmptyTab(m_Tabs[m_CurrentTab]);
		}
	}
	ImGui::End();

	if(menu.project.newProject)
		NewProject();
	if(menu.project.openProject)
		OpenProject();
	if(menu.project.runProject)
		RunProject();
	if(menu.project.exportProject)
		ExportProject(m_Project.ExportPath);
	if(menu.project.exportProjectTo)
		ExportProject();

	if(menu.tab.newTab)
		NewTab();
	if(menu.tab.newScene)
		NewTab(CreateRef<SceneTab>());
	if(menu.tab.newUI)
		NewTab(CreateRef<UITab>());

	if(menu.tab.openTab)
		OpenTab();
	if(menu.tab.openScene)
		OpenTab(TabType::Scene);
	if(menu.tab.openUI)
		OpenTab(TabType::UI);

	if(menu.tab.reopenTab)
		ReopenTab();
	if(menu.tab.closeTab)
		CloseTab(GetCurrentTab());
}

void Editor::RenderEmptyTab(Ref<Tab>& tab) {
	ImGui::Begin("##Empty");
	{
		if(ImGui::Button("New Scene")) {
			tab.reset();
			tab = CreateRef<SceneTab>();
		}
		if(ImGui::Button("New UI")) {
			tab.reset();
			tab = CreateRef<UITab>();
		}
		if(ImGui::Button("Open Scene")) {
			tab.reset();
			tab = CreateRef<SceneTab>();
			tab->As<SceneTab>()->OpenScene();
		}
		if(ImGui::Button("Open UI")) {
			tab.reset();
			tab = CreateRef<UITab>();
			tab->As<UITab>()->OpenUI();
		}
	}
	ImGui::End();
}

void Editor::RenderWelcomeScreen() {
	auto flags = ImGuiWindowFlags_NoDecoration
			   | ImGuiWindowFlags_NoMove;
	ImGui::Begin("##Welcome", nullptr, flags);
	{
		ImVec2 size = { 300, ImGui::GetContentRegionAvail().y };
		auto childFlags = ImGuiChildFlags_Border;
		ImGui::BeginChild("Options", size, childFlags);
		{
			if(ImGui::Button("Open Project"))
				menu.project.openProject = true;
			if(ImGui::Button("New Project"))
				menu.project.newProject = true;
			// for(auto prev : m_PreviousProjects)
			// 	if(ImGui::Selectable(prev.c_str()))
			// 		NewProject(prev);

		}
		ImGui::EndChild();
		ImGui::SameLine();

		m_WelcomeImage.UsePosition = false;
		m_WelcomeImage.Width = 800;
		m_WelcomeImage.Height = 800;
		m_WelcomeImage.Render();
	}
	ImGui::End();
}

void Editor::SetTab(Ref<Tab> tab) {
	auto title = "Magma Editor: " + m_Project.Name;
	if(tab) {
		auto [_, idx] = m_Tabs.Find([&](auto& val) { return val == tab; });
		m_CurrentTab = idx;
		title += " - " + tab->GetName();
	}

	Application::GetWindow()->SetTitle(title);
}

void Editor::NewTab(Ref<Tab> tab) {
	menu.tab.newScene = false;
	menu.tab.newUI = false;

	s_Instance->m_Tabs.Add(tab);
	s_Instance->SetTab(tab);
}

void Editor::NewTab() {
	menu.tab.newTab = false;
	Ref<Tab> newTab = CreateRef<Tab>();
	newTab->SetName("New Tab");
	NewTab(newTab);
}

void Editor::OpenTab(TabType type) {
	std::string exts;
	if(type == TabType::None)
		exts = ".magma.scene, .magma.ui.json";
	else if(type == TabType::Scene)
		exts = ".magma.scene";
	else if(type == TabType::UI)
		exts = ".magma.ui.json";

	IGFD::FileDialogConfig config;
	config.path = m_Project.Path;
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", exts.c_str(), config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			std::string path = instance->GetFilePathName();
			if(fs::path(path).extension().string() == ".scene")
				NewTab(CreateRef<SceneTab>(path));
			else
				NewTab(CreateRef<UITab>(path));
		}

		instance->Close();
		menu.tab.openTab = false;
		menu.tab.openScene = false;
		menu.tab.openUI = false;
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

	if(idx == m_CurrentTab)
		SetTab(m_Tabs[idx - 1]);
}

void Editor::NewProject() {
	menu.project.newProject = false;

}

void Editor::NewProject(const std::string& volcPath) {
	m_Project.Load(volcPath);

	m_AssetManager.Clear();
	m_AssetManager.Load(m_Project.Path);

	auto rootPath = fs::path(volcPath).parent_path();

	m_App->SetProject(m_Project);
	m_App->AppLoad =
		[this, rootPath](Ref<ScriptModule>& script)
		{
			auto scriptPath = rootPath / "Project" / "App" / m_Project.App;
			script = AssetImporter::GetScript(scriptPath.string() + ".as");
		};
	m_App->ScreenLoad =
		[rootPath](Ref<ScriptModule>& script, const std::string& name)
		{
			auto scriptPath = rootPath / "Project" / "Screen" / name;
			script = AssetImporter::GetScript(scriptPath.string() + ".as");
		};
	m_App->SceneLoad =
		[rootPath](Scene& scene)
		{
			auto scenePath = rootPath / "Visual" / "Scene" / scene.Name;
			SceneLoader::EditorLoad(scene, scenePath.string() + ".magma.scene");
		};
	m_App->UILoad =
		[rootPath](UIPage& page)
		{
			auto uiPath = rootPath / "Visual" / "UI" / page.Name;
			UILoader::EditorLoad(page, uiPath.string() + ".magma.ui.json",
				UITab::GetTheme());
		};

	m_App->ChangeScreen = false;
	m_App->RenderScene = false;
	m_App->RenderUI = false;
	m_App->Running = false;

	auto themePath = rootPath / "Visual" / "UI" / "theme.magma.ui.json";
	if(fs::exists(themePath))
		UITab::GetTheme() = UILoader::LoadTheme(themePath.string());
}

void Editor::OpenProject() {
	IGFD::FileDialogConfig config;
	config.path = ".";
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", ".magma.proj", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			std::string path = instance->GetFilePathName();
			CloseProject();
			NewProject(path);
			NewTab(CreateRef<ProjectTab>());
		}

		instance->Close();
		menu.project.openProject = false;
	}
}

void Editor::RunProject() {
	menu.project.runProject = false;

	std::string command;
#ifdef VOLCANICENGINE_WINDOWS
	command = "Runtime --project ";
	command += m_Project.ExportPath + "\\.volc.proj";
#elif VOLCANICENGINE_LINUX
	command = "Runtime --project ";
	command += m_Project.ExportPath + "/.volc.proj";
#endif
	system(command.c_str());
}

void Editor::CloseProject() {
	if(!GetProjectTab())
		return;

	m_Tabs.Clear();
	m_ClosedTabs.Clear();

	m_AssetManager.Save();
	m_AssetManager.Clear();

	m_Project.Save((fs::path(m_Project.Path) / ".magma.proj").string());
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
		menu.project.exportProjectTo = false;
	}

	if(exportPath == "")
		return;

	ExportProject(exportPath);
}

void Editor::ExportProject(const std::string& exportPath) {
	menu.project.exportProject = false;

	if(fs::is_regular_file(exportPath)) {
		VOLCANICORE_LOG_INFO("'%s' is not a valid directory path");
		return;
	}

	fs::create_directories(exportPath);
	fs::create_directories(fs::path(exportPath) / "Class");
	fs::create_directories(fs::path(exportPath) / "Scene");
	fs::create_directories(fs::path(exportPath) / "UI");

	m_Project.ExportPath = exportPath;
	m_Project.Save((fs::path(exportPath) / ".volc.proj").string());

	for(auto& screen : m_Project.Screens) {
		auto* mod = ScriptManager::LoadScript(
			(fs::path(m_Project.Path) / "Project" / "Screen" / screen.Name
			).string() + ".as", false);
		BinaryWriter writer(
			(fs::path(exportPath) / "Class" / screen.Name).string() + ".class");
		ScriptManager::SaveScript(mod, writer);
	}

	fs::path sceneFolder = fs::path(m_Project.Path) / "Visual" / "Scene";
	for(auto path : FileUtils::GetFiles(sceneFolder.string())) {
		Scene scene;
		SceneLoader::EditorLoad(scene, path);
		SceneLoader::RuntimeSave(scene, m_Project.Path, exportPath);
	}
	fs::path uiFolder = fs::path(m_Project.Path) / "Visual" / "UI";
	for(auto path : FileUtils::GetFiles(uiFolder.string())) {
		if(fs::path(path).filename().string() == "theme.magma.ui.json")
			continue;

		UIPage uiPage;
		UILoader::EditorLoad(uiPage, path, UITab::GetTheme());
		UILoader::RuntimeSave(uiPage, m_Project.Path, exportPath);
	}

	auto* mod = ScriptManager::LoadScript(
		(fs::path(m_Project.Path) / "Project" / "App" / m_Project.App
		).string() + ".as", false);
	BinaryWriter writer((fs::path(exportPath) / ".volc.class").string());
	ScriptManager::SaveScript(mod, writer);

	m_AssetManager.RuntimeSave(exportPath);

	auto runtimeEnv = getenv("VOLC_RUNTIME");
	VOLCANICORE_ASSERT(runtimeEnv);
	std::string runtimePath = runtimeEnv;
	auto target = (fs::path(exportPath) / m_Project.App).string() + ".exe";

	if(FileUtils::FileExists(target))
		fs::remove(target);

	fs::copy_file(runtimePath, target, fs::copy_options::overwrite_existing);
	VOLCANICORE_LOG_INFO("Exported project successfully");
}

}