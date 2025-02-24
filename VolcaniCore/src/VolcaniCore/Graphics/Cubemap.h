#pragma once

#include "Core/Defines.h"
#include "Core/List.h"
#include "Core/Template.h"

#include "Texture.h"

namespace VolcaniCore {

class Cubemap : public Derivable<Cubemap> {
public:
	static Ref<Cubemap> Create(const List<Buffer<uint8_t>>& faces);

public:
	Cubemap() = default;
	virtual ~Cubemap() = default;
};

}