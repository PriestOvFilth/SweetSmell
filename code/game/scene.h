#pragma once

#include <vector>
#include <string>
#include "common/math3d.h"
#include "common/common.h"
#include "engine/light.h"

class Entity;
class StaticMeshComponent;
class SkeletalMeshComponent;
class MeshComponent;
class CameraComponent;
class PhysicsEngine;
class Level;
class PlayerEntity;
class EditorPlayerEntity;
class LightComponent;

struct LightInfo
{
	std::vector<struct SpotLight> SpotLights;
	std::vector<struct PointLight> PointLights;
	DirectionalLight Ambient;
};

class Scene
{
public:

	Scene();
	~Scene();

	bool Init();

	void Update(float DeltaTime);

	void Render(bool bShadowPass);

	void Destroy();

	void AddEntity(class Entity* Ent);
	void DeleteEntity(std::string Name);
	class Entity* GetEntityByName(std::string Name);

	bool LoadNewLevel(const char* FileName);
	
	void AddLight(LightComponent* Comp);
	void AddPointLight(Vector3 Color, float AmbInt, float DiffInt, Vector3 Pos, float ConstAtten, float LinAtten, float ExpAtten);
	void AddSpotLight(Vector3 Color, float AmbInt, float DiffInt, Vector3 Pos, float ConstAtten, float LinAtten, float ExpAtten, Vector3 Direction, float Cutoff);
	void EnableAmbientLight(bool bEnable, Vector3 Color, float Intensity);

	class CameraComponent* GetPlayerCamera();

	std::vector<class Entity*> Entities;

	// a list of all mesh components in the scene.
	// used for rendering because most of the time
	// we can't rely on an entity's root component
	// being a mesh component.
	std::vector<class MeshComponent*> MeshComponents;

	LightInfo SceneLights;

	class PhysicsEngine* Physics;

	class Level* CurrentLevel;
	std::string StartupLevel;

	class PlayerEntity* Player;
	class EditorPlayerEntity* EditorPlayer;
};