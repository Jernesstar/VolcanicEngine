#pragma once

#include <cmath>

#include <Magma/Core/BinaryWriter.h>
#include <Magma/Core/BinaryReader.h>

namespace Demo {

class Template : public Application {
public:
	Template();

	void OnUpdate(TimeStep ts) override;

	void Write();
	void Read();

private:
	Ref<Camera> camera;
	CameraController controller;
};

struct SimpleStruct {
	uint32_t x, y;
	float Height;
};

enum class SomeEnum { FirstValue, SecondValue, ThirdValue };

class ComplexClass {
public:
	SomeEnum Type;
	Buffer<uint32_t> Data;

public:
	ComplexClass(SomeEnum type)
		: Type(type)
	{
		Data = Buffer<uint32_t>(5);
		Data.Add(1);
		Data.Add(2);
		Data.Add(3);
		Data.Add(4);
		Data.Add(5);
	}
};

}

namespace Magma {

template<>
BinaryWriter& BinaryWriter::WriteObject(const Demo::ComplexClass& complex) {
	WriteRaw<uint32_t>((uint32_t)complex.Type);
	WriteRaw<uint64_t>(complex.Data.GetCount());
	WriteData(complex.Data.Get(), complex.Data.GetSize());

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(Demo::ComplexClass& complex) {
	ReadRaw<uint32_t>((uint32_t&)complex.Type);
	uint64_t size;
	ReadRaw<uint64_t>(size);
	complex.Data = Buffer<uint32_t>(size);
	ReadData(complex.Data.Get(), size * sizeof(uint32_t));
	for(uint64_t i = 0; i < size; i++)
		complex.Data.Add();

	return *this;
}

}

namespace Demo {

Template::Template() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::K)
				VOLCANICORE_LOG_INFO("Test Key K");
		});

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 0.0f, 0.0f, 3.0f });
	controller = CameraController{ camera };

	VOLCANICORE_LOG_INFO("Template Project Started");

	// Write();
	// Read();

	VOLCANICORE_LOG_INFO("Success");
}

void Template::OnUpdate(TimeStep ts) {
	// VOLCANICORE_LOG_INFO("Update");
}

void Template::Write() {
	BinaryWriter writer("Sandbox/assets/test.bin");

	writer.Write(std::string("VOLC_AP"));

	List<SimpleStruct> simpletons;
	for(uint64_t i = 0; i < 10; i++)
		simpletons.Emplace(i*i - 2*i + 5, i, std::pow(2, i));
	writer.Write(simpletons);

	ComplexClass complex(SomeEnum::SecondValue);
	writer.Write(complex);

	Map<uint64_t, bool> assets =
	{
		{ 15346425955542, true },
		{ 48725849247642, false },
		{ 52805442487852, false },
		{ 49134754284255, true },
		{ 42864029521301, false },
	};

	writer.Write(assets);
}

void Template::Read() {
	BinaryReader reader("Sandbox/assets/test.bin");

	std::string str;
	reader.Read(str);

	List<SimpleStruct> newSimpletons;
	reader.Read(newSimpletons);

	ComplexClass newComplex(SomeEnum::FirstValue);
	reader.Read(newComplex);

	Map<uint64_t, bool> assets;
	reader.Read(assets);

	VOLCANICORE_LOG_INFO(str.c_str());

	for(auto& simpleton : newSimpletons)
		VOLCANICORE_LOG_INFO("Simple: [%i, %i, %f]",
			simpleton.x, simpleton.y, simpleton.Height);

	VOLCANICORE_LOG_INFO("Complex: %i", newComplex.Type);
	for(uint64_t i = 0; i < newComplex.Data.GetCount(); i++)
		VOLCANICORE_LOG_INFO("Data #%i: %i",
			i, *(newComplex.Data.Get() + i));

	for(auto& [id, loaded] : assets)
		VOLCANICORE_LOG_INFO("{ ID:%li, Loaded:%i }", id, loaded);
}

}