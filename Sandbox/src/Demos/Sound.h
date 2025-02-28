#pragma once

#include <soloud.h>
#include <soloud_wav.h>

namespace Demo {

class Sound : public Application {
public:
	Sound();
	~Sound();

	void OnUpdate(TimeStep ts) override;

private:
	Ref<Camera> camera;
	CameraController controller;

	SoLoud::Soloud s_SoundEngine;
	SoLoud::Wav sound;
};

Sound::Sound() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});
	Events::RegisterListener<KeyPressedEvent>(
		[&](const KeyPressedEvent& event)
		{
			if(event.Key == Key::K && !event.IsRepeat)
				s_SoundEngine.play(sound, 10.0f);
		});

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 0.0f, 0.0f, 3.0f });
	controller = CameraController{ camera };

	s_SoundEngine.init();
	VOLCANICORE_ASSERT(sound.load("TestProj/Visual/Asset/Audio/Woosh2.wav") == 0);
}

Sound::~Sound() {
	s_SoundEngine.deinit();
}

void Sound::OnUpdate(TimeStep ts) {
	// VOLCANICORE_LOG_INFO("Update");
}

}