#pragma once

#include <vector>

#pragma warning(push)
#pragma warning(disable: 4305) // for a warning in a bullet header
#include <btBulletCollisionCommon.h>
#pragma warning(pop)

#include "common/common.h"
#include "common/math3d.h"
#include "engine/light.h"

class Entity;
class StaticMesh;
class SkeletalMesh;
class Camera;
class SoundBase;
class Mesh;

enum EAxis
{
	AXIS_X,
	AXIS_Y,
	AXIS_Z
};

enum ECollisionShape
{
	CS_Capsule,
	CS_Cone,
	CS_Cylinder,
	CS_Plane,
	CS_Box,
	CS_Sphere,
	CS_Mesh
};

enum ELightType
{
	LT_Point,
	LT_Spot,
	LT_Ambient,
	LT_Other // used for SetShaderTransforms
};

class Component
{
public:

	virtual void Init() = 0;
	virtual void Update(float DeltaTime) = 0;
	virtual void Destroy() = 0;

	class Entity* Owner;
	std::string Name;
};

class TransformComponent : public Component
{
public:

	virtual void Init() override;
	virtual void Update(float DeltaTime) override;
	virtual void Destroy() override;

	void AttachChild(class TransformComponent* Comp);

	void SetPosition(float x, float y, float z)
	{
		PrevWorldPos = WorldPosition;

		WorldPosition.x = x;
		WorldPosition.y = y;
		WorldPosition.z = z;

		UpdateTransform();
	}

	void SetPosition(Vector3 P)
	{
		PrevWorldPos = WorldPosition;
		WorldPosition = P;
		
		UpdateTransform();
	}

	void AddPosition(float x, float y, float z)
	{
		PrevWorldPos = WorldPosition;

		WorldPosition.x += x;
		WorldPosition.y += y;
		WorldPosition.z += z;

		UpdateTransform();
	}

	void AddPosition(Vector3 P)
	{
		PrevWorldPos = WorldPosition;
		
		WorldPosition.x += P.x;
		WorldPosition.y += P.y;
		WorldPosition.z += P.z;

		UpdateTransform();
	}

	void SetRotation(float x, float y, float z)
	{
		Rotation.x = x;
		Rotation.y = y;
		Rotation.z = z;

		UpdateTransform();
	}

	void SetRotation(Vector3 R)
	{
		Rotation = R;

		UpdateTransform();
	}

	void AddRotation(float Angle, enum EAxis Axis)
	{
		switch (Axis)
		{
			case AXIS_X:
				SetRotation(Rotation.x + Angle, Rotation.y, Rotation.z);
				break;
			case AXIS_Y:
				SetRotation(Rotation.x, Rotation.y + Angle, Rotation.z);
				break;
			case AXIS_Z:
				SetRotation(Rotation.x, Rotation.y, Rotation.z + Angle);
				break;
		}
	}

	void SetScale(float x, float y, float z)
	{
		Scale.x = x;
		Scale.y = y;
		Scale.z = z;

		UpdateTransform();
	}

	void SetScale(Vector3 S)
	{
		Scale = S;

		UpdateTransform();
	}

	void AddScale(float x, float y, float z)
	{
		Scale.x *= x;
		Scale.y *= y;
		Scale.z *= z;

		UpdateTransform();
	}

	void AddScale(Vector3 S)
	{
		Scale.x *= S.x;
		Scale.y *= S.y;
		Scale.z *= S.z;

		UpdateTransform();
	}

	void UpdateTransform();

	std::vector<class TransformComponent*> Children;
	class TransformComponent* Parent;

	Vector3 PrevWorldPos;
	Vector3 WorldPosition;

	Vector3 LocalPosition;
	Vector3 PrevLocalPosition;

	Vector3 Rotation;
	Vector3 Scale;

	Transform Trans;
};

class MeshComponent : public TransformComponent
{
public:

	MeshComponent();

	virtual bool InitModel(const char* FileName) { return true; }
	virtual void Render(bool bShadowPass) = 0;

	class Mesh* RenderMesh;
	bool bCastsShadow;
};

class StaticMeshComponent : public MeshComponent
{
public:

	virtual void Init() override {}
	virtual bool InitModel(const char* FileName) override;
	virtual void Destroy() override;
	virtual void Render(bool bShadowPass) override;
};

class SkeletalMeshComponent : public MeshComponent
{
public:

	virtual void Init() override {}
	virtual bool InitModel(const char* FileName) override;
	virtual void Update(float DeltaTime) override;
	virtual void Destroy() override;
	virtual void Render(bool bShadowPass) override;

	std::vector<Matrix4> BoneTransforms;
	float AnimTime;
};

class CameraComponent : public TransformComponent
{
public:

	virtual void Init() override;
	virtual void Update(float DeltaTime) override;
	virtual void Destroy() override;

	class Camera* Cam;
};

class AudioComponent : public TransformComponent
{
public:

	virtual void Init() override {}

	// Type: 0 - wave, 1 - stream
	void InitSound(uint8 Type, const char* FileName);
	void FireSound(bool bIs3D, bool bIsLooping = false, float Panning = 0.0f, float Volume = 1.0f, float MaxDistance = 50.0f);
	virtual void Update(float DeltaTime) override;
	virtual void Destroy() override;

	class SoundBase* Sound;
};

class CollisionComponent : public TransformComponent
{
public:

	CollisionComponent() {}

	virtual void Init() override;
	virtual void Update(float DeltaTime) override;
	virtual void Destroy() override;

	btCollisionShape* CreateCollisionShape();
	void SetCollisionFlags(int32 Flags);

	btCollisionShape *Shape;
	btCollisionObject *CollisionObject;

	// for capsule and sphere shapes
	float Radius;
	float Height;

	AABB BoundingBox; // for box shape
	enum ECollisionShape CollisionShape;
};

class LightComponent : public TransformComponent
{
public:

	virtual void Init() override;
	virtual void Update(float DeltaTime) override;

	void UpdatePositionAndRotation();

	uint8 Type;
	SpotLight Spot;
	PointLight Point;
};