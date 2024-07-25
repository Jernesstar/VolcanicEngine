#pragma once

namespace Magma {

class RigidBody {
public:
	enum class Type {
		Static, // Infinte mass/inertia
		Dynamic
	};

	const Type Type;

public:
	RigidBody(Type type);
	// RigidBody(Type type, const Shape& shape);
	~RigidBody();

protected:
	PxRigidActor* m_Actor;
};

class StaticBody : public RigidBody {
public:
	StaticBody();
	~StaticBody();

private:
	
};

class DynamicBody : public RigidBody {
public:
	DynamicBody();
	~DynamicBody();

private:
	
};

}