#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Core/Log.h"
#include "Core/Assert.h"

#include "BufferLayout.h"

using namespace VolcaniCore::OpenGL;

namespace VolcaniCore {

Model::Model(const std::string& path)
	: Path(path)
{
	VOLCANICORE_ASSERT(path != "");

	LoadMesh(path);
}

Model::~Model() { Clear(); }

void Model::Clear() {
	m_Meshes.clear();
	m_Materials.clear();

	m_Positions.clear();
	m_Normals.clear();
	m_TextureCoords.clear();
	m_Indices.clear();

	m_Buffers[BufferIndex::Position].reset();
	m_Buffers[BufferIndex::TextureCoordinate].reset();
	m_Buffers[BufferIndex::Normal].reset();
	m_IndexBuffer.reset();
	m_VertexArray.reset();
}

void Model::LoadMesh(const std::string& path) {
	Assimp::Importer imp;
	uint32_t load_flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;
	const aiScene* scene = imp.ReadFile(path.c_str(), load_flags);

	VOLCANICORE_ASSERT_ARGS(scene, "Error importing model(s) from %s: %s", path.c_str(), imp.GetErrorString());

	m_Path = path;
	m_Meshes.resize(scene->mNumMeshes);
	m_Materials.resize(scene->mNumMaterials);

	uint32_t vertex_count = 0;
	uint32_t index_count = 0;

	for(uint32_t i = 0; i < m_Meshes.size(); i++) {
		aiMesh* in_mesh = scene->mMeshes[i];
		Mesh& out_mesh = m_Meshes[i];
		out_mesh.MaterialIndex = in_mesh->mMaterialIndex;
		out_mesh.IndexCount = in_mesh->mNumFaces * 3;
		out_mesh.BaseVertex = vertex_count;
		out_mesh.BaseIndex = index_count;

		vertex_count += in_mesh->mNumVertices;
		index_count += out_mesh.IndexCount;
	}

	m_Positions.reserve(vertex_count);
	m_TextureCoords.reserve(vertex_count);
	m_Normals.reserve(vertex_count);
	m_Indices.reserve(index_count);

	for(uint32_t i = 0; i < m_Meshes.size(); i++)
		LoadSubMesh(scene->mMeshes[i]);

	for(uint32_t i = 0; i < scene->mNumMaterials; i++)
		LoadMaterial(scene->mMaterials[i], path, i);

	BufferLayout l1({ { "Position",          BufferDataType::Vec3, false } }, false);
	BufferLayout l2({ { "TextureCoordinate", BufferDataType::Vec2, false } }, false);
	BufferLayout l3({ { "Normal",            BufferDataType::Vec3, false } }, false);

	m_Buffers[BufferIndex::Position]          = CreatePtr<VertexBuffer>(m_Positions.size(), l1, &m_Positions[0]);
	m_Buffers[BufferIndex::TextureCoordinate] = CreatePtr<VertexBuffer>(m_TextureCoords.size(), l2, &m_TextureCoords[0]);
	m_Buffers[BufferIndex::Normal]            = CreatePtr<VertexBuffer>(m_Normals.size(), l3, &m_Normals[0]);

	m_IndexBuffer = CreatePtr<IndexBuffer>(&m_Indices[0], m_Indices.size());

	m_VertexArray = CreatePtr<VertexArray>();
	m_VertexArray->SetIndexBuffer(m_IndexBuffer.get());
	m_VertexArray->AddVertexBuffer(m_Buffers[BufferIndex::Position].get());
	m_VertexArray->AddVertexBuffer(m_Buffers[BufferIndex::TextureCoordinate].get());
	m_VertexArray->AddVertexBuffer(m_Buffers[BufferIndex::Normal].get());

	delete scene;
}

void Model::LoadSubMesh(const aiMesh* mesh) {
	for(uint32_t i = 0; i < mesh->mNumVertices; i++) {
		const aiVector3D& position = mesh->mVertices[i];
		const aiVector3D& normal = mesh->mNormals[i];
		const aiVector3D& texture_coord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f);

		m_Positions.push_back(glm::vec3(position.x, position.y, position.z));
		m_Normals.push_back(glm::vec3(normal.x, normal.y, normal.z));
		m_TextureCoords.push_back(glm::vec2(texture_coord.x, texture_coord.y));
	}

	for(uint32_t i = 0; i < mesh->mNumFaces; i++) {
		const aiFace& face = mesh->mFaces[i];
		m_Indices.push_back(face.mIndices[0]);
		m_Indices.push_back(face.mIndices[1]);
		m_Indices.push_back(face.mIndices[2]);
	}
}

void Model::LoadMaterial(const aiMaterial* material, const std::string& path, uint32_t index) {
	std::size_t slash_index = path.find("textures");
	std::string dir;

	if(slash_index == std::string::npos)
		dir = ".";
	else if(slash_index == 0)
		dir = "/";
	else
		dir = path.substr(0, slash_index);

	m_Materials[index].Diffuse = LoadTexture(material, dir, aiTextureType_DIFFUSE);
	m_Materials[index].Specular = LoadTexture(material, dir, aiTextureType_SPECULAR);
	m_Materials[index].Roughness = LoadTexture(material, dir, aiTextureType_DIFFUSE_ROUGHNESS);
}

Ref<Texture> Model::LoadTexture(const aiMaterial* material, const std::string& dir, aiTextureType type) {
	if(material->GetTextureCount(type) == 0)
		return nullptr;

	aiString path;
	if(material->GetTexture(type, 0, &path) == AI_FAILURE)
		return nullptr;

	std::string full_path = dir + "/" + p;
	return Texture::Create(full_path);
}

}