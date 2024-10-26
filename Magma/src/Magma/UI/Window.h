#pragma once

#include "UIElement.h"

using namespace VolcaniCore;

namespace Magma::UI {

class Window : public UIElement {
public:
	struct Specification {
		std::string Name = "##Window";
		uint32_t Width = 100;
		uint32_t Height = 100;
		float x = 0;
		float y = 0;
		glm::vec4 Color = glm::vec4(1.0f);

		uint32_t BorderWidth = 0;
		uint32_t BorderHeight = 0;
		glm::vec4 BorderColor = glm::vec4(1.0f);
	};

public:
	static Ref<UI::Window> Create(const UI::Window::Specification& specs);

public:
	Window(std::string name, uint32_t width = 100, uint32_t height = 100,
			float x = 0.0f, float y = 0.0f,
			const glm::vec4& bgColor = glm::vec4(1.0f),
			uint32_t borderWidth = 0, uint32_t borderHeight = 0,
			const glm::vec4& borderColor = glm::vec4(1.0f));

	std::string GetName() const { return m_Name; }

	bool IsOpen() const { return m_Open; }
	void Open() { m_Open = true; }
	void Close() { m_Open = false; }

protected:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	std::string m_Name;
	uint32_t m_BorderWidth, m_BorderHeight;
	glm::vec4 m_BorderColor;
	bool m_Open = true;
};

}