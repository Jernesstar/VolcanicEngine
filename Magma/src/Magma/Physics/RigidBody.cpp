#include "RigidBody.h"

#include <Physics/Physics.h>

namespace Magma::Physics {

// TODO: Scaled transforms
RigidBody::RigidBody(RigidBodyType type, const Shape& shape, const Transform& t)
	: Type(type), m_Transform(t)
{
	PxTransform tr(t.Translation.x, t.Translation.y, t.Translation.z);
	if(type == RigidBodyType::Static)
		m_Actor = GetPhysicsLib()->createRigidStatic(tr);
	if(type == RigidBodyType::Dynamic)
		m_Actor = GetPhysicsLib()->createRigidDynamic(tr);

	m_Actor->attachShape(*shape.m_Shape);

	m_Actor->userData = this;
}

RigidBody::~RigidBody() {

}

void RigidBody::UpdateTransform() {
	auto pose = m_Actor->getGlobalPose();
	// m_Transform.Translation = { pose.p.x, pose.p.y, pose.p.z };
	// m_Transform.Rotation	= { pose.q.x, pose.q.y, pose.q.z };
}

StaticBody::StaticBody(const Shape& shape, const Transform& t)
	: RigidBody(RigidBodyType::Static, shape, t)
{

}

DynamicBody::DynamicBody(const Shape& shape, const Transform& t)
	: RigidBody(RigidBodyType::Dynamic, shape, t)
{
	PxRigidBodyExt::updateMassAndInertia(*m_Actor->is<PxRigidDynamic>(), 10.0f);

	// m_Actor->setAngularDamping(0.5f);
	// m_Actor->setLinearVelocity(velocity);
}

}