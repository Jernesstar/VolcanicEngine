#pragma once

#include "Core/Defines.h"

#include "Texture.h"

namespace VolcaniCore {

class Quad {
public:
	Quad(Ref<Texture> texture);

	static Ref<Quad> Create(Ref<Texture> texture);

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }
};

}