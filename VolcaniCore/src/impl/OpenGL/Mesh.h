#pragma once

#include "Object/Mesh.h"

#include "OpenGL/Texture2D.h"
#include "OpenGL/VertexArray.h"

#include "Object/Texture.h"

namespace VolcaniCore::OpenGL {

class Mesh : public VolcaniCore::Mesh {
public:
	Mesh(const std::string& path);

	Mesh(const std::vector<Vertex>& vertices,
		 const std::vector<uint32_t>& indices, const Material& material = { });

	Ref<VertexArray> GetVertexArray() { return m_VertexArray; }

private:
	Ref<VertexArray> m_VertexArray;
};

}