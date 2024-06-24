#pragma once

#include "UI.h"

#include <string>
#include <vector>

#include <imgui/imgui.h>
#include <glm/vec4.hpp>

#include <Core/Defines.h>

using namespace VolcaniCore;

namespace Magma::UI {


class Image : public UIElement {
public:
	Image(Ref<Texture> image);
	Image(const std::string& imagePath);

	void Set(Ref<Texture> texture);
	// void Set(const std::string& imagePath); // AssetManager
	Ref<Texture> GetImage() { return image; }

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	Ref<Texture> m_Image;
};


}