#pragma once

#include "Panel.h"

#include <Magma/ECS/Entity.h>

using namespace Magma::ECS;

namespace Magma {

class ComponentEditorPanel : public Panel {
public:
	ComponentEditorPanel();
	~ComponentEditorPanel() = default;

	void SetContext(Entity& entity);

	template<typename TComponent>
	void SetFocus();

	void Update(TimeStep ts) override;
	void Draw() override;

private:
	Entity m_Context;
	uint32_t m_ComponentFocus;
};

}