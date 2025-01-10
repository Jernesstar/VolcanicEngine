#pragma once

#include "Panel.h"

#include "Entity.h"

using namespace Magma::ECS;

namespace Magma {

class ComponentPanel : public Panel {
public:
	ComponentPanel();
	~ComponentPanel() = default;

	void SetContext(Entity& entity);

	void Update(TimeStep ts) override;
	void Draw() override;

private:
	Entity m_Context;
};

}