#pragma once

#include "Object/Mesh.h"

#include "OpenGL/Texture2D.h"
#include "OpenGL/VertexArray.h"

#include "Object/Texture.h"

namespace VolcaniCore::OpenGL {

class Mesh : public VolcaniCore::Mesh {
public:
	Mesh(std::vector<Vertex> vertices,
		 std::vector<uint32_t> indices,
		 Material material);
	~Mesh();

	void Bind() const {
		m_VertexArray->Bind();
	}

	Ref<OpenGL::VertexArray> m_VertexArray;
	Ref<OpenGL::VertexBuffer> m_TransformBuffer;
private:
	Ref<OpenGL::VertexBuffer> m_VertexBuffer;
	Ref<OpenGL::IndexBuffer> m_IndexBuffer;

	friend class Renderer;
};

}