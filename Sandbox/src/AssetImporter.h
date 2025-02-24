#pragma once

#include <VolcaniCore/Graphics/Mesh.h>
#include <VolcaniCore/Graphics/Texture.h>
#include <VolcaniCore/Graphics/Cubemap.h>

using namespace VolcaniCore;

namespace Demo {

class AssetImporter {
public:
	static Ref<Mesh> GetMesh(const std::string& path);
	static Ref<Texture> GetTexture(const std::string& path);
	static Ref<Cubemap> GetCubemap(const std::string& path);
};

}