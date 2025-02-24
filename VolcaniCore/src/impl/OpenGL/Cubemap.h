#pragma once

#include "Graphics/Cubemap.h"

namespace VolcaniCore::OpenGL {


class Cubemap : public VolcaniCore::Cubemap {
public:
	Cubemap(const List<Buffer<uint8_t>>& faces);

	void Bind() const;

private:
	uint32_t m_TextureID;
};


}