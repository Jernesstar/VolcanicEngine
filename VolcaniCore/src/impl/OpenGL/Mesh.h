#pragma once

#include "Object/Mesh.h"

#include <vector>
#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <assimp/scene.h>

#include "OpenGL/Texture2D.h"
#include "OpenGL/VertexArray.h"

#include "Object/Texture.h"

namespace VolcaniCore::OpenGL {

class Mesh : public VolcaniCore::Mesh {
public:
	struct Material {
		Ref<Texture> Diffuse;
		Ref<Texture> Specular;
		Ref<Texture> Roughness;

		void Bind() const {
			if(Diffuse) Diffuse->As<OpenGL::Texture2D>()->Bind();
			if(Specular) Specular->As<OpenGL::Texture2D>()->Bind();
			if(Roughness) Roughness->As<OpenGL::Texture2D>()->Bind();
		}
	};

public:
	Mesh();
	~Mesh();

	void Bind() const {
		m_VertexArray->Bind();
	}

	const Material& GetMaterial(uint32_t index) const {
		return m_Materials[index];
	}

private:
	enum BufferIndex {
		Position,
		TextureCoordinate,
		Normal
	};

	std::vector<Material> m_Materials;

	std::vector<glm::vec3> m_Positions;
	std::vector<glm::vec3> m_Normals;
	std::vector<glm::vec2> m_TextureCoords;
	std::vector<uint32_t> m_Indices;

	Ptr<OpenGL::VertexBuffer> m_Buffers[3];
	Ptr<OpenGL::IndexBuffer> m_IndexBuffer;
	Ptr<OpenGL::VertexArray> m_VertexArray;
};

}