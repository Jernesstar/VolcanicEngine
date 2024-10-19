#pragma once

#include "Vec.h"

namespace Ash {

class Object {
	virtual ~Object() = default;
};

class Sphere : public Object {

};

class Box : public Object {

};

}