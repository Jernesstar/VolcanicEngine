#pragma once

#include <Object/Mesh.h>
#include <Object/Model.h>

using namespace VolcaniCore;

namespace TheMazeIsLava {

class Asset {
public:
	static void Init();

public:
	inline static Ref<Mesh> Wall;
	inline static Ref<Mesh> Lava;

	inline static Ref<Mesh> Player;
	inline static Ref<Mesh> Torch;
	inline static Ref<Mesh> Stairs;
};

}