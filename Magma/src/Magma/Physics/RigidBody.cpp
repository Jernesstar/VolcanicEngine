#include "RigidBody.h"

#include <Physics/Physics.h>

namespace Magma::Physics {

Ref<RigidBody> RigidBody::Create(RigidBody::Type type, Ref<Shape> shape,
								 const Transform& t)
{
	if(type == RigidBody::Type::Static)
		return CreateRef<StaticBody>(shape, t);
	return CreateRef<DynamicBody>(shape, t);
}

RigidBody::RigidBody(RigidBody::Type type, Ref<Shape> shape,
					 const Transform& t)
	: m_Type(type), m_Shape(shape), m_Transform(t) { }

RigidBody::~RigidBody() {
#ifdef MAGMA_PHYSICS
	m_Actor->release();
#endif
}

// TODO(Fix): Scaled transforms
void RigidBody::UpdateTransform() {
#ifdef MAGMA_PHYSICS
	PxTransform pose = m_Actor->getGlobalPose();
	m_Transform.Translation = { pose.p.x, pose.p.y, pose.p.z };
	m_Transform.Rotation	= { pose.q.x, pose.q.y, pose.q.z };
#endif
}

void RigidBody::SetTransform(const Transform& t) {
#ifdef MAGMA_PHYSICS
	m_Transform = t;
	PxTransform pose;
	pose.p = PxVec3(t.Translation.x, t.Translation.y, t.Translation.z);
	pose.q = PxQuat(t.Rotation.x, t.Rotation.y, t.Rotation.z, 1.0);
	m_Actor->setGlobalPose(pose);
#endif
}

StaticBody::StaticBody(Ref<Shape> shape, const Transform& t)
	: RigidBody(RigidBody::Type::Static, shape, t)
{
#ifdef MAGMA_PHYSICS
	m_Actor = GetPhysicsLib()->createRigidStatic(PxTransform(PxVec3(0.0f)));
	m_Actor->userData = static_cast<void*>(this);
#endif

	SetShape(shape);
	SetTransform(t);
}

void StaticBody::SetShape(Ref<Shape> shape) {
#ifdef MAGMA_PHYSICS
	m_Shape = shape;
	m_Actor->attachShape(*shape->m_Shape);
#endif
}

// TODO(Fix): attachShape for non-kinematic dynamic bodies not supported
DynamicBody::DynamicBody(Ref<Shape> shape, const Transform& t)
	: RigidBody(RigidBody::Type::Dynamic, shape, t)
{
#ifdef MAGMA_PHYSICS
	m_Actor = GetPhysicsLib()->createRigidDynamic(PxTransform(PxVec3(0.0f)));
	m_Actor->userData = static_cast<void*>(this);
	// m_Actor
	// ->is<PxRigidBody>()->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	PxRigidBodyExt::updateMassAndInertia(*m_Actor->is<PxRigidDynamic>(), 10.0f);
#endif

	SetShape(shape);
	SetTransform(t);
}

void DynamicBody::SetShape(Ref<Shape> shape) {
#ifdef MAGMA_PHYSICS
	// m_Shape->m_Shape =
	// 	PxRigidActorExt::createExclusiveShape(*m_Actor,
	// 		shape->m_Shape->getGeometry(), *shape->m_Shape->getMaterials()[0]);
#endif
}

void DynamicBody::ApplyForce(const glm::vec3& f) {
#ifdef MAGMA_PHYSICS
	// m_Actor->setAngularDamping(0.5f);
	// m_Actor->setLinearVelocity(velocity);
#endif
}

}