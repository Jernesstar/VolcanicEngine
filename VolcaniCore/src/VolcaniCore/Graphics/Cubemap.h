#pragma once

#include "Core/List.h"

#include "Texture.h"

namespace VolcaniCore {

class Cubemap : public Derivable<Cubemap> {
public:
	static Ref<Cubemap> Create(const List<ImageData>& faces);

public:
	Cubemap() = default;
	virtual ~Cubemap() = default;
};

}