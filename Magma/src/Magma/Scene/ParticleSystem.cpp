#include "ParticleSystem.h"

#include "ECS/World.h"

#include <VolcaniCore/Core/Log.h>

using namespace Magma::Physics;

namespace Magma {

void ParticleSystem::Update(TimeStep ts) {
	m_TimeStep = ts;
}

void ParticleSystem::Run(Phase phase) {
	if(phase == Phase::OnUpdate) {
		m_EntityWorld
		->ForEach<ParticleEmitterComponent>(
			[this](Entity& entity)
			{
				auto [pc] = GetRequired(entity);

			});
	}
}

void ParticleSystem::OnComponentAdd(Entity& entity) {

}

void ParticleSystem::OnComponentSet(Entity& entity) {

}

void ParticleSystem::OnComponentRemove(Entity& entity) {

}

}