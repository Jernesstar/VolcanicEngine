#pragma once

#include <VolcaniCore/Graphics/Mesh.h>
#include <VolcaniCore/Graphics/Texture.h>
#include <VolcaniCore/Graphics/Cubemap.h>

#include <Magma/Audio/Sound.h>

using namespace VolcaniCore;
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
	static ImageData GetImageData(const std::string& path, bool flip = false);

	static Ref<Mesh> GetMesh(const std::string& path);
	static void GetMeshData(const std::string& path,
		List<SubMesh>& mesh, List<MaterialPaths>& materialPaths);

	static Ref<Texture> GetTexture(const std::string& path);
	static Ref<Cubemap> GetCubemap(const std::string& path);

	static Buffer<float> GetAudioData(const std::string& path);
	static Ref<Sound> GetAudio(const std::string& path);
};

}