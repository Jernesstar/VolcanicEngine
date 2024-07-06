#pragma once

#include "Object/Model.h"

#include <vector>
#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <assimp/scene.h>

#include "OpenGL/Texture2D.h"
#include "OpenGL/VertexArray.h"

#include "Object/Texture.h"

namespace VolcaniCore::OpenGL {

class Model : public VolcaniCore::Model {
public:
	const std::string Path;

public:
	Model(const std::string& path);
	~Model();

	void Bind() const {
		m_VertexArray->Bind();
	}

private:
	void Load(const std::string& path);
	void Unload();

	void LoadMesh(const aiMesh* mesh, const aiMaterial* material);

	Material LoadMaterial(const std::string& path
						  const aiMaterial* mat);

	Ref<Texture> LoadTexture(const std::string& dir,
							 const aiMaterial* mat,
							 aiTextureType type);
};

}