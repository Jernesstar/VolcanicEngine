#include "AudioEngine.h"

namespace Magma::Audio {

static SoLoud::Soloud* s_Engine;

void AudioEngine::Init() {
	s_Engine = new SoLoud::Soloud;
	s_Engine->init();
}

void AudioEngine::Shutdown() {
	s_Engine->deinit();
	delete s_Engine;
}

SoLoud::Soloud* AudioEngine::Get() {
	return s_Engine;
}

}