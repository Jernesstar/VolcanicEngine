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
};

Geometry::Geometry() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	initPhysics();

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	static const PxVec3 convexVerts[] = // Vertices for a square pyramid
	{
		PxVec3( 0,  1,  0),
		PxVec3( 1,  0,  0),
		PxVec3(-1,  0,  0),
		PxVec3( 0,  0,  1),
		PxVec3( 0,  0, -1)
	};

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count  = 5;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data	 = convexVerts;
	convexDesc.flags		 = PxConvexFlag::eCOMPUTE_CONVEX;

	PxTolerancesScale scale;
	PxCookingParams params(scale);

	PxDefaultMemoryOutputStream buf;
	PxConvexMeshCookingResult::Enum result;
	bool status = PxCookConvexMesh(params, convexDesc, buf, &result);
	if(!status)
		VOLCANICORE_LOG_ERROR("Failed to create convex mesh");

	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* convexMesh = gPhysics->createConvexMesh(input);
	PxShape* convexShape =
		gPhysics->createShape(PxConvexMeshGeometry(convexMesh), *gMaterial, true);
												 // true, it will not be shared

	Ref<Mesh> mesh = Mesh::Create(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count  = mesh->GetVertices().size();
	meshDesc.points.stride = sizeof(Vertex);
	meshDesc.points.data   = &mesh->GetVertices()[0];

	meshDesc.triangles.count  = mesh->GetIndices().size();
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data   = &mesh->GetIndices()[0];

	// PxTolerancesScale scale;
	// PxCookingParams params(scale);

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum res;
	status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &res);
	// bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &res);
	if(!status)
		VOLCANICORE_LOG_ERROR("Failed to create triangle mesh");

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(),
										writeBuffer.getSize());
	PxTriangleMesh* triangleMesh = gPhysics->createTriangleMesh(readBuffer);
	PxShape* triangleShape = gPhysics->createShape(
		PxTriangleMeshGeometry(triangleMesh), *gMaterial, true);
}

Geometry::~Geometry() {
	cleanupPhysics();
}

void Geometry::OnUpdate(TimeStep ts) {
	stepPhysics(ts);
}

}