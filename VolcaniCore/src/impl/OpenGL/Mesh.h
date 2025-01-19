#pragma once

#include "Graphics/Mesh.h"

#include "OpenGL/Texture2D.h"
#include "OpenGL/VertexArray.h"

#include "Graphics/Texture.h"

namespace VolcaniCore::OpenGL {

class Mesh : public VolcaniCore::Mesh {
public:
	Mesh(const std::string& path);

	Mesh(const List<Vertex>& vertices,
		 const List<uint32_t>& indices, const Material& material = { });

	Ref<VertexArray> GetVertexArray() { return m_VertexArray; }

private:
	Ref<VertexArray> m_VertexArray;
};

}