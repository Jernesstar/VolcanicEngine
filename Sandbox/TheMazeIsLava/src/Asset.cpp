#include "Asset.h"

namespace TheMazeIsLava {

void Asset::Init() {
	auto tex = Texture::Create("Sandbox/TheMazeIsLava/assets/images/stone.png");
	Wall = Mesh::Create(MeshPrimitive::Cube, Material{ .Diffuse = tex });

	Player = Model::Create("Sandbox/assets/models/sphere/wooden_sphere.obj");
	Torch = Model::Create("Sandbox/TheMazeIsLava/assets/models/torch/torch.obj");
	Stairs =
		Model::Create("Sandbox/TheMazeIsLava/assets/models/stairs/stairs.obj");
}

}