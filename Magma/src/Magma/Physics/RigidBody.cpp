#include "RigidBody.h"

#include <Physics/Physics.h>

namespace Magma::Physics {

Ref<RigidBody> RigidBody::Create(RigidBody::Type type,
								 const Shape& shape,
								 const Transform& t)
{
	if(type == RigidBody::Type::Static)
		return CreateRef<StaticBody>(shape, t);
	if(type == RigidBody::Type::Dynamic)
		return CreateRef<DynamicBody>(shape, t);
}

Ref<RigidBody> RigidBody::Create(RigidBody::Type type,
								 const Transform& t)
{
	if(type == RigidBody::Type::Static)
		return CreateRef<StaticBody>(t);
	if(type == RigidBody::Type::Dynamic)
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

void RigidBody::SetShape(const Shape& shape) {
	m_Actor->attachShape(*shape.m_Shape);
	m_HasShape = true;
}

// TODO: Scaled transforms

void RigidBody::UpdateTransform() {
	PxTransform pose = m_Actor->getGlobalPose();
	m_Transform.Translation = { pose.p.x, pose.p.y, pose.p.z };
	m_Transform.Rotation	= { pose.q.x, pose.q.y, pose.q.z };
}

void RigidBody::UpdateTransform(const Transform& t) {
	m_Transform = t;
	PxTransform pose{
		.p = { t.Translation.x, t.Translation.y, t.Translation.z }
		.q = { t.Rotation.x, t.Rotation.y, t.Rotation.z }
	};
	m_Actor->setGlobalPose(pose);
}

StaticBody::StaticBody(const Shape& shape, const Transform& t)
	: RigidBody(RigidBody::Type::Static, shape, t)
{
	PxTransform tr(PxVec3(t.Translation.x, t.Translation.y, t.Translation.z));
	m_Actor = GetPhysicsLib()->createRigidStatic(tr);
	m_Actor->userData = static_cast<void*>(this);

	SetShape(shape);
	UpdateTransform(t);
}

StaticBody::StaticBody(const Shape& shape, const Transform& t)
	: RigidBody(RigidBody::Type::Static, t)
{
	m_Actor = GetPhysicsLib()->createRigidStatic();
	m_Actor->userData = static_cast<void*>(this);

	UpdateTransform(t);
}

DynamicBody::DynamicBody(const Shape& shape, const Transform& t)
	: RigidBody(RigidBody::Type::Dynamic, shape, t)
{
	m_Actor = GetPhysicsLib()->createRigidDynamic();
	m_Actor->userData = static_cast<void*>(this);

	SetShape(shape);

	PxRigidBodyExt::updateMassAndInertia(*m_Actor->is<PxRigidDynamic>(), 10.0f);

	UpdateTransform(t);
}

DynamicBody::DynamicBody(const Transform& t)
	: RigidBody(RigidBody::Type::Dynamic, t)
{
	PxTransform tr(PxVec3(t.Translation.x, t.Translation.y, t.Translation.z));
	m_Actor = GetPhysicsLib()->createRigidDynamic(tr);
	m_Actor->userData = static_cast<void*>(this);

	PxRigidBodyExt::updateMassAndInertia(*m_Actor->is<PxRigidDynamic>(), 10.0f);

	UpdateTransform(t);
}

// void DynamicBody::SetVelocity(float velocity) {
// 	// m_Actor->setAngularDamping(0.5f);
// 	m_Actor->setLinearVelocity(velocity);
// }

}