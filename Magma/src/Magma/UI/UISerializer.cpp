#include "UISerializer.h"

#include "UI.h"

namespace Magma::UI {

template<>
void UISerializer::Serialize(Ref<Window> ui, JSONSerializer& serializer) {
	serializer
		.WriteKey("BorderWidth").Write(ui->BorderWidth)
		.WriteKey("BorderHeight").Write(ui->BorderHeight)
		.WriteKey("BorderColor").Write(ui->BorderColor)
		.WriteKey("Open").Write(ui->IsOpen());
}

template<>
void UISerializer::Serialize(Ref<Button> ui, JSONSerializer& serializer) {

}

template<>
void UISerializer::Serialize(Ref<Dropdown> ui, JSONSerializer& serializer) {

}

template<>
void UISerializer::Serialize(Ref<Text> ui, JSONSerializer& serializer) {
	serializer
		.WriteKey("Text").Write(ui->Content);
}

template<>
void UISerializer::Serialize(Ref<TextInput> ui, JSONSerializer& serializer) {
	serializer
		.WriteKey("Text").Write(ui->Text);
}

template<>
void UISerializer::Serialize(Ref<Image> ui, JSONSerializer& serializer) {
	serializer.WriteKey("Path").Write(ui->Content->GetPath());
}

template<>
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
		.WriteKey("Width").Write(ui->Width)
		.WriteKey("Height").Write(ui->Height)
		.WriteKey("x").Write(ui->x)
		.WriteKey("y").Write(ui->y)
		.WriteKey("Color").Write(ui->Color);

	switch(ui->GetType()) {
		case UIElement::Type::Empty:
			Serialize(std::static_pointer_cast<Empty>(ui), serializer);
			break;
		case UIElement::Type::Window:
			Serialize(std::static_pointer_cast<Window>(ui), serializer);
			break;
		case UIElement::Type::Button:
			Serialize(std::static_pointer_cast<Button>(ui), serializer);
			break;
		case UIElement::Type::Dropdown:
			Serialize(std::static_pointer_cast<Dropdown>(ui), serializer);
			break;
		case UIElement::Type::Text:
			Serialize(std::static_pointer_cast<Text>(ui), serializer);
			break;
		case UIElement::Type::TextInput:
			Serialize(std::static_pointer_cast<TextInput>(ui), serializer);
			break;
		case UIElement::Type::Image:
			Serialize(std::static_pointer_cast<Image>(ui), serializer);
			break;
	}

	serializer.WriteKey("Children").BeginSequence();
	for(auto child : ui->GetChildren())
		Serialize(child, serializer);
	serializer.EndSequence();

	serializer.EndMapping();
}

void UISerializer::Save(Ref<UIElement> ui, const std::string& path) {
	JSONSerializer serializer;
	serializer.BeginMapping(); // File

	Serialize(ui, serializer);

	serializer.EndMapping(); // File
	serializer.Finalize(path);
}

template<>
Ref<Window> UISerializer::Deserialize(JSONParserNode& serializer) {
	
}

template<>
Ref<Button> UISerializer::Deserialize(JSONParserNode& serializer) {
	
}

template<>
Ref<Dropdown> UISerializer::Deserialize(JSONParserNode& serializer) {
	
}

template<>
Ref<Text> UISerializer::Deserialize(JSONParserNode& serializer) {
	
}

template<>
Ref<TextInput> UISerializer::Deserialize(JSONParserNode& serializer) {
	
}

template<>
Ref<Image> UISerializer::Deserialize(JSONParserNode& serializer) {
	
}

template<>
Ref<UIElement> UISerializer::Deserialize(JSONParserNode& serializer) {
	
}

Ref<UI::UIElement> UISerializer::Load(const std::string& path) {
	// TODO(Implement):
}

}