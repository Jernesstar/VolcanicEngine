#include "UISerializer.h"

#include "UI.h"

namespace Magma::UI {

template<>
void UISerializer::Serialize(Window* ui, JSONSerializer& serializer) {
	serializer
		.WriteKey("BorderWidth").Write(ui->BorderWidth)
		.WriteKey("BorderHeight").Write(ui->BorderHeight)
		.WriteKey("BorderColor").Write(ui->BorderColor)
		.WriteKey("Open").Write(ui->Open);
}

template<>
void UISerializer::Serialize(Button* ui, JSONSerializer& serializer) {

}

template<>
void UISerializer::Serialize(Dropdown* ui, JSONSerializer& serializer) {

}

template<>
void UISerializer::Serialize(Text* ui, JSONSerializer& serializer) {
	serializer
		.WriteKey("Text").Write(ui->Content);
}

template<>
void UISerializer::Serialize(TextInput* ui, JSONSerializer& serializer) {
	serializer
		.WriteKey("Text").Write(ui->Text);
}

template<>
void UISerializer::Serialize(Image* ui, JSONSerializer& serializer) {
	serializer.WriteKey("Path").Write(ui->Content->GetPath());
}

template<>
void UISerializer::Serialize(UIElement* ui, JSONSerializer& serializer) {
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
			Serialize(ui->As<Empty>(), serializer);
			break;
		case UIElement::Type::Window:
			Serialize(ui->As<Window>(), serializer);
			break;
		case UIElement::Type::Button:
			Serialize(ui->As<Button>(), serializer);
			break;
		case UIElement::Type::Dropdown:
			Serialize(ui->As<Dropdown>(), serializer);
			break;
		case UIElement::Type::Text:
			Serialize(ui->As<Text>(), serializer);
			break;
		case UIElement::Type::TextInput:
			Serialize(ui->As<TextInput>(), serializer);
			break;
		case UIElement::Type::Image:
			Serialize(ui->As<Image>(), serializer);
			break;
	}

	serializer.WriteKey("Children").BeginSequence();
	for(auto child : ui->GetChildren())
		Serialize(child, serializer);
	serializer.EndSequence();

	serializer.EndMapping();
}

void UISerializer::Save(UIPage* page, const std::string& path) {
	JSONSerializer serializer;
	serializer.BeginMapping(); // File

	// TODO(Implement): Serialize theme

	serializer.WriteKey("Elements")
	.BeginSequence();

	for(auto* ui : page->GetFirstOrderElements())
		Serialize(ui, serializer);

	serializer.EndSequence();

	serializer.EndMapping(); // File
	serializer.Finalize(path);
}

template<>
Window* UISerializer::Deserialize(JSONParserNode& serializer) {
	
}

template<>
Button* UISerializer::Deserialize(JSONParserNode& serializer) {
	
}

template<>
Dropdown* UISerializer::Deserialize(JSONParserNode& serializer) {
	
}

template<>
Text* UISerializer::Deserialize(JSONParserNode& serializer) {
	
}

template<>
TextInput* UISerializer::Deserialize(JSONParserNode& serializer) {
	
}

template<>
Image* UISerializer::Deserialize(JSONParserNode& serializer) {
	
}

template<>
UIElement* UISerializer::Deserialize(JSONParserNode& serializer) {
	
}

UIPage* UISerializer::Load(const std::string& path) {
	// TODO(Implement):

}

}