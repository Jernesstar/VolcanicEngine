#pragma once

#include <VolcanicCore/Core/Time.h>

namespace Magma {

class EditorLayer {
public:
	EditorLayer();
	~EditorLayer();

	void Update(TimeStep ts);
	void Render();
};

}