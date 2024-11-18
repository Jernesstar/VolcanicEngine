#pragma once

#include "Core/JSONSerializer.h"
#include "Core/JSONParser.h"

#include "UI/UIElement.h"

using namespace VolcaniCore;

namespace Magma::UI {

class UISerializer {
public:
	static void Save(Ref<UIElement> ui, const std::string& path);
	static Ref<UIElement> Load(const std::string& path);

private:
	template<typename TUIElement>
	static void Serialize(Ref<TUIElement> ui, JSONSerializer& serializer) { }

	template<typename TUIElement>
	static Ref<TUIElement> Deserialize(JSONParserNode& serializer) { }
};

}