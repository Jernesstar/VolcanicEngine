#include "Model.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Core/Log.h"
#include "Core/Assert.h"
#include "Core/FileUtils.h"

namespace VolcaniCore {

Ref<Model> Model::Create(const std::string& path) {
	return CreateRef<Model>(path);
}

Model::Model(const std::string& path)
	: Path(path)
{
	Load(path);
}

void Model::AddMesh(Ref<Mesh> mesh) {
	m_Meshes.Add(mesh);
}

static Ref<Mesh> LoadMesh(const std::string& path,
						  const aiScene* scene, uint32_t meshIndex);

void Model::Load(const std::string& path) {
	if(path == "")
		return;

	Assimp::Importer importer;
	uint32_t loadFlags = aiProcess_Triangulate
						| aiProcess_GenSmoothNormals
						| aiProcess_FlipUVs
						| aiProcess_JoinIdenticalVertices;
	const aiScene* scene = importer.ReadFile(path.c_str(), loadFlags);

	VOLCANICORE_ASSERT_ARGS(scene, "Error importing model(s) from %s: %s",
									path.c_str(), importer.GetErrorString());

	m_Meshes.Reallocate(scene->mNumMeshes);
	for(uint32_t i = 0; i < scene->mNumMeshes; i++)
		m_Meshes.Add(LoadMesh(path, scene, i));
}

static Ref<Texture> LoadTexture(const std::string& dir,
								const aiMaterial* mat, aiTextureType type);

Ref<Mesh> LoadMesh(const std::string& path,
				   const aiScene* scene, uint32_t meshIndex)
{


	auto mesh = scene->mMeshes[meshIndex];
	auto mat = scene->mMaterials[mesh->mMaterialIndex];

	List<Vertex> vertices;
	List<uint32_t> indices;

	vertices.Reallocate(mesh->mNumVertices);
	indices.Reallocate(mesh->mNumFaces * 3);

	for(uint32_t i = 0; i < mesh->mNumVertices; i++) {
		const aiVector3D& pos	   = mesh->mVertices[i];
		const aiVector3D& normal   = mesh->mNormals[i];
		const aiVector3D& texCoord = mesh->HasTextureCoords(0)
									? mesh->mTextureCoords[0][i]
									: aiVector3D(0.0f, 0.0f, 0.0f);

		Vertex v{
			.Position		= glm::vec3(pos.x, pos.y, pos.z),
			.Normal			= glm::vec3(normal.x, normal.y, normal.z),
			.TexCoord		= glm::vec2(texCoord.x, texCoord.y)
		};
		vertices.Add(v);
	}

	for(uint32_t i = 0; i < mesh->mNumFaces; i++) {
		const aiFace& face = mesh->mFaces[i];
		indices.Add(face.mIndices[0]);
		indices.Add(face.mIndices[1]);
		indices.Add(face.mIndices[2]);
	}

	glm::vec4 diffuse = glm::vec4(0.0f);
	glm::vec4 specular = glm::vec4(0.0f);
	glm::vec4 emissive = glm::vec4(0.0f);

	auto material = scene->mMaterials[mesh->mMaterialIndex];
	aiColor4D color;
	if(material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
		diffuse = glm::vec4(color.r, color.g, color.b, color.a);
	if(material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
		specular = glm::vec4(color.r, color.g, color.b, color.a);
	if(material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
		emissive = glm::vec4(color.r, color.g, color.b, color.a);

	namespace fs = std::filesystem;

	std::string dir = (fs::path(path).parent_path() / "textures").string();

	Ref<Mesh> newMesh = Mesh::Create(vertices, indices,
		Material{
			.Diffuse  = LoadTexture(dir, mat, aiTextureType_DIFFUSE),
			.Specular = LoadTexture(dir, mat, aiTextureType_SPECULAR),
			.Emissive = LoadTexture(dir, mat, aiTextureType_EMISSIVE),
			// .Roughness = LoadTexture(dir, mat, aiTextureType_DIFFUSE_ROUGHNESS)

			.DiffuseColor  = diffuse,
			.SpecularColor = specular,
			.EmissiveColor = emissive
		});

	return newMesh;
}

Ref<Texture> LoadTexture(const std::string& dir,
						 const aiMaterial* material, aiTextureType type)
{
	if(material->GetTextureCount(type) == 0)
		return nullptr;

	aiString path;
	if(material->GetTexture(type, 0, &path) == AI_FAILURE)
		return nullptr;

	return Texture::Create(std::string(path.data));
}

}