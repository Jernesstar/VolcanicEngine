#include "Asset.h"

namespace TheMazeIsLava {

void Asset::Init() {
	auto tex = Texture::Create("Sandbox/TheMazeIsLava/assets/images/stone.png");
	auto tex2 = Texture::Create("Sandbox/TheMazeIsLava/assets/images/lava.png");
	Wall = Mesh::Create(MeshPrimitive::Cube, Material{ .Diffuse = tex });
	Lava = Mesh::Create(MeshPrimitive::Cube, Material{ .Diffuse = tex2 });

	Player = Mesh::Create("Sandbox/assets/models/sphere/wooden_sphere.obj");
	Torch = Mesh::Create("Sandbox/TheMazeIsLava/assets/models/torch/torch.obj");
	Stairs =
		Mesh::Create("Sandbox/TheMazeIsLava/assets/models/stairs/stairs.obj");
}

}