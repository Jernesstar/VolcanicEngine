#pragma once

#include "Graphics/Cubemap.h"

namespace VolcaniCore::OpenGL {


class Cubemap : public VolcaniCore::Cubemap {
public:
	Cubemap(const std::string& cubemap_folder);
	Cubemap(const std::vector<std::string>& faces);

	void Bind() const;

private:
	uint32_t m_TextureID;
};


}