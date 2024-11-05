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
	static void Serialize(Ref<UIElement> element, JSONSerializer& serializer);
	static Ref<UIElement> Deserialize(JSONParserNode& serializer);
};

}