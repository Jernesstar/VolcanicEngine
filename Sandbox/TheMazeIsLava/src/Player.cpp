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
	Add<RigidBodyComponent>(RigidBody::Type::Dynamic);
	Add<ScriptComponent>()
	.OnInput =
	[](TransformComponent& tc) {
		auto& tr = tc.Translation;
		int dx = (Input::KeyPressed(Key::Right) || Input::KeyPressed(Key::A))
				-(Input::KeyPressed(Key::Left)	|| Input::KeyPressed(Key::D));
		int dz = (Input::KeyPressed(Key::Up)	|| Input::KeyPressed(Key::W))
				-(Input::KeyPressed(Key::Down)	|| Input::KeyPressed(Key::S));

		tr.x += (float)dx * 5.0f;
		tr.z += (float)dz * 5.0f;
	};
}

}
