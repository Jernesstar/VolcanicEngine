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
	Add<TransformComponent>();
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

		glm::vec3 moveDir = { dx, 0.0f, dz };
		float speed = 0.03f;
		if(dx || dz)
			tr += glm::normalize(moveDir) * speed;
	};
}

}
