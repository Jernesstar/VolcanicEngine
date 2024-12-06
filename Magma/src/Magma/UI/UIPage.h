#pragma once

#include "UI.h"

#include "Core/DLL.h"

namespace Magma::UI {

using UINode = std::pair<UIElement::Type, uint32_t>;

class UIPage {
public:
	bool Visible = true;

public:
	UIPage() = default;
	UIPage(const std::string& filePath);
	~UIPage() = default;

	void Load(const std::string& filePath);
	void Reload();

	void Update(TimeStep ts);
	void Render();

	void OnEvent(const std::string& id, const UIState& state);

	UINode Add(UIElement::Type type, const std::string& id);
	void Add(const UINode& node) {
		m_FirstOrders.push_back(node);
	}

	template<typename TUIElement, typename ...Args>
	requires std::derived_from<TUIElement, UIElement>
	UINode Add(Args&&... args) {
		auto node = AddNew(std::forward<Args>(args)...);
		m_FirstOrders.push_back(node);
		return node;
	}

	template<typename TUIElement, typename ...Args>
	requires std::derived_from<TUIElement, UIElement>
	UINode AddNew(Args&&... args) {
		auto& list = GetList<TUIElement>();
		list.emplace_back(std::forward<Args>(args)...);
		return { GetType<TUIElement>(), list.size() };
	}

	UIElement* Get(const UINode& node) const;
	UIElement* Get(const std::string& id) const;
	List<UIElement*> GetFirstOrderElements() const;
	std::string GetPath() const { return m_Path; }
	std::string GetName() const { return m_Name; }

	// void SetTheme();

private:
	List<Window> Windows;
	List<Button> Buttons;
	List<Dropdown> Dropdowns;
	List<Text> Texts;
	List<TextInput> TextInputs;
	List<Image> Images;

	List<UINode> m_FirstOrders;
	Map<std::string, UIState> m_States;

	std::string m_Path;
	std::string m_Name;
	Ref<DLL> m_GenFile;

private:
	template<typename TUIType>
	List<TUIType>& GetList();
	template<typename TUIType>
	UIElement::Type GetType();

	void UpdateElement(TimeStep ts, UIElement* element);
};

}