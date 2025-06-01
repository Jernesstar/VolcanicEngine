#pragma once

#include <VolcaniCore/Graphics/Mesh.h>
#include <VolcaniCore/Graphics/Texture.h>
#include <VolcaniCore/Graphics/Cubemap.h>

#include <Magma/Audio/Sound.h>

using namespace Magma::Audio;

namespace Magma {
struct MaterialPaths {
	std::string Diffuse;
	std::string Specular;
	std::string Emissive;

	std::string operator[](uint32_t i) { return *(&Diffuse + i); }
};

class AssetImporter {
public:
	static VolcaniCore::ImageData GetImageData(const std::string& path,
		bool flip = true);

	static Ref<VolcaniCore::Mesh> GetMesh(const std::string& path);
	static void GetMeshData(const std::string& path,
		VolcaniCore::List<VolcaniCore::SubMesh>& mesh,
		VolcaniCore::List<MaterialPaths>& materialPaths);

	static Ref<VolcaniCore::Texture> GetTexture(const std::string& path,
		bool flip = true);
	static Ref<VolcaniCore::Cubemap> GetCubemap(const std::string& path);

	static VolcaniCore::Buffer<uint32_t> GetShaderData(const std::string& name);

	static VolcaniCore::Buffer<float> GetAudioData(const std::string& path);
	static Ref<Sound> GetAudio(const std::string& path);
};

}