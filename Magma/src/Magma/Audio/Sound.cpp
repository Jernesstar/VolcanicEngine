#include "Sound.h"

#include "AudioEngine.h"

namespace Magma::Audio {

Sound::Sound(SoLoud::AudioSource* source)
	: m_Source(source)
{
	
}

Sound::~Sound() {
	
}

void Sound::Play() {
	AudioEngine::Get()->play(*m_Source, Volume);
}

}