#pragma once
#include <Magma/UI/UI.h>
#include <Magma/UI/UIClickable.h>
#include <VolcaniCore/Core/Defines.h>
using namespace VolcaniCore;
using namespace Magma::UI;
namespace UIElements {
class Button1 : public UIClickable {
public:
void OnClick() override {
UIClickable::OnClick();
UIBrowser::SetPage("some_other_page"); 
}
void OnHover() override {
UIClickable::OnHover();
nce

#include <UIRenderer.h>
#include <U
}
void OnMouseUp() override {
UIClickable::OnMouseUp();
}
};
}
