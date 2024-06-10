#pragma once

#include <vector>
#include <string>
#include <tinyxml2.h>
#include "common/math3d.h"
#include "common/common.h"

class Component;
class TransformComponent;
class Scene;
class StaticMeshComponent;
class CameraComponent;
class CollisionComponent;
class AudioComponent;
class SkeletalMeshComponent;

using namespace tinyxml2;

class Entity
{
public:

	Entity();

	virtual bool Init() { return false; }

	virtual void Update(float DeltaTime);

	virtual void Destroy();

	void AddComponent(class Component* Comp);
	void AttachComponent(class TransformComponent* Comp);
	void AttachChildToComponent(class TransformComponent* Parent, class TransformComponent* Child);
	
	bool LoadEntityDef(const char* Path);
	std::string GetEntityDefStringValue(const char* ElementName, const char* AttributeName);
	bool GetEntityDefBoolValue(const char* ElementName, const char* AttributeName);
	int GetEntityDefIntValue(const char* ElementName, const char* AttributeName);
	float GetEntityDefFloatValue(const char* ElementName, const char* AttributeName);

	class TransformComponent* RootComponent;

	// this holds only components that ARE NOT TransformComponents
	std::vector<class Component*> OwnedComponents;
	
	class Scene* ParentScene;
	std::string Name;

	std::string EntityDefPath;
	XMLDocument EntityDef;
	bool EntityDefLoaded;

	Vector3 SpawnPosition;
	Vector3 SpawnRotation;
	Vector3 SpawnScale;

	bool bVisible;
};

class StaticMeshEntity : public Entity
{
public:

	StaticMeshEntity();

	virtual bool Init() override;

	class StaticMeshComponent* Mesh;
	class CollisionComponent* Collision;
};

class SkeletalMeshEntity : public Entity
{
public:

	SkeletalMeshEntity();

	virtual bool Init() override;

	class SkeletalMeshComponent* Mesh;
	class CollisionComponent* Collision;

	int32 AnimToPlay;
};

class SoundEntity : public Entity
{
public:

	SoundEntity();

	virtual bool Init() override;

	class AudioComponent* Sound;
};

class LightEntity : public Entity
{
public:

	LightEntity();

	virtual bool Init() override;

	class LightComponent* LightComp;
	int32 Index;
};