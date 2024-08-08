#include "Camera.h"

#include "OrthographicCamera.h"
#include "StereographicCamera.h"

namespace VolcaniCore {

Ref<Camera> Camera::Create(Camera::Type type) {
	if(type == Camera::Type::Ortho) {
		return CreateRef<OrthographicCamera>();
	}
	return CreateRef<StereographicCamera>();
}

Ref<Camera> Camera::Create(Camera::Type type, float fov_rotation) {
	if(type == Camera::Type::Ortho) {
		return CreateRef<OrthographicCamera>(fov_rotation);
	}
	return CreateRef<StereographicCamera>(fov_rotation);
}

}