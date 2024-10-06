#include "Asset.h"

namespace TheMazeIsLava {

void Asset::Init() {
	std::string assetPath = "Sandbox/TheMazeIsLava/assets/";

	auto tex = Texture::Create(assetPath + "images/stone.png");
	auto tex2 = Texture::Create(assetPath + "images/lava.png");
	Wall = Mesh::Create(MeshPrimitive::Cube, Material{ .Diffuse = tex });
	Lava = Mesh::Create(MeshPrimitive::Cube, Material{ .Diffuse = tex2 });

	// TODO(Change): Models in MeshComponent
	Player = Mesh::Create(assetPath + "models/player/Knight_Golden_Male.obj");
	Torch  = Mesh::Create(assetPath + "models/torch/torch.obj");
	Stairs = Mesh::Create(assetPath + "models/stairs/stairs.obj");
}

}