#pragma once

#include "Core/Defines.h"
#include "Core/List.h"
#include "Core/Template.h"

#include "Texture.h"

namespace VolcaniCore {

struct ImageData {
	uint32_t Width, Height;
	Buffer<uint8_t> Data;
};

class Cubemap : public Derivable<Cubemap> {
public:
	static Ref<Cubemap> Create(const List<ImageData>& faces);

public:
	Cubemap() = default;
	virtual ~Cubemap() = default;
};

}