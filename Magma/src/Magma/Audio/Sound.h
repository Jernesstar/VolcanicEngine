#pragma once

#include <soloud.h>

#include <VolcaniCore/Core/Defines.h>

namespace Magma::Audio {

class Sound {
public:
	float Volume = -1.0f;

public:
	Sound(SoLoud::AudioSource* source);
	~Sound();

	void Play();

	Buffer<float> GetData();

private:
	SoLoud::AudioSource* m_Source;
};

}