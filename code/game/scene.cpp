#include "common/common.h"
#include "engine/render.h"
#include "engine/camera.h"
#include "engine/mesh.h"
#include "scene.h"
#include "entity.h"
#include "component.h"
#include "physics.h"
#include "player.h"
#include "level.h"

using namespace std;

Scene::Scene()
{
	StartupLevel = "data/levels/test.level";
	Physics = new PhysicsEngine(true);
}

Scene::~Scene()
{
	Destroy();

	SAFE_DELETE(Physics);
	SAFE_DELETE(CurrentLevel);
}

bool Scene::Init()
{
	if (!GRenderer->InitLightShader(0) || !GRenderer->InitLightShader(1))
	{
		return false;
	}

	CurrentLevel = new Level();

	return true;
}

void Scene::Update(float DeltaTime)
{
	for (uint32 i = 0; i < Entities.size(); i++)
	{
		Entities[i]->Update(DeltaTime);
	}

	Physics->Update(DeltaTime);

	vector<CollisionComponent*> CollisionComps = Physics->Collision->TrackedComps;
	for (uint32 i = 0; i < CollisionComps.size(); i++)
	{
		Vector3 NewPos;

		if (Physics->Collision->IsColliding(CollisionComps[i]))
		{
			NewPos = CollisionComps[i]->PrevWorldPos;
		}
		else
		{
			NewPos = Physics->Collision->GetComponentPosition(CollisionComps[i]);
		}

		CollisionComps[i]->SetPosition(NewPos);
	}
}

void Scene::Render(bool bShadowPass)
{
	for (uint32 i = 0; i < MeshComponents.size(); i++)
	{
		if (MeshComponents[i]->Owner->bVisible)
		{
			MeshComponents[i]->Render(bShadowPass);
		}
	}
}

void Scene::Destroy()
{
	for (uint32 i = 0; i < Entities.size(); i++)
	{
		Entities[i]->Destroy();
		SAFE_DELETE(Entities[i]);
	}

	Entities.clear();
	Entities.resize(0);

	MeshComponents.clear();
	MeshComponents.resize(0);

	SceneLights.PointLights.clear();
	SceneLights.PointLights.resize(0);
	
	SceneLights.SpotLights.clear();
	SceneLights.SpotLights.resize(0);

	Physics->Destroy();
}

void Scene::AddEntity(class Entity* Ent)
{
	Ent->ParentScene = this;

	Entities.push_back(Ent);
}

void Scene::DeleteEntity(string Name)
{
	// cleanup the entity
	for (uint32 i = 0; i < Entities.size(); i++)
	{
		if (Entities[i])
		{
			Entities[i]->Destroy();
		}
	}

	// remove entity from vector
	for (auto It = Entities.begin(); It != Entities.end();)
	{
		if (*It != NULL)
		{
			It = Entities.erase(It);
		}
		else
		{
			++It;
		}
	}
}

Entity* Scene::GetEntityByName(string Name)
{
	for (uint32 i = 0; i < Entities.size(); i++)
	{
		if (Entities[i]->Name == Name)
		{
			return Entities[i];
		}
	}

	return NULL;
}

bool Scene::LoadNewLevel(const char* FileName)
{
	GIsLevelLoading = true;

	float StartLoadTime = GetDebugPerfCounter();

	GRenderer->DrawLoadingScreen();
	
	Destroy();

	WriteLog("Loading level %s\n", FileName);
	if (!CurrentLevel->LoadLevel(FileName))
	{
		WriteLog("Error: could not load level %s\n", FileName);
		return false;
	}

	float EndLoadTime = GetDebugPerfCounter();
	float LoadTimeSeconds = GetDebugPerfTime(StartLoadTime, EndLoadTime);
	WriteLog("Level load took %.2fs\n", LoadTimeSeconds);

	GIsLevelLoading = false;

	return true;
}

void Scene::AddLight(LightComponent* Comp)
{
	if (Comp)
	{
		LightEntity* Owner = dynamic_cast<LightEntity*>(Comp->Owner);
		if (Owner)
		{
			if (Comp->Type == LT_Point)
			{
				SceneLights.PointLights.push_back(Comp->Point);
				Owner->Index = (int32)SceneLights.PointLights.size();
			}
			else if (Comp->Type == LT_Spot)
			{
				SceneLights.SpotLights.push_back(Comp->Spot);
				Owner->Index = (int32)SceneLights.SpotLights.size();
			}
		}
	}
}

void Scene::AddPointLight(Vector3 Color, float AmbInt, float DiffInt, Vector3 Pos, float ConstAtten, float LinAtten, float ExpAtten)
{
	PointLight P;
	P.Color = Color;
	P.AmbientIntensity = AmbInt;
	P.DiffuseIntensity = DiffInt;
	P.Position = Pos;
	P.Attenuation.Constant = ConstAtten;
	P.Attenuation.Linear = LinAtten;
	P.Attenuation.Exp = ExpAtten;

	SceneLights.PointLights.push_back(P);

	if (SceneLights.PointLights.size() > MAX_POINT_LIGHTS)
	{
		WriteLog("WARNING: too many point lights! MAX_POINT_LIGHTS is %d and the number of point lights in the scene is %d\n", MAX_POINT_LIGHTS, SceneLights.PointLights.size());
	}
}

void Scene::AddSpotLight(Vector3 Color, float AmbInt, float DiffInt, Vector3 Pos, float ConstAtten, float LinAtten, float ExpAtten, Vector3 Direction, float Cutoff)
{
	SpotLight S;
	S.Color = Color;
	S.AmbientIntensity = AmbInt;
	S.DiffuseIntensity = DiffInt;
	S.Position = Pos;
	S.Attenuation.Constant = ConstAtten;
	S.Attenuation.Linear = LinAtten;
	S.Attenuation.Exp = ExpAtten;
	S.Direction = Direction;
	S.Cutoff = Cutoff;
	
	SceneLights.SpotLights.push_back(S);

	if (SceneLights.SpotLights.size() > MAX_SPOT_LIGHTS)
	{
		WriteLog("WARNING: too many spot lights! MAX_POINT_LIGHTS is %d and the number of spot lights in the scene is %d\n", MAX_SPOT_LIGHTS, SceneLights.SpotLights.size());
	}
}

void Scene::EnableAmbientLight(bool bEnable, Vector3 Color, float Intensity)
{
	if (bEnable)
	{
		SceneLights.Ambient.AmbientIntensity = Intensity;
		SceneLights.Ambient.Color = Color;
	}
	else
	{
		SceneLights.Ambient.AmbientIntensity = 0.0f;
		SceneLights.Ambient.Color = Color;
	}
}

CameraComponent* Scene::GetPlayerCamera()
{
	for (uint32 i = 0; i < Entities.size(); i++)
	{
		EditorPlayerEntity* Player = dynamic_cast<class EditorPlayerEntity*>(Entities[i]);
		if (Player)
		{
			return Player->CamComp;
		}
	}

	return NULL;
}