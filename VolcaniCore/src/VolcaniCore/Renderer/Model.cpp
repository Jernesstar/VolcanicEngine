#include "Model.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Core/Log.h"
#include "Core/Assert.h"

namespace VolcaniCore {

Ref<Model> Model::Create(const std::string& path) {
	return CreateRef<Model>(path);
}

Model::Model(const std::string& path)
	: Path(path)
{
	Load(path);
}

// Model::~Model() {

// }

static Ref<Mesh> LoadMesh(const std::string& path,
						  const aiScene* scene, uint32_t meshIndex);

void Model::Load(const std::string& path) {
	VOLCANICORE_ASSERT(path != "");

	Assimp::Importer importer;
	uint32_t loadFlags = aiProcess_Triangulate
						| aiProcess_GenSmoothNormals
						| aiProcess_FlipUVs
						| aiProcess_JoinIdenticalVertices;
	const aiScene* scene = importer.ReadFile(path.c_str(), loadFlags);

	VOLCANICORE_ASSERT_ARGS(scene, "Error importing model(s) from %s: %s",
									path.c_str(), importer.GetErrorString());

	m_Meshes.reserve(scene->mNumMeshes);

	for(uint32_t i = 0; i < scene->mNumMeshes; i++)
		m_Meshes[i] = std::move(LoadMesh(path, scene, i));
	for(uint32_t i = 0; i < scene->mNumMeshes; i++)
		VOLCANICORE_LOG_INFO("%s", m_Meshes[i]->GetMaterial().Diffuse->GetPath().c_str());
}

// void Model::Unload() {

// }

static Ref<Texture> LoadTexture(const std::string& dir,
								const aiMaterial* mat, aiTextureType type);

Ref<Mesh> LoadMesh(const std::string& path,
					 const aiScene* scene,
					 uint32_t meshIndex)
{
	auto mesh = scene->mMeshes[meshIndex];
	auto mat = scene->mMaterials[mesh->mMaterialIndex];

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	vertices.reserve(mesh->mNumVertices);
	indices.reserve(mesh->mNumFaces * 3);

	for(uint32_t i = 0; i < mesh->mNumVertices; i++) {
		const aiVector3D& pos	   = mesh->mVertices[i];
		const aiVector3D& normal   = mesh->mNormals[i];
		const aiVector3D& texCoord =
			mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i]
										: aiVector3D(0.0f, 0.0f, 0.0f);

		Vertex v{
			.Position		   = glm::vec3(pos.x, pos.y, pos.z),
			.Normal			   = glm::vec3(normal.x, normal.y, normal.z),
			.TextureCoordinate = glm::vec2(texCoord.x, texCoord.y)
		};
		vertices[i] = v;
	}

	for(uint32_t i = 0; i < mesh->mNumFaces; i++) {
		const aiFace& face = mesh->mFaces[i];
		indices[i + 0] = face.mIndices[0];
		indices[i + 1] = face.mIndices[1];
		indices[i + 2] = face.mIndices[2];
	}

	std::size_t slashIndex = path.find("textures");
	std::string dir;

	if(slashIndex == std::string::npos)
		dir = ".";
	else if(slashIndex == 0)
		dir = "/";
	else
		dir = path.substr(0, slashIndex);

	Ref<Mesh> newMesh = Mesh::Create(vertices, indices,
		Material{
			.Diffuse   = LoadTexture(dir, mat, aiTextureType_DIFFUSE),
			.Specular  = LoadTexture(dir, mat, aiTextureType_SPECULAR),
			.Roughness = LoadTexture(dir, mat, aiTextureType_DIFFUSE_ROUGHNESS)
			// .Emissive = LoadTexture(dir, mat, aiTextureType_EMISSIVE)
		}
	);
	return std::move(newMesh);
}

Ref<Texture> LoadTexture(const std::string& dir,
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