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

namespace Demo {

ImageData AssetImporter::GetImageData(const std::string& path, bool flip) {
	stbi_set_flip_vertically_on_load((int)flip);
	ImageData image;
	int width, height, bpp;
	uint8_t* pixels = stbi_load(path.c_str(), &width, &height, &bpp, 4);
	if(!pixels) {
		VOLCANICORE_LOG_WARNING("Could not load image '%s'", path.c_str());
		return { };
	}

	image.Width = (uint32_t)width;
	image.Height = (uint32_t)height;
	image.Data = Buffer(pixels, image.Width * image.Height * 4);
	return image;
}

Ref<Texture> AssetImporter::GetTexture(const std::string& path) {
	ImageData image = GetImageData(path, false);
	auto texture = Texture::Create(image.Width, image.Height);
	texture->SetData(image.Data);
	return texture;
}

Ref<Cubemap> AssetImporter::GetCubemap(const std::string& folder) {
	List<fs::path> paths;
	for(auto path : FileUtils::GetFiles(folder, { ".png", ".jpg", ".jpeg" }))
		paths.Add(fs::path(path));

	if(paths.Count() < 6)
		VOLCANICORE_LOG_WARNING(
			"Folder %s does not have at least 6 images", folder.c_str());

	Map<std::string, int> map =
	{
		{ "right", 0 }, { "left",	1 },
		{ "top",   2 }, { "bottom", 3 },
		{ "front", 4 }, { "back",	5 }
	};
	List<ImageData> output(6);

	for(auto& path : paths)
		output.Insert(map[path.filename().string()], GetImageData(path));

	return Cubemap::Create(output);
}

static SubMesh LoadMesh(const std::string& path, const aiMesh* mesh) {
	List<Vertex> vertices(mesh->mNumVertices);
	List<uint32_t> indices(mesh->mNumFaces * 3);

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

	return { vertices, indices, (int32_t)mesh->mMaterialIndex };
}

static Ref<Texture> LoadTexture(const std::string& dir,
	const aiMaterial* material, aiTextureType type)
{
	if(material->GetTextureCount(type) == 0)
		return nullptr;
	aiString path;
	if(material->GetTexture(type, 0, &path) == AI_FAILURE)
		return nullptr;

	return AssetImporter::GetTexture(std::string(path.data));
}

static Material LoadMaterial(const std::string& dir, const aiMaterial* mat) {
	glm::vec4 diffuse = glm::vec4(0.0f);
	glm::vec4 specular = glm::vec4(0.0f);
	glm::vec4 emissive = glm::vec4(0.0f);

	aiColor4D color;
	if(mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
		diffuse = glm::vec4(color.r, color.g, color.b, color.a);
	if(mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
		specular = glm::vec4(color.r, color.g, color.b, color.a);
	if(mat->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
		emissive = glm::vec4(color.r, color.g, color.b, color.a);

	return Material{
		.Diffuse  = LoadTexture(dir, mat, aiTextureType_DIFFUSE),
		.Specular = LoadTexture(dir, mat, aiTextureType_SPECULAR),
		.Emissive = LoadTexture(dir, mat, aiTextureType_EMISSIVE),
		// .Roughness = LoadTexture(dir, mat, aiTextureType_DIFFUSE_ROUGHNESS)

		.DiffuseColor  = diffuse,
		.SpecularColor = specular,
		.EmissiveColor = emissive
	};
}

Ref<Mesh> AssetImporter::GetMesh(const std::string& path) {
	namespace fs = std::filesystem;
	if(path == "")
		return nullptr;

	Assimp::Importer importer;
	uint32_t loadFlags = aiProcess_Triangulate
						| aiProcess_GenSmoothNormals
						| aiProcess_FlipUVs
						| aiProcess_JoinIdenticalVertices;
	const aiScene* scene = importer.ReadFile(path.c_str(), loadFlags);

	VOLCANICORE_ASSERT_ARGS(scene, "Error importing mesh from %s: %s",
							path.c_str(), importer.GetErrorString());

	Ref<Mesh> mesh = CreateRef<Mesh>(MeshType::Model);
	mesh->SubMeshes.Allocate(scene->mNumMeshes);
	mesh->Materials.Allocate(scene->mNumMaterials);

	for(uint32_t i = 0; i < scene->mNumMeshes; i++)
		mesh->SubMeshes.Add(LoadMesh(path, scene->mMeshes[i]));

	auto dir = (fs::path(path).parent_path() / "textures").string();
	for(uint32_t i = 0; i < scene->mNumMaterials; i++)
		mesh->Materials
			.Add(
				LoadMaterial(
					dir, scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]));

	return mesh;
}

Ref<Sound> AssetImporter::GetAudio(const std::string& path) {
	auto source = new SoLoud::Wav;
	VOLCANICORE_ASSERT(source->load(path.c_str()) == 0);
	return CreateRef<Sound>(source);
}

}