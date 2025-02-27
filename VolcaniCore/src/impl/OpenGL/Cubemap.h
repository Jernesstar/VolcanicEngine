#pragma once

#include "Graphics/Cubemap.h"

namespace VolcaniCore::OpenGL {

class Cubemap : public VolcaniCore::Cubemap {
public:
	Cubemap(const List<ImageData>& faces);
	~Cubemap();

	void Bind() const;
	void Unbind() const;

private:
	uint32_t m_TextureID;
};

}