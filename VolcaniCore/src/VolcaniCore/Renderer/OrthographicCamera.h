#pragma once

#include "Camera.h"

namespace VolcaniCore {

class OrthographicCamera : public Camera {
public:
	OrthographicCamera();
	OrthographicCamera(float width, float height,
					   float near, float far, float rotation = 0.0f);
	~OrthographicCamera() = default;

	void SetProjection(float near, float far);

private:
	void CalculateView() override;
	void CalculateProjection() override;
};

}