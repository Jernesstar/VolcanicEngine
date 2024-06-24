#pragma once

#include <Renderer/Texture.h>

namespace Magma {

class Animation {
public:
	float Speed;

public:
	Animation(std::vector<Ref<Texture>> frames);
	Animation(std::vector<std::string> frames);
	~Animation();

	void Render(TimeStep ts);

private:
	std::vector<Ref<Texture>> m_Frames;
};

}