#include "Asset.h"

namespace TheMazeIsLava {

void Asset::Init() {
	auto tex = Texture::Create("Sandbox/TheMazeIsLava/assets/images/stone.png");
	// TODO(codespace): Lava texture
	// auto tex2 = Texture::Create("Sandbox/TheMazeIsLava/assets/images/lava.png");
	Wall = Mesh::Create(MeshPrimitive::Cube, Material{ .Diffuse = tex });
	// Lava = Mesh::Create(MeshPrimitive::Cube, Material{ .Diffuse = tex2 });
	Lava = Mesh::Create(MeshPrimitive::Cube, Material{ .Diffuse = tex });

	Player = Model::Create("Sandbox/assets/models/sphere/wooden_sphere.obj")
			 ->GetMesh(0);
	Torch = Model::Create("Sandbox/TheMazeIsLava/assets/models/torch/torch.obj")
			->GetMesh(0);
	Stairs =
		Model::Create("Sandbox/TheMazeIsLava/assets/models/stairs/stairs.obj")
		->GetMesh(0);
}

}