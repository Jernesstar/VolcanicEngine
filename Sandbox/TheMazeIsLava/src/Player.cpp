#include "Player.h"

#include <Core/Input.h>
#include <Renderer/Renderer3D.h>

#include "Asset.h"

using namespace VolcaniCore;
using namespace Magma::Physics;

namespace TheMazeIsLava {

Player::Player(ECS::World& world)
	: Entity(world.AddEntity("Player").GetHandle())
{
	Add<TransformComponent>(
		Transform{
			.Translation = { 0.0f, 1.0f, 0.0f },
			.Scale = glm::vec3(0.2f)
		});
	Add<MeshComponent>(Asset::Player);
	Add<RigidBodyComponent>(RigidBody::Type::Static);
	Add<ScriptComponent>()
	.OnInput =
	[](TransformComponent& tc) {
		if(Input::MouseButtonPressed(Mouse::LeftButton))
			return;

		auto& tr = tc.Translation;
		int dx = (Input::KeyPressed(Key::Right)	|| Input::KeyPressed(Key::D))
				-(Input::KeyPressed(Key::Left)	|| Input::KeyPressed(Key::A));
		int dz = (Input::KeyPressed(Key::Down)	|| Input::KeyPressed(Key::S))
				-(Input::KeyPressed(Key::Up)	|| Input::KeyPressed(Key::W));

		tr.x += (float)dx * 0.3f;
		tr.z += (float)dz * 0.3f;
	};
}

}
