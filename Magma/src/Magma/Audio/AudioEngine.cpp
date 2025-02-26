#include "AudioEngine.h"

namespace Magma::Audio {

static SoLoud::Soloud s_Engine;

void AudioEngine::Init() {
	s_Engine.init();
}

void AudioEngine::Close() {
	s_Engine.deinit();
}

SoLoud::Soloud* AudioEngine::Get() {
	return &s_Engine;
}

}