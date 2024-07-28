#include "RigidBody.h"

#include <Physics/Physics.h>

namespace Magma::Physics {

RigidBody::RigidBody(RigidBodyType type)
	: Type(type)
{
	auto physics = PhysicsSystem::s_Physics;
	PxTransform t(PxVec3(0.0, 0.0, 0.0));

	if(type == RigidBodyType::Static)
		m_Actor = physics->createRigidStatic(PxTransform());
	if(type == RigidBodyType::Dynamic)
		m_Actor = physics->createRigidDynamic(PxTransform());

	m_Actor->userData = this;
}

RigidBody::~RigidBody() {
	
}

StaticBody::StaticBody()
	: RigidBody(RigidBodyType::Static)
{

}

DynamicBody::DynamicBody()
	: RigidBody(RigidBodyType::Dynamic)
{
	PxRigidBodyExt::updateMassAndInertia(*m_Actor->is<PxRigidDynamic>(), 10.0f);
}

}