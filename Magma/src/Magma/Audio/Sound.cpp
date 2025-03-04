#include "Sound.h"

#include <soloud_wav.h>

#include "AudioEngine.h"

namespace Magma::Audio {

Sound::Sound(Buffer<float> data) {

}

Sound::~Sound() {
	// delete m_Source;
}

void Sound::Play() {
	AudioEngine::Get()->play(m_Sound, Volume);
}

}