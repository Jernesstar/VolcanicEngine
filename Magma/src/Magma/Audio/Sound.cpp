#include "Sound.h"

#include <soloud_wav.h>

#include "AudioEngine.h"

namespace Magma::Audio {

Sound::Sound() {

}

Sound::~Sound() {

}

void Sound::Play(float volume) {
	AudioEngine::Get()->play(m_Sound, volume);
}

}