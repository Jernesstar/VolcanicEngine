#include "UILoader.h"

#include <fstream>

#include <angelscript/add_on/scriptarray/scriptarray.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/Algo.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/BinaryWriter.h>
#include <Magma/Core/BinaryReader.h>

#include <Lava/Core/App.h>

#include "Runtime.h"
#include "AssetManager.h"

using namespace Magma;
using namespace Magma::UI;
using namespace Lava;

static UIPage* s_CurrentPage;

namespace Magma {

template<>
BinaryReader& BinaryReader::ReadObject(UI::UINode& node) {
	uint32_t type;
	Read(type);
	node.first = (UI::UIElementType)type;
	Read(node.second);

	return *this;
}

static void ReadUI(BinaryReader* reader, UIElement* element) {
	std::string id;
	reader->Read(id);
	element->SetID(id);
	reader->Read(element->Width);
	reader->Read(element->Height);
	reader->Read(element->x);
	reader->Read(element->y);
	uint32_t xAlign, yAlign;
	reader->Read(xAlign);
	reader->Read(yAlign);
	element->xAlignment = (XAlignment)xAlign;
	element->yAlignment = (YAlignment)yAlign;
	reader->Read(element->UsePosition);
	reader->ReadData(&element->Color.r, sizeof(glm::vec4));
	reader->Read(element->Children);
	uint64_t assetID;
	reader->Read(assetID);
	element->ModuleID = assetID;
	reader->Read(element->Class);

	if(element->Class == "")
		return;

	auto* assetManager = AssetManager::Get();
	Asset asset = { element->ModuleID, AssetType::Script };
	assetManager->Load(asset);

	auto mod = assetManager->Get<ScriptModule>(asset);
	auto _class = mod->GetClass(element->Class);

	if(!_class)
		return;

	element->ScriptInstance = _class->Instantiate(element->GetID());

	auto handle = element->ScriptInstance->GetHandle();
	for(uint32_t i = 0; i < handle->GetPropertyCount(); i++) {
		int typeID;
		reader->Read(typeID);
		if(typeID == -1)
			continue;

		ScriptField field = element->ScriptInstance->GetProperty(i);
		std::string typeName;
		if(field.Type)
			typeName = field.Type->GetName();

		if(field.TypeID == asTYPEID_BOOL)
			reader->Read(*field.As<bool>());
		else if(field.TypeID == asTYPEID_INT8)
			reader->Read(*field.As<int8_t>());
		else if(field.TypeID == asTYPEID_INT16)
			reader->Read(*field.As<int16_t>());
		else if(field.TypeID == asTYPEID_INT32)
			reader->Read(*field.As<int32_t>());
		else if(field.TypeID == asTYPEID_INT64)
			reader->Read(*field.As<int64_t>());
		else if(field.TypeID == asTYPEID_UINT8)
			reader->Read(*field.As<uint8_t>());
		else if(field.TypeID == asTYPEID_UINT16)
			reader->Read(*field.As<uint16_t>());
		else if(field.TypeID == asTYPEID_UINT32)
			reader->Read(*field.As<uint32_t>());
		else if(field.TypeID == asTYPEID_UINT64)
			reader->Read(*field.As<uint64_t>());
		else if(field.TypeID == asTYPEID_FLOAT)
			reader->Read(*field.As<float>());
		else if(field.TypeID == asTYPEID_DOUBLE)
			reader->Read(*field.As<double>());
		else if(typeName == "Asset")
			reader->Read(*field.As<Asset>());
		else if(typeName == "Vec3")
			reader->Read(*field.As<glm::vec3>());
		else if(typeName == "string")
			reader->Read(*field.As<std::string>());
		else if(typeName == "array") {
			auto* array = field.As<CScriptArray>();
			auto subTypeID = array->GetArrayObjectType()->GetSubTypeId();
			auto* subType = ScriptEngine::Get()->GetTypeInfoById(subTypeID);
			uint64_t size = 0;
			if(subType)
				size = subType->GetSize();
			else
				size = ScriptEngine::Get()->GetSizeOfPrimitiveType(subTypeID);

			uint32_t count;
			reader->Read(count);
			Buffer<void> data(size, count);
			reader->ReadData(data.Get(), (uint64_t)size * count);

			array->Reserve(count);
			for(uint32_t i = 0; i < count; i++)
				array->InsertLast((char*)data.Get() + size * i);
		}
	}
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::Window& window) {
	ReadUI(this, &window);
	Read(window.BorderWidth);
	Read(window.BorderHeight);
	ReadData(&window.BorderColor.r, sizeof(glm::vec4));

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::Dropdown& dropdown) {
	ReadUI(this, &dropdown);
	Read(dropdown.CurrentItem);
	Read(dropdown.Options);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::Text& text) {
	ReadUI(this, &text);
	Read(text.Content);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::TextInput& textInput) {
	ReadUI(this, &textInput);
	Read(textInput.Text);
	Read(textInput.Hint);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::Image& image) {
	ReadUI(this, &image);
	uint64_t id;
	Read(id);
	Asset asset = { id, AssetType::Texture };
	auto* assetManager = AssetManager::Get();
	image.Content = assetManager->Get<Texture>(asset);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(UI::Button& button) {
	ReadUI(this, &button);
	bool isText;
	Read(isText);
	if(isText) {
		auto display = CreateRef<UI::Text>("", s_CurrentPage);
		Read(*display);
		button.Display = display;
	}
	else {
		auto display = CreateRef<UI::Image>("", s_CurrentPage);
		Read(*display);
		button.Display = display;
	}

	return *this;
}

}

namespace Lava {

void UILoader::Load(UIPage& page, const std::string& path) {
	namespace fs = std::filesystem;

	BinaryReader reader(path);

	s_CurrentPage = &page;
	reader.Read(page.Name);
	reader
		.Read(page.Windows, "", &page)
		.Read(page.Buttons, "", &page)
		.Read(page.Dropdowns, "", &page)
		.Read(page.Texts, "", &page)
		.Read(page.TextInputs, "", &page)
		.Read(page.Images, "", &page);

	reader.Read(page.LayerNodes);
	s_CurrentPage = nullptr;
}

}