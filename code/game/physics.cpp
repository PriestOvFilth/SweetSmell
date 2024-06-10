#include "physics.h"
#include "game/component.h"

using namespace std;

void CollisionManager::Init(float WorldSize)
{
	btVector3 WorldAABBMin(-WorldSize, -WorldSize, -WorldSize);
	btVector3 WorldAABBMax(WorldSize, WorldSize, WorldSize);

	CollisionConfiguration = new btDefaultCollisionConfiguration();
	Dispatcher = new btCollisionDispatcher(CollisionConfiguration);
	Broadphase = new bt32BitAxisSweep3(WorldAABBMin, WorldAABBMax);
	World = new btCollisionWorld(Dispatcher, Broadphase, CollisionConfiguration);
}

void CollisionManager::Update(float DeltaTime)
{
	World->performDiscreteCollisionDetection();
}

void CollisionManager::Destroy()
{
	// we can't use SAFE_DELETE here because it results in a heap corruption error
	delete World;
	World = NULL;

	delete Broadphase;
	Broadphase = NULL;

	delete Dispatcher;
	Dispatcher = NULL;

	delete CollisionConfiguration;
	CollisionConfiguration = NULL;

	TrackedComps.clear();
	TrackedComps.resize(0);
}

Vector3 CollisionManager::GetComponentPosition(class CollisionComponent* Comp)
{
	if (Comp)
	{
		btCollisionObject* Object = Comp->CollisionObject;
		if (Object)
		{
			btVector3 btPos = Object->getWorldTransform().getOrigin();
			Vector3 Pos = Vector3(btPos.getX(), btPos.getY(), btPos.getZ());
			return Pos;
		}
	}

	return Vector3::Zero;
}
void CollisionManager::SetComponentPosition(class CollisionComponent* Comp, Vector3 NewPosition)
{
	if (Comp)
	{
		btCollisionObject* Object = Comp->CollisionObject;
		if (Object)
		{
			Object->getWorldTransform().setOrigin(btVector3(NewPosition.x, NewPosition.y, NewPosition.z));
			Object->activate(true);
		}
	}
}

bool CollisionManager::IsColliding(CollisionComponent* Comp)
{
	if (Comp)
	{
		CollisionResult Result = TestCollision(Comp);
		if (Result.bCollided)
		{
			return true;
		}
	}

	return false;
}

CollisionResult CollisionManager::TestCollision(class CollisionComponent* Comp)
{
	CollisionResult Result = {};

	int32 NumAdds = 0;

	if (Comp)
	{
		int32 NumManifolds = World->getDispatcher()->getNumManifolds();
		for (int32 i = 0; i < NumManifolds; i++)
		{
			btPersistentManifold* ContactManifold = World->getDispatcher()->getManifoldByIndexInternal(i);

			btCollisionObject* ObjectA = (btCollisionObject*)ContactManifold->getBody0();
			btCollisionObject* ObjectB = (btCollisionObject*)ContactManifold->getBody1();

			ContactManifold->refreshContactPoints(ObjectA->getWorldTransform(), ObjectB->getWorldTransform());

			if ((ObjectA == Comp->CollisionObject) || (ObjectB == Comp->CollisionObject))
			{
				int32 NumContacts = ContactManifold->getNumContacts();

				for (int32 j = 0; j < NumContacts; j++)
				{
					btManifoldPoint& PT = ContactManifold->getContactPoint(j);

					float Dist = (float)PT.getDistance();
					if (Dist < 0.0f)
					{
						btVector3 Vec = PT.m_normalWorldOnB;
						Result.Normal += Vector3(Vec.getX(), Vec.getY(), Vec.getZ());
						Result.Distance += Dist;
						NumAdds++;
					}
				}

				Result.bCollided = true;
			}
		}
	}

	if (NumAdds > 0)
	{
		Result.Normal = Result.Normal / (float)NumAdds;
		Result.Distance = Result.Distance / (float)NumAdds;
	}

	return Result;
}

void CollisionManager::TrackObject(class CollisionComponent* Comp)
{
	World->addCollisionObject(Comp->CollisionObject);
	Comp->CollisionObject->setUserPointer((void*)Comp);
	Comp->CollisionObject->activate(true);
	TrackedComps.push_back(Comp);
}

PhysicsEngine::PhysicsEngine(bool bUseOnlyCollision)
{
	bCollisionOnly = bUseOnlyCollision;
	Collision = new CollisionManager();
}

PhysicsEngine::~PhysicsEngine()
{
	SAFE_DELETE(Collision);
}

void PhysicsEngine::Init(float WorldSize)
{
	WriteLog("Creating physics world with world size %.1f\n", WorldSize);
	if (bCollisionOnly)
	{
		Collision->Init(WorldSize);
	}
}

void PhysicsEngine::Update(float DeltaTime)
{
	if (bCollisionOnly)
	{
		Collision->Update(DeltaTime);
	}
}

void PhysicsEngine::Destroy()
{
	Collision->Destroy();
}

void PhysicsEngine::TrackComponent(CollisionComponent* Comp)
{
	if (bCollisionOnly)
	{
		Collision->TrackObject(Comp);
	}
}