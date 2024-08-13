#pragma once

#include <Object/Mesh.h>
#include <Object/Model.h>

namespace TheMazeIsLava {

struct Asset {
	static void Init();

	inline static Ref<Mesh> Wall;

	inline static Ref<Model> Player;
	inline static Ref<Model> Torch;
	inline static Ref<Model> Stairs;
};

}