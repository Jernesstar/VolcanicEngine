#pragma once

#include "Vec.h"

namespace Ash {

struct Ray {
	Vec3 Position;
	Vec3 Direction;

	Vec3 operator ()(float t) { return Position + Direction * t; }
};

}