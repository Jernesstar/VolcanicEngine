#pragma once

#include <string>
#include <vector>

namespace TheMazeIsLava {

class UIElement {
public:
	UIElement(Ref<UIElement> parent = nullptr);
	~UIElement();

	virtual Add(Ref<UIElement> element) = 0;
	void Render() {
		Draw();
		for(auto& node : m_Children) node->Render();
	}

protected:
	virtual void Draw() = 0;

private:
	Ref<UIElement> m_Parent;
	std::vector<Ref<UIElement>> m_Children;
};

class UIWindow : public UIElement {

};

}