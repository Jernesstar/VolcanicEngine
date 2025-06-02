#pragma once

#include <VolcaniCore/Graphics/Mesh.h>
#include <VolcaniCore/Graphics/Texture.h>
#include <VolcaniCore/Graphics/Cubemap.h>
#include <VolcaniCore/Graphics/Shader.h>

#include <Magma/Audio/Sound.h>

using namespace VolcaniCore;
using namespace Magma::Audio;

namespace Demo {

struct ShaderFile {
	const std::string Path;
	const ShaderType Type;
};

class AssetImporter {
public:
	static ImageData GetImageData(const std::string& path, bool flip = false);

	static Ref<Mesh> GetMesh(const std::string& path);
	static Ref<Texture> GetTexture(const std::string& path);
	static Ref<Cubemap> GetCubemap(const std::string& path);
	static Ref<ShaderPipeline> GetShader(const List<ShaderFile>& files);
	static Ref<Sound> GetAudio(const std::string& path);
};

}