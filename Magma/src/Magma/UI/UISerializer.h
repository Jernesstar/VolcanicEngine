#pragma once

#include "Core/JSONSerializer.h"
#include "Core/JSONParser.h"

#include "UI/UIElement.h"

using namespace VolcaniCore;

namespace Magma::UI {

class UISerializer {
public:
	static void Save(UIPage* page, const std::string& path);
	static UIPage* Load(const std::string& path);

private:
	template<typename TUIElement>
	static void Serialize(TUIElement* ui, JSONSerializer& serializer) { }

	template<typename TUIElement>
	static TUIElement* Deserialize(JSONParserNode& serializer) { }
};

}