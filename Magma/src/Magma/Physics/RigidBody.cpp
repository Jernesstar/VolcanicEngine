#include "RigidBody.h"

#include <Physics/Physics.h>

namespace Magma::Physics {

Ref<RigidBody> RigidBody::Create(RigidBody::Type type,
								 const Shape& shape,
								 const Transform& t)
{
	if(type == RigidBody::Type::Static)
		return CreateRef<StaticBody>(shape, t);
	return CreateRef<DynamicBody>(shape, t);
}

Ref<RigidBody> RigidBody::Create(RigidBody::Type type,
								 const Transform& t)
{
	if(type == RigidBody::Type::Static)
		return CreateRef<StaticBody>(t);
	return CreateRef<DynamicBody>(t);
}

RigidBody::RigidBody(RigidBody::Type type, const Shape& shape,
					 const Transform& t)
	: m_Type(type), m_Transform(t), m_HasShape(true),
		m_ShapeType(shape.GetType()) { }

RigidBody::RigidBody(RigidBody::Type type, const Transform& t)
	: m_Type(type), m_Transform(t), m_HasShape(false) { }

RigidBody::~RigidBody() {
	m_Actor->release();
}

// TODO(Fix): Scaled transforms
void RigidBody::UpdateTransform() {
	PxTransform pose = m_Actor->getGlobalPose();
	m_Transform.Translation = { pose.p.x, pose.p.y, pose.p.z };
	m_Transform.Rotation	= { pose.q.x, pose.q.y, pose.q.z };
}

void RigidBody::UpdateTransform(const Transform& t) {
	m_Transform = t;
	PxTransform pose;
	pose.p = PxVec3(t.Translation.x, t.Translation.y, t.Translation.z);
	pose.q = PxQuat(t.Rotation.x, t.Rotation.y, t.Rotation.z, 1.0);
	m_Actor->setGlobalPose(pose);
}

StaticBody::StaticBody(const Shape& shape, const Transform& t)
	: RigidBody(RigidBody::Type::Static, shape, t)
{
	m_Actor = GetPhysicsLib()->createRigidStatic(PxTransform(PxVec3(0.0f)));
	m_Actor->userData = static_cast<void*>(this);

	SetShape(shape);
	UpdateTransform(t);
}

StaticBody::StaticBody(const Transform& t)
	: RigidBody(RigidBody::Type::Static, t)
{
	m_Actor = GetPhysicsLib()->createRigidStatic(PxTransform(PxVec3(0.0f)));
	m_Actor->userData = static_cast<void*>(this);

	UpdateTransform(t);
}

void StaticBody::SetShape(const Shape& shape) {
	m_Actor->attachShape(*shape.m_Shape);
	m_HasShape = true;
	m_ShapeType = shape.GetType();
}

DynamicBody::DynamicBody(const Shape& shape, const Transform& t)
	: RigidBody(RigidBody::Type::Dynamic, shape, t)
{
	m_Actor = GetPhysicsLib()->createRigidDynamic(PxTransform(PxVec3(0.0f)));
	m_Actor->userData = static_cast<void*>(this);
	// m_Actor
	// ->is<PxRigidBody>()->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	PxRigidBodyExt::updateMassAndInertia(*m_Actor->is<PxRigidDynamic>(), 10.0f);

	SetShape(shape);
	UpdateTransform(t);
}

// TODO(Fix): attachShape for non-kinematic dynamic bodies not supported
DynamicBody::DynamicBody(const Transform& t)
	: RigidBody(RigidBody::Type::Dynamic, t)
{
	m_Actor = GetPhysicsLib()->createRigidDynamic(PxTransform(PxVec3(0.0f)));
	m_Actor->userData = static_cast<void*>(this);

	PxRigidBodyExt::updateMassAndInertia(*m_Actor->is<PxRigidDynamic>(), 10.0f);

	UpdateTransform(t);
}

void DynamicBody::SetShape(const Shape& shape) {
	// m_Shape =
	// PhysX::PxRigidActorExt::createExclusiveShape(*m_Actor,
	// 	shape.m_Shape->getGeometry(), *shape.m_Shape->getMaterials()[0]),
}

// void DynamicBody::SetVelocity(float velocity) {
// 	// m_Actor->setAngularDamping(0.5f);
// 	m_Actor->setLinearVelocity(velocity);
// }

}