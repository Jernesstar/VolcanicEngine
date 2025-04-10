#pragma once

#include <glm/vec4.hpp>

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/Template.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/UUID.h>

#include <Magma/Script/ScriptObject.h>

namespace Magma::UI {

class UIPage;

struct UIState {
	bool Clicked = false;
	bool Hovered = false;
	bool MouseUp = false;
	bool MouseDown = false;
};

enum class XAlignment { Left, Center, Right };
enum class YAlignment { Top, Center, Bottom };

enum class UIElementType {
	Empty,
	Window,
	Button,
	Dropdown,
	Text,
	TextInput,
	Image,
	None
};

using UINode = std::pair<UIElementType, uint32_t>;

class UIElement : public VolcaniCore::Derivable<UIElement> {
public:
	uint32_t Width = 0;
	uint32_t Height = 0;
	float x = 0;
	float y = 0;
	XAlignment xAlignment = XAlignment::Left;
	YAlignment yAlignment = YAlignment::Top;
	bool UsePosition = true;
	glm::vec4 Color = glm::vec4(0.0f);
	// TODO(Implement): Border
	VolcaniCore::List<UINode> Children;
	std::string Class = "";
	VolcaniCore::UUID ModuleID = 0;
	Ref<Script::ScriptObject> ScriptInstance = nullptr;
	// std::string Layer = "Root";

public:
	UIElement(UIElementType type)
		: m_Type(type), m_ID(""), m_Root(nullptr) { }
	UIElement(UIElementType type, const std::string& id, UIPage* root)
		: m_Type(type), m_ID(id), m_Root(root) { }
	virtual ~UIElement() = default;

	virtual void Draw() = 0;
	void Render();

	UINode Add(UIElementType type, const std::string& id);
	void Add(const UINode& node);

	UIState GetState() const { return m_State; }

	UIElement& SetSize(uint32_t width, uint32_t height);
	UIElement& SetPosition(uint32_t x, uint32_t y);
	UIElement& CenterX();
	UIElement& CenterY();
	UIElement& Center();

	void Clear();

	void SetID(const std::string& id) { m_ID = id; }
	void SetNode(const UINode& node) { m_Node = node; }

	bool Is(UIElementType type) const { return m_Type == type; }
	UIElementType GetType() const { return m_Type; }
	std::string GetID() const { return m_ID; }
	UINode GetNode() const { return m_Node; }

	UIElement* GetParent() const;
	UIElement* GetChild(const UINode& node) const;
	UIElement* GetChild(const std::string& id) const;
	VolcaniCore::List<UIElement*> GetChildren() const;

protected:
	const UIElementType m_Type;
	std::string m_ID;

	UIPage* m_Root;
	UINode m_Node;
	UINode m_Parent;

	UIState m_State;

	friend class UIPage;
};

}