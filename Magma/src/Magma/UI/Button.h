#pragma once

#include "UI.h"

#include "Image.h"
#include "Text.h"

using namespace VolcaniCore;

namespace Magma::UI {

class Button : public UIElement {
public:
	std::function<void(void)> OnPressed = [](){};
	std::function<void(void)> OnReleased = [](){};

public:
	Button(const glm::vec4& color, Ref<Text> uiText);
	Button(Ref<Image> uiImage);
	Button(const glm::vec4& color, const std::string& text,
			const glm::vec4& textColor = glm::vec4(1.0f));
	Button(const std::string& imagePath);

	Button* SetOnPressed(const std::function<void(void)>& callback) {
		OnPressed = callback;
		return this;
	}
	Button* SetOnReleased(const std::function<void(void)>& callback) {
		OnReleased = callback;
		return this;
	}

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	bool hasText = false;
	bool hasImage = false;
	Ref<UIElement> m_Display;
};

}