#pragma once

namespace Demo {

class Text : public Application {
public:
	Text();

	void OnUpdate(TimeStep ts) override;

private:
	Ref<Camera> camera;
	CameraController controller;
};

Text::Text() {

}

void Text::OnUpdate(TimeStep ts) {

}

}