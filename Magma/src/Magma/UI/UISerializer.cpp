#pragma once

#include "UISerializer.h"

namespace Magma::UI {

void UISerializer::Save(Ref<UIElement> ui, const std::string& path) {
	JSONSerializer serializer;
	serializer.BeginMapping(); // File

	Serialize(ui, serializer);

	serializer.EndMapping(); // File
	serializer.Finalize(path);
}

Ref<UI::UIElement> UISerializer::Load(const std::string& path) {

}

void UISerializer::Serialize(Ref<UIElement> ui, JSONSerializer& serializer) {
	serializer
	.BeginMapping()
		.WriteKey("Type");

	switch(ui->GetType()) {
		case UIElement::Type::Empty:
			serializer.Write("Empty");
			break;
		case UIElement::Type::Window:
			serializer.Write("Window");
			break;
		case UIElement::Type::Button:
			serializer.Write("Button");
			break;
		case UIElement::Type::Dropdown:
			serializer.Write("Dropdown");
			break;
		case UIElement::Type::Text:
			serializer.Write("Text");
			break;
		case UIElement::Type::TextInput:
			serializer.Write("TextInput");
			break;
		case UIElement::Type::Image:
			serializer.Write("Image");
			break;
	}

	serializer
		.WriteKey("Width").Write(ui->GetWidth())
		.WriteKey("Height").Write(ui->GetHeight())
		.WriteKey("x").Write(ui->GetPositionX())
		.WriteKey("y").Write(ui->GetPositionY())
		.WriteKey("Color").Write(ui->GetColor());

	// ui->OnSerialize(serializer);

	serializer.WriteKey("Children").BeginSequence();
	for(auto child : ui->GetChildren())
		Serialize(child, serializer);
	serializer.EndSequence();

	serializer.EndMapping();
}

Ref<UIElement> UISerializer::Deserialize(JSONParserNode& serializer) {
	
}


}