#pragma once

#include <soloud.h>
#include <soloud_wav.h>

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/Buffer.h>

using namespace VolcaniCore;

namespace Magma::Audio {

class Sound {
public:
	float Volume = -1.0f;

public:
	Sound();
	~Sound();

	void Play();

	SoLoud::Wav& GetInternal() { return m_Sound; }

private:
	SoLoud::Wav m_Sound;
};

}