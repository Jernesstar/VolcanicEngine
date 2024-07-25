#include "RigidBody.h"

namespace Magma {

RigidBody::RigidBody(Type type)
	: Type(type)
{
	if(type == Type::Static)
		m_Actor = gPhysics->createRigidStatic(t);
	if(type == Type::Dynamic)
		m_Actor = gPhysics->createRigidDynamic(t);

	float r = 1.0f;
	PxTransform t(PxVec3(0.0, 0.0, 0.0));
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(r, r, r), *material);

	m_Actor->attachShape(*shape);
	shape->release();
}

RigidBody::~RigidBody() {
	
}

StaticBody::StaticBody()
	: RigidBody(Type::Static)
{

}

DynamicBody::DynamicBody()
	: RigidBody(Type::Dynamic)
{
	PxRigidBodyExt::updateMassAndInertia(*m_Actor, 10.0f);
}

}