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
		case UIElementType::Empty:
			serializer.Write("Empty");
			break;
		case UIElementType::Window:
			serializer.Write("Window");
			break;
		case UIElementType::Button:
			serializer.Write("Button");
			break;
		case UIElementType::Dropdown:
			serializer.Write("Dropdown");
			break;
		case UIElementType::Text:
			serializer.Write("Text");
			break;
		case UIElementType::TextInput:
			serializer.Write("TextInput");
			break;
		case UIElementType::Image:
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
		case UIElementType::Empty:
			Serialize(ui->As<Empty>(), serializer);
			break;
		case UIElementType::Window:
			Serialize(ui->As<Window>(), serializer);
			break;
		case UIElementType::Button:
			Serialize(ui->As<Button>(), serializer);
			break;
		case UIElementType::Dropdown:
			Serialize(ui->As<Dropdown>(), serializer);
			break;
		case UIElementType::Text:
			Serialize(ui->As<Text>(), serializer);
			break;
		case UIElementType::TextInput:
			Serialize(ui->As<TextInput>(), serializer);
			break;
		case UIElementType::Image:
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

}