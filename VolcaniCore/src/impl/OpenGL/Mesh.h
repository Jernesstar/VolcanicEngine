#pragma once

#include "Object/Mesh.h"

#include "OpenGL/Texture2D.h"
#include "OpenGL/VertexArray.h"

#include "Object/Texture.h"

namespace VolcaniCore::OpenGL {

class Mesh : public VolcaniCore::Mesh {
public:
	Mesh(const std::vector<Vertex>& vertices,
		 const std::vector<uint32_t>& indices,
		 const Material& material);

// private:
	Ref<OpenGL::VertexArray> m_VertexArray;
	Ref<OpenGL::VertexBuffer> m_VertexBuffer;
	Ref<OpenGL::IndexBuffer> m_IndexBuffer;

	friend class Renderer;
};

}