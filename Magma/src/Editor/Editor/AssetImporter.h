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
	std::string Speccular;
	std::string Emissive;

	std::string operator[](uint32_t i) { return *(&this->Diffuse + i); }

	List<std::string>::iterator begin() { return begin(); }
	List<std::string>::iterator end() { return end(); }
	List<std::string>::const_iterator begin() const { return &this->Diffuse; }
	List<std::string>::const_iterator end()	const { return &this->Diffuse + 3; }
};

class AssetImporter {
public:
	static ImageData GetImageData(const std::string& path, bool flip = false);

	static Ref<Mesh> GetMesh(const std::string& path);
	static List<MaterialPaths> GetMeshMaterials(const std::string& path);
	static List<SubMesh> GetMeshData(const std::string& path);

	static Ref<Texture> GetTexture(const std::string& path);
	static Ref<Cubemap> GetCubemap(const std::string& path);

	static Ref<Sound> GetAudio(const std::string& path);
};

}