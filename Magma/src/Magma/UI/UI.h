#pragma once

#include "UIElement.h"
#include "UIBuilder.h"
#include "UISerializer.h"
#include "Empty.h"
#include "Window.h"
#include "Button.h"
#include "Dropdown.h"
#include "Image.h"
#include "Text.h"
#include "TextInput.h"

using namespace VolcaniCore;

namespace Magma::UI {

void Init();
void Close();
void Begin();
void End();

}