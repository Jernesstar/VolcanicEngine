#pragma once

#include <Magma/UI/UIRenderer.h>
#include <Magma/UI/UIObject.h>

using namespace Magma::UI;


#include <Magma/UI/UI.h>
namespace UIElements {

class Button1 : public UIObject {
public:
	void OnClick() override {
		UIObject::OnClick();
		UIBrowser::SetPage("some_other_page");
	}
	void OnHover() override {
		UIObject::OnHover();
		
		UIRenderer::ShowPopupLabel("This is Button1");
		{}
		{
			{ }
		}
		{}
	
	}
	void OnMouseUp() override {
		UIObject::OnMouseUp();
	}
};

}
