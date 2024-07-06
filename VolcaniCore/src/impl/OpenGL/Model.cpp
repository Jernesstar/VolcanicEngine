#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Core/Log.h"
#include "Core/Assert.h"

using namespace VolcaniCore;

namespace VolcaniCore::OpenGL {

// TODO: Move to Object/Model.cpp, as this is not renderer-specific
Model::Model(const std::string& path)
	: Path(path)
{
	VOLCANICORE_ASSERT(path != "");
	Load(path);
}

Model::~Model() {
	Unload();
}

void Model::Unload() {

}

void Model::Load(const std::string& path) {
	Assimp::Importer importer;
	uint32_t loadFlags = aiProcess_Triangulate
						| aiProcess_GenSmoothNormals
						| aiProcess_FlipUVs
						| aiProcess_JoinIdenticalVertices;
	const aiScene* scene = importer.ReadFile(path.c_str(), loadFlags);

	VOLCANICORE_ASSERT_ARGS(scene, "Error importing model(s) from %s: %s",
									path.c_str(), importer.GetErrorString());

	m_Meshes.resize(scene->mNumMeshes);

	for(uint32_t i = 0; i < m_Meshes.size(); i++)
		LoadMesh(path, scene->mMeshes[i],
				 scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]);

	delete scene;
}

void Model::LoadMesh(const std::string& path,
					 const aiMesh* mesh,
					 const aiMaterial* mat){

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	vertices.reserve(mesh->mNumVertices);
	indices.reserve(mesh->mNumFaces * 3);

	for(uint32_t i = 0; i < mesh->mNumVertices; i++) {
		const aiVector3D& pos	   = mesh->mVertices[i];
		const aiVector3D& normal   = mesh->mNormals[i];
		const aiVector3D& texCoord =
			mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] // Multiple textures supported
									  : aiVector3D(0.0f, 0.0f, 0.0f);

		Vertex v{
			.Position		   = glm::vec3(pos.x, pos.y, pos.z),
			.Normal			   = glm::vec3(normal.x, normal.y, normal.z),
			.TextureCoordinate = glm::vec2(texCoord.x, texCoord.y)
		};
		vertices.push_back(v);
	}

	for(uint32_t i = 0; i < mesh->mNumFaces; i++) {
		const aiFace& face = mesh->mFaces[i];
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	Material material = LoadMaterial(path, mat);

	m_Meshes.push_back(Mesh::Create(vertices, indices, material));
}

Material Model::LoadMaterial(const std::string& path,
						 const aiMaterial* mat)
{
	std::size_t slashIndex = path.find("textures");
	std::string dir;

	if(slashIndex == std::string::npos)
		dir = ".";
	else if(slashIndex == 0)
		dir = "/";
	else
		dir = path.substr(0, slashIndex);
	
	Material material {
		.Diffuse   = LoadTexture(dir, mat, aiTextureType_DIFFUSE),
		.Specular  = LoadTexture(dir, mat, aiTextureType_SPECULAR),
		.Roughness = LoadTexture(dir, mat, aiTextureType_DIFFUSE_ROUGHNESS)
	};

	return material;
}

Ref<Texture> Model::LoadTexture(const std::string& dir,
								const aiMaterial* material,
								aiTextureType type)
{
	if(material->GetTextureCount(type) == 0)
		return nullptr;

	aiString path;
	if(material->GetTexture(type, 0, &path) == AI_FAILURE)
		return nullptr;
	std::string p(path.data);

	std::string full_path = dir + "/" + p;
	return Texture::Create(full_path);
}

}