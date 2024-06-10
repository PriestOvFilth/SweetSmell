#pragma once

#include <vector>

#pragma warning(push)
#pragma warning(disable: 4305) // for a warning in a bullet header
#include <btBulletDynamicsCommon.h>
#pragma warning(pop)

#include "common/common.h"
#include "common/math3d.h"

class CollisionComponent;

struct CollisionResult
{
	bool bCollided;
	Vector3 Normal;
	float Distance;
};

class CollisionManager
{
public:

	void Init(float WorldSize);
	void Update(float DeltaTime);
	void Destroy();

	Vector3 GetComponentPosition(class CollisionComponent* Comp);
	void SetComponentPosition(class CollisionComponent* Comp, Vector3 NewPosition);
	
	bool IsColliding(class CollisionComponent* Comp);
	CollisionResult TestCollision(class CollisionComponent* Comp);
	
	void TrackObject(class CollisionComponent* Comp);

	btCollisionWorld *World;
	btDefaultCollisionConfiguration *CollisionConfiguration;
	btCollisionDispatcher *Dispatcher;
	btBroadphaseInterface*  Broadphase;

	std::vector<class CollisionComponent*> TrackedComps;
};

class PhysicsEngine
{
public:

	PhysicsEngine(bool bUseOnlyCollision);
	~PhysicsEngine();

	void Init(float WorldSize);
	void Update(float DeltaTime);
	void Destroy();

	void TrackComponent(class CollisionComponent* Comp);

	class CollisionManager* Collision;
	bool bCollisionOnly;
};