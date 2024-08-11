#pragma once

namespace Demo {

class Geometry : public Application {
public:
	Geometry();
	~Geometry();

	void OnUpdate(TimeStep ts);

private:
	Ref<ShaderPipeline> shader;
	Ref<Mesh> cube;

	Ref<Camera> camera;
	Ref<CameraController> controller;

	Physics::World world;
};

Geometry::Geometry() {
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	Physics::Init();

	// static const PxVec3 convexVerts[] = // Vertices for a square pyramid
	// {
	// 	PxVec3( 0,  1,  0),
	// 	PxVec3( 1,  0,  0),
	// 	PxVec3(-1,  0,  0),
	// 	PxVec3( 0,  0,  1),
	// 	PxVec3( 0,  0, -1)
	// };

	// Shape convexShape(convexVerts);

	Ref<Mesh> mesh = Mesh::Create(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});

	Shape meshShape(mesh);
}

Geometry::~Geometry() {
	Physics::Close();
}

void Geometry::OnUpdate(TimeStep ts) {
	world.OnUpdate(ts);
}

}