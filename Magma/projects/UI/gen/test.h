#pragma once

#include <Magma/UI/UIRenderer.h>
#include <Magma/UI/UIClickable.h>

using namespace Magma::UI;


#include <Magma/UI/UI.h>
namespace UIElements {

class Button1 : public UIClickable {
public:
	void OnClick() override {
		UIClickable::OnClick();
		UIBrowser::SetPage("some_other_page");
	}
	void OnHover() override {
		UIClickable::OnHover();
		
		UIRenderer::ShowPopupLabel("This is Button1");
		{}
		{
			{ }
		}
		{}
	
	}
	void OnMouseUp() override {
		UIClickable::OnMouseUp();
	}
};

}
