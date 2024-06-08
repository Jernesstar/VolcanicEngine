#pragma once

#include <string>
#include <vector>

#include <Core/Defines.h>

using namespace VolcaniCore;

namespace TheMazeIsLava {

class UIElement {
public:
	UIElement(Ref<UIElement> parent = nullptr)
		: m_Parent(parent) { }
	virtual ~UIElement() = default;

	void Render() {
		Draw();
		for(auto& node : m_Children) node->Render();
	}
	UIElement* Add(Ref<UIElement> element) {
		OnElementAdd(element);
		m_Children.push_back(element);
		return this; // To enable chaining
	};

protected:
	virtual void Draw() = 0;
	virtual void OnElementAdd(Ref<UIElement> element) = 0;

private:
	Ref<UIElement> m_Parent;
	std::vector<Ref<UIElement>> m_Children;
};

class UIWindow : public UIElement {

};

}