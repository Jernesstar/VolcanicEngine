#pragma once

#include <soloud.h>
#include <soloud_wav.h>

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/Buffer.h>

namespace Magma::Audio {

class Sound {
public:
	Sound();
	~Sound();

	void Play(float volume = -1.0f);

	SoLoud::Wav& GetInternal() { return m_Sound; }

private:
	SoLoud::Wav m_Sound;
};

}