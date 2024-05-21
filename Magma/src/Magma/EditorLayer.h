#pragma once

#include <VolcaniCore/Core/Time.h>

namespace Magma {

class EditorLayer {
public:
	EditorLayer();
	~EditorLayer();

	void Update(VolcaniCore::TimeStep ts);
	void Render();
};

}