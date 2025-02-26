#include "AssetImporter.h"

#include <stb_image/stb_image.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <soloud.h>
#include <soloud_wav.h>

#include "Core/Log.h"
#include "Core/Assert.h"
#include "Core/FileUtils.h"

using namespace VolcaniCore;

namespace fs = std::filesystem;

namespace Magma {

static uint8_t* ReadImage(const std::string& path,
	int& width, int& height, int& bitsPerPixel, int desiredChannels, bool flip)
{
	stbi_set_flip_vertically_on_load((int)flip);
	uint8_t* pixelData =
		stbi_load(path.c_str(), &width, &height, &bitsPerPixel, desiredChannels);

	VOLCANICORE_ASSERT_ARGS(pixelData, "Could not load image from path '%s'",
										path.c_str());

	return pixelData;
}

Ref<Texture> AssetImporter::GetTexture(const std::string& path) {
	int width, height, bpp;
	auto data = ReadImage(path, width, height, bpp, 4, 0);
	auto texture = Texture::Create(width, height);
	texture->SetData(data);
	return texture;
}

static List<std::string> GetImagePaths(const std::string& folder) {
	List<std::string> paths;
	for(auto path : FileUtils::GetFiles(folder, { ".png", ".jpg", ".jpeg" })) {
		paths.Add(path);
		if(paths.Count() == 6) break;
	}

	if(paths.Count() < 6)
		VOLCANICORE_LOG_WARNING(
			"Cubemap folder %s does not have at least 6 images",
			folder.c_str());

	return paths;
}

Ref<Cubemap> AssetImporter::GetCubemap(const std::string& path) {
	auto facePaths = GetImagePaths(path);
	Map<std::string, int> map =
	{
		{ "right", 0 }, { "left",	1 },
		{ "top",   2 }, { "bottom", 3 },
		{ "front", 4 }, { "back",	5 }
	};
	List<Buffer<uint8_t>> output(6);

	for(auto& facePath : facePaths) {
		int width, height, bpp;
		uint8_t* data =
			ReadImage(facePath, width, height, bpp, 4, false);
		// output.Emplace(ImageData(width, height, bpp, data));
	}
}

static Ref<Texture> LoadTexture(const std::string& dir,
								const aiMaterial* mat, aiTextureType type);

static SubMesh LoadMesh(const std::string& path,
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

		Vertex v
		{
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

	auto subMesh = SubMesh(vertices, indices,
		Material{
			.Diffuse  = LoadTexture(dir, mat, aiTextureType_DIFFUSE),
			.Specular = LoadTexture(dir, mat, aiTextureType_SPECULAR),
			.Emissive = LoadTexture(dir, mat, aiTextureType_EMISSIVE),
			// .Roughness = LoadTexture(dir, mat, aiTextureType_DIFFUSE_ROUGHNESS)

			.DiffuseColor  = diffuse,
			.SpecularColor = specular,
			.EmissiveColor = emissive
		});

	return subMesh;
}

Ref<Texture> LoadTexture(const std::string& dir,
						 const aiMaterial* material, aiTextureType type)
{
	if(material->GetTextureCount(type) == 0)
		return nullptr;

	aiString path;
	if(material->GetTexture(type, 0, &path) == AI_FAILURE)
		return nullptr;

	return AssetImporter::GetTexture(std::string(path.data));
}

Ref<Mesh> AssetImporter::GetMesh(const std::string& path) {
	if(path == "")
		return nullptr;

	Assimp::Importer importer;
	uint32_t loadFlags = aiProcess_Triangulate
						| aiProcess_GenSmoothNormals
						| aiProcess_FlipUVs
						| aiProcess_JoinIdenticalVertices;
	const aiScene* scene = importer.ReadFile(path.c_str(), loadFlags);

	VOLCANICORE_ASSERT_ARGS(scene, "Error importing model(s) from %s: %s",
									path.c_str(), importer.GetErrorString());

	Ref<Mesh> mesh = CreateRef<Mesh>();
	mesh->SubMeshes.Reallocate(scene->mNumMeshes);
	for(uint32_t i = 0; i < scene->mNumMeshes; i++)
		mesh->SubMeshes.Add(LoadMesh(path, scene, i));

	return mesh;
}

Ref<Sound> AssetImporter::GetSound(const std::string& path) {

}

}