#pragma once

#include "Renderer/Model.h"

#include <vector>
#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <assimp/scene.h>

#include "OpenGL/Texture2D.h"
#include "OpenGL/VertexArray.h"

#include "Renderer/Texture.h"

namespace VolcaniCore::OpenGL {

class Model : public VolcaniCore::Model {
public:
	struct Mesh {
		uint32_t BaseVertex = 0;
		uint32_t BaseIndex = 0;
		uint32_t IndexCount = 0;
		uint32_t MaterialIndex = 0;
	};

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

	const std::string Path;

public:
	Model(const std::string& folder_path);
	~Model();

	void Bind() const { m_VertexArray->Bind(); }

	uint32_t GetMeshCount() 		const { return m_Meshes.size(); }
	const Mesh& GetMesh(uint32_t i) const { return m_Meshes[i]; }
	const Material& GetMaterial(uint32_t index) const { return m_Materials[index]; }

private:
	enum BufferIndex {
		Position,
		TextureCoordinate,
		Normal
	};

	std::vector<Mesh> m_Meshes;
	std::vector<Material> m_Materials;

	std::vector<glm::vec3> m_Positions;
	std::vector<glm::vec3> m_Normals;
	std::vector<glm::vec2> m_TextureCoords;
	std::vector<uint32_t> m_Indices;

	Ptr<OpenGL::VertexBuffer> m_Buffers[3];
	Ptr<OpenGL::IndexBuffer> m_IndexBuffer;
	Ptr<OpenGL::VertexArray> m_VertexArray;

	void Clear();
	void LoadMesh(const std::string& path);
	void LoadSubMesh(const aiMesh* mesh);
	void LoadMaterial(const aiMaterial* material, const std::string& path, uint32_t index);
	Ref<Texture> LoadTexture(const aiMaterial* material, const std::string& dir, aiTextureType type);
};

}