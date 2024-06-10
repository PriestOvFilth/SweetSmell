#include "component.h"
#include "engine/mesh.h"
#include "engine/camera.h"
#include "engine/render.h"
#include "scene.h"
#include "entity.h"
#include "engine/sound.h"
#include "engine/debug.h"
#include "physics.h"
#include "engine/resource.h"

void TransformComponent::Init()
{
	SetScale(1.f, 1.f, 1.f);
	SetRotation(0.f, 0.f, 0.f);
	SetPosition(0.f, 0.f, 0.f);
}

void TransformComponent::Update(float DeltaTime)
{
	for (uint32 i = 0; i < Children.size(); i++)
	{
		Children[i]->Update(DeltaTime);
	}
}

void TransformComponent::AttachChild(TransformComponent* Comp)
{
	Comp->Parent = this;
	Children.push_back(Comp);
}

void TransformComponent::Destroy()
{
	for (uint32 i = 0; i < Children.size(); i++)
	{
		if (Children[i])
		{
			Children[i]->Destroy();
		}
	}

	Children.clear();
}

void TransformComponent::UpdateTransform()
{
	// sync the position, rotation, and scale with component transform
	Trans.SetWorldPos(WorldPosition);
	Trans.SetRotation(Rotation);
	Trans.SetScale(Scale);

	// now calculate the world matrix here instead of in GetWorldTrans()
	// so we can multiply the parent matrix with ours if we are a child
	Matrix4 ScaleM, RotationM, TranslationM;
	ScaleM.InitScaleTransform(Scale.x, Scale.y, Scale.z);
	RotationM.InitRotateTransform(Rotation.x, Rotation.y, Rotation.z);
	TranslationM.InitTranslationTransform(WorldPosition.x, WorldPosition.y, WorldPosition.z);

	Matrix4 CompTrans = TranslationM * RotationM * ScaleM;

	if (Parent)
	{
		Matrix4 FinalTrans = Parent->Trans.GetWorldTrans() * CompTrans;
		Trans.WorldTrans = FinalTrans;
	}
	else
	{
		Trans.WorldTrans = CompTrans;
	}

	for (uint32 i = 0; i < Children.size(); i++)
	{
		Children[i]->UpdateTransform();
	}
}

MeshComponent::MeshComponent()
{
	SetScale(1.f, 1.f, 1.f);
	SetRotation(0.f, 0.f, 0.f);
	SetPosition(0.f, 0.f, 0.f);

	bCastsShadow = false;
}

bool StaticMeshComponent::InitModel(const char* FileName)
{
	RenderMesh = GResourceMan->GetStaticMesh(FileName);
	if (!RenderMesh->bIsLoaded)
	{
		return false;
	}

	return true;
}

void StaticMeshComponent::Render(bool bShadowPass)
{
	GRenderer->RenderMesh(bShadowPass, this);

	for (uint32 i = 0; i < Children.size(); i++)
	{
		MeshComponent* Comp = dynamic_cast<class MeshComponent*>(Children[i]);
		if (Comp)
		{
			Comp->Render(bShadowPass);
		}
	}
}

void StaticMeshComponent::Destroy()
{
	TransformComponent::Destroy();
}

bool SkeletalMeshComponent::InitModel(const char* FileName)
{
	RenderMesh = GResourceMan->GetSkeletalMesh(FileName);
	if (!RenderMesh->bIsLoaded)
	{
		return false;
	}

	return true;
}

void SkeletalMeshComponent::Update(float DeltaTime)
{
	AnimTime += DeltaTime * 0.001f;

	TransformComponent::Update(DeltaTime);
}

void SkeletalMeshComponent::Render(bool bShadowPass)
{
	SkeletalMesh* Sk = dynamic_cast<SkeletalMesh*>(RenderMesh);
	Sk->BoneTransform(AnimTime, BoneTransforms);
	GRenderer->RenderMesh(bShadowPass, this);

	for (uint32 i = 0; i < Children.size(); i++)
	{
		MeshComponent* Comp = dynamic_cast<class MeshComponent*>(Children[i]);
		if (Comp)
		{
			Comp->Render(bShadowPass);
		}
	}
}

void SkeletalMeshComponent::Destroy()
{
	TransformComponent::Destroy();
}

void CameraComponent::Init()
{
	Cam = new Camera(GRenderer->GameWindow, GRenderer->WindowWidth, GRenderer->WindowHeight);
	Cam->Pos = Vector3(0.f, 0.f, 0.f);
	Cam->Target = Vector3(-1.f, 0.f, 0.f);
	Cam->Up = Vector3(0.f, 1.f, 0.f);
	Cam->Init(GRenderer->GameWindow);

	SetPosition(Cam->Pos);
	SetRotation(Cam->Target);
}

void CameraComponent::Update(float DeltaTime)
{
	TransformComponent::Update(DeltaTime);

	// if we're the child of another component set the camera's position and target to our relative position and rotation.
	// otherwise our postion and rotation is the camera's position and target.
	if (Parent)
	{
		Cam->Pos = WorldPosition;
		Cam->Target = Rotation;
	}
	else
	{
		SetPosition(Cam->Pos);
		SetRotation(Cam->Target);
	}

	Cam->Update();
}

void CameraComponent::Destroy()
{
	SAFE_DELETE(Cam);
	TransformComponent::Destroy();
}

void AudioComponent::InitSound(uint8 Type, const char* FileName)
{
	switch (Type)
	{
		case 0:
		{
			SoundWave* SndWav = GResourceMan->GetSoundWave(FileName);
			if (SndWav)
			{
				Sound = SndWav;
			}
		} break;
		case 1:
		{
			SoundStream* SndStrm = new SoundStream();
			if (SndStrm)
			{
				SndStrm->Filename = FileName;
				Sound = SndStrm;
			}
		} break;
	}
}

void AudioComponent::FireSound(bool bIs3D, bool bIsLooping, float Panning, float Volume, float MaxDistance)
{
	if (Sound)
	{
		Sound->bIs3D = bIs3D;
		Sound->bLooping = bIsLooping;
		Sound->Panning = Panning;
		Sound->Volume = Volume;
		Sound->MaxDistance = MaxDistance;
		Sound->Position = WorldPosition;
		GAudio->PlaySound(Sound);
	}
}

void AudioComponent::Update(float DeltaTime)
{
	TransformComponent::Update(DeltaTime);

	if (Sound)
	{
		Sound->Position = LocalPosition;
		Sound->Velocity = PrevLocalPosition - LocalPosition;
	}
}

void AudioComponent::Destroy()
{
	TransformComponent::Destroy();
}

void CollisionComponent::Init()
{
	btMatrix3x3 BasisA;
	BasisA.setIdentity();

	CollisionObject = new btCollisionObject();
	CollisionObject->getWorldTransform().setBasis(BasisA);
	CollisionObject->getWorldTransform().setOrigin(btVector3(WorldPosition.x, WorldPosition.y, WorldPosition.z));

	Shape = CreateCollisionShape();
	if (Shape)
	{
		CollisionObject->setCollisionShape(Shape);
	}

	GScene->Physics->TrackComponent(this);
}

btCollisionShape* CollisionComponent::CreateCollisionShape()
{
	btCollisionShape* NewShape = NULL;

	// TODO: add the other shapes
	switch (CollisionShape)
	{
		case CS_Capsule:
		{
			NewShape = new btCapsuleShape(Radius, Height);
		} break;
		case CS_Plane:
		{
			NewShape = new btBoxShape(btVector3(BoundingBox.Max.x / 2.0f, 0.05f, BoundingBox.Max.z / 2.0f));
		} break;
		case CS_Box:
		{
			NewShape = new btBoxShape(btVector3(BoundingBox.Max.x / 2.0f, BoundingBox.Max.y / 2.0f, BoundingBox.Max.z / 2.0f));
		} break;
		case CS_Sphere:
		{
			NewShape = new btSphereShape(Radius);
		} break;
	}

	return NewShape;
}

void CollisionComponent::SetCollisionFlags(int32 Flags)
{
	CollisionObject->setCollisionFlags(CollisionObject->getCollisionFlags() | Flags);
}

void CollisionComponent::Update(float DeltaTime)
{
	CollisionManager* Collision = GScene->Physics->Collision;
	Collision->SetComponentPosition(this, WorldPosition);

	btVector3 btPos = CollisionObject->getWorldTransform().getOrigin();
	Vector3 Pos = Vector3(btPos.getX(), btPos.getY(), btPos.getZ());

	AABB Box = BoundingBox.Transform(Trans.GetWorldTrans());
	//GDebug->AddBox(Pos, Vector3(0.0f, 0.5f, 1.0f), BoundingBox.GetExtents().x, BoundingBox.GetExtents().y, BoundingBox.GetExtents().z);
	GDebug->AddAABB(Box.Min, Box.Max, Vector3(0.0f, 0.5f, 1.0f));

	TransformComponent::Update(DeltaTime);
}

void CollisionComponent::Destroy()
{
	delete Shape;
	delete CollisionObject;
}

void LightComponent::Init()
{
	UpdatePositionAndRotation();
}

void LightComponent::Update(float DeltaTime)
{
	TransformComponent::Update(DeltaTime);

	UpdatePositionAndRotation();
}

void LightComponent::UpdatePositionAndRotation()
{
	LightEntity* Ent = dynamic_cast<LightEntity*>(Owner);
	if (Ent)
	{
		if (Type == LT_Point)
		{
			Point.Position = WorldPosition;

			GScene->SceneLights.PointLights[Ent->Index - 1] = Point;
		}
		else if (Type == LT_Spot)
		{
			Spot.Position = WorldPosition;

			// FIXME: if the rotation is in degrees, it needs to be normalized
			// otherwise it will make the scene ULTRA BRIGHT
			Spot.Direction = Rotation;

			GScene->SceneLights.SpotLights[Ent->Index - 1] = Spot;
		}
	}
}