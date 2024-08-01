#pragma once

static void createActors(Physics::World& world) {
	Shape box(SphapeType::Box);
	for(uint32_t x = 0; x < 10; x++) {
		RigidBody body(RigidBodyType::Dynamic, box,
			Transform{ .Translation = 0.5f * { x * 2.0f, 0.0f, 0.0f } });

		world.AddActor(body);
	}
}

namespace Demo {

class Raycast : public Application {
public:
	Raycast();
	~Raycast();

	void OnUpdate(TimeStep ts);

private:
	Ref<ShaderPipeline> shader;
	Ref<Mesh> cube;

	Ref<Camera> camera;
	Ref<CameraController> controller;

	PHysics::World world;
};

Raycast::Raycast() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	EventSystem::RegisterListener<MouseButtonPressedEvent>(
	[this](const MouseButtonPressedEvent& event) {
		auto width = Application::GetWindow()->GetWidth();
		auto height = Application::GetWindow()->GetHeight();
		glm::vec4 originNDC{
			(event.x/width - 0.5f) * 2.0f,
			(event.y/height - 0.5f) * 2.0f,
			-1.0f, 1.0f
		};
		glm::vec4 endNDC{
			(event.x/width - 0.5f) * 2.0f,
			(event.y/height - 0.5f) * 2.0f,
			1.0f, 1.0f
		};

		glm::mat4 invViewProj = glm::inverse(camera->GetViewProjection());
		glm::vec4 worldStart = invViewProj * originNDC;
		glm::vec4 worldEnd   = invViewProj * endNDC;
		worldStart /= worldStart.w;
		worldEnd   /= worldEnd.w;
		glm::vec3 rayDir = glm::normalize(glm::vec3(worldEnd - worldStart));
		float maxDist = 10000.0f;

		auto hitInfo = world.Raycast(worldStart, rayDir, maxDist);
		if(hitInfo.HasHit) {
			VOLCANICORE_LOG_INFO("{");
			VOLCANICORE_LOG_INFO("\tx: %d", hitInfo.GetActor().GetTransform().Translation.x);
			VOLCANICORE_LOG_INFO("\tDistance: %d", hitInfo.Distance);
			VOLCANICORE_LOG_INFO("}\n");
		}
		else
			VOLCANICORE_LOG_INFO("No hit");
	});

	cube = Mesh::Create(MeshPrimitive::Cube,
	Material{
		.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		// .Specular = Texture::Create("Sandbox/assets/images/wood_specular.png"),
	});
	shader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
	});
	shader->Bind();
	shader->SetTexture("u_Diffuse", cube->GetMaterial().Diffuse, 0);

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 0.0f, 0.0f, 3.0f });
	controller = CreateRef<CameraController>(camera);

	Physics::Init();
	createActors(world);

	// gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	// gScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
}

Raycast::~Raycast() {
	Physics::Close();
}

void Raycast::OnUpdate(TimeStep ts) {
	world.OnUpdate(ts);
	controller->OnUpdate(ts);

	shader->SetMat4("u_ViewProj", camera->GetViewProjection());

	Renderer::Clear();

	for(auto* body : world.GetActors()) {
		shader->SetMat4("u_Model", body->GetTransform());
		Renderer3D::DrawMesh(cube);
	}

	// const PxRenderBuffer& rb = gScene->getRenderBuffer();

	// for(PxU32 i=0; i < rb.getNbPoints(); i++)
	// {
	// 	const PxDebugPoint& point = rb.getPoints()[i];
		
	// }

	// for(PxU32 i=0; i < rb.getNbLines(); i++)
	// {
	// 	const PxDebugLine& line = rb.getLines()[i];
		
	// }
}

}