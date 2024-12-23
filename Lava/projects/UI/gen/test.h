#pragma once

#include <Magma/UI/UIRenderer.h>

#include <Lava/UIObject.h>

using namespace Magma::UI;
using namespace Lava;

#include <Magma/UI/UI.h>
#include <VolcaniCore/Core/Log.h>

namespace UIObjects {

class Button1 : public UIObject {
public:
	void OnUpdate(TimeStep ts) override {
		UIObject::OnUpdate(ts);

	}
	void OnClick() override {
		UIObject::OnClick();
		VOLCANICORE_LOG_INFO("Coming from Button1");
	}
	void OnHover() override {
		UIObject::OnHover();
		UIRenderer::ShowPopupLabel("This is the one and only Button1");
	}
	void OnMouseUp() override {
		UIObject::OnMouseUp();
	}
};

}
