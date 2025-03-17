#pragma once

#include <soloud.h>

namespace Magma::Audio {

class AudioEngine {
public:
	static void Init();
	static void Shutdown();
	static SoLoud::Soloud* Get();
};

}