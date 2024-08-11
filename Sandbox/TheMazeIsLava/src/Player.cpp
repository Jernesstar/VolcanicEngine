#include "Player.h"

#include <Core/Input.h>
#include <Renderer/Renderer3D.h>

#include "GameState.h"

using namespace VolcaniCore;
using namespace Magma::Physics;

namespace TheMazeIsLava {

Player::Player(ECS::World& world)
	: Entity(world.AddEntity().GetHandle())
{
	Add<TransformComponent>();
	// TODO(codespace): Add Player model
	// Add<MeshComponent>(GameState::PlayerModel1);
	Add<RigidBodyComponent>(RigidBody::Type::Dynamic);
	Add<ScriptComponent>()
	.OnInput = [](TransformComponent& tc) {
		auto& tr = tc.Translation;
		auto dx = ((Input::KeyPressed(Key::Right) || Input::KeyPressed(Key::A))
				  -(Input::KeyPressed(Key::Left) || Input::KeyPressed(Key::D)));
		auto dy = ((Input::KeyPressed(Key::Up) || Input::KeyPressed(Key::W))
				  -(Input::KeyPressed(Key::Down) || Input::KeyPressed(Key::S)));

		tr.x += dx * 5.0f;
		tr.t += dy * 5.0f;
	};
}

}
