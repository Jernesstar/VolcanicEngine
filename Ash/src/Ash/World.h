#pragma once

#include <VolcaniCore/Core/Defines.h>

#include "Object.h"

using namespace VolcaniCore;

namespace Ash {

class World {
public:
	World();
	~World();

	void Add(Ref<Object> object);

private:
	
};

}