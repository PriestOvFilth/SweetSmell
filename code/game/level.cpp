#include "level.h"
#include "entity.h"
#include "component.h"
#include "player.h"
#include "scene.h"
#include "engine/render.h"
#include "engine/resource.h"
#include "physics.h"

using namespace std;

Level::Level()
{
	Clear();
}

Level::~Level()
{
	Clear();
}

void Level::Clear()
{
	LevelFile.Clear();
	LevelEntities.clear();
	LevelEntities.resize(0);
}

bool Level::LoadLevel(const char* FilePath)
{
	Clear();
	GResourceMan->Clear();

	FileName = FilePath;
	
	if (LevelFile.LoadFile(FileName) != XML_SUCCESS)
	{
		WriteLog("Unable to parse XML document %s\n", FileName);
		GIsLevelLoading = false;
		return false;
	}

	const char *SkyboxDirectory = NULL, *PosX = NULL, *NegX = NULL, *PosY = NULL, *NegY = NULL, *PosZ = NULL, *NegZ = NULL;
	float PhysicsWorldSize;
	
	XMLElement* Root = LevelFile.FirstChildElement("scene");
	if (Root)
	{
		// fog
		XMLElement* Fog = Root->FirstChildElement("fog");
		if (Fog)
		{
			Fog->QueryBoolAttribute("Enabled", &GRenderer->bEnableFog);
			Fog->QueryFloatAttribute("Distance", &GRenderer->FogDistance);
			Vector3 FogColor = VectorFromString(Fog->Attribute("Color"), ", ");
		}

		// ambient lighting
		XMLElement* Ambient = Root->FirstChildElement("ambient");
		if (Ambient)
		{
			bool bEnabled = Ambient->BoolAttribute("Enabled");
			Vector3 Color = VectorFromString(Ambient->Attribute("Color"), ", ");
			float AmbInt = Ambient->FloatAttribute("AmbInt");

			GScene->EnableAmbientLight(bEnabled, Color, AmbInt);
		}

		// skybox
		XMLElement* Skybox = Root->FirstChildElement("skybox");
		if (Skybox)
		{
			Skybox->QueryBoolAttribute("SkyboxEnabled", &GRenderer->bRenderSky);
			Skybox->QueryStringAttribute("SkyboxDirectory", &SkyboxDirectory);
			Skybox->QueryStringAttribute("SkyboxPosX", &PosX);
			Skybox->QueryStringAttribute("SkyboxNegX", &NegX);
			Skybox->QueryStringAttribute("SkyboxPosY", &PosY);
			Skybox->QueryStringAttribute("SkyboxNegY", &NegY);
			Skybox->QueryStringAttribute("SkyboxPosZ", &PosZ);
			Skybox->QueryStringAttribute("SkyboxNegZ", &NegZ);
		}

		// physics
		XMLElement* Physics = Root->FirstChildElement("physics");
		if (Physics)
		{
			Physics->QueryFloatAttribute("WorldSize", &PhysicsWorldSize);
		}

		GScene->Physics->Init(PhysicsWorldSize);

		ParseEntities(Root);
		ParseLights(Root);
	}
	else
	{
		WriteLog("Error: Missing root element in level file %s\n", FileName);
		GIsLevelLoading = false;
		return false;
	}

#if FILTH_DEBUG || FILTH_EDITOR
	// create an editor player if we're in the editor
	GScene->EditorPlayer = new EditorPlayerEntity();
	GScene->AddEntity(GScene->EditorPlayer);
	if (!GScene->EditorPlayer->Init())
	{
		WriteLog("Error: unable to init editor player\n");
		GIsLevelLoading = false;
		return false;
	}
#endif

	if (GRenderer->bRenderSky)
	{
		if (!GRenderer->Sky->Init(SkyboxDirectory, PosX, NegX, PosY, NegY, PosZ, NegZ))
		{
			WriteLog("Unable to init skybox\n");
			GIsLevelLoading = false;
			return false;
		}
	}
	
	WriteLog("Total resources in memory: %d\n", GResourceMan->TotalResources);
	WriteLog("Resources reused: %d\n", GResourceMan->ResourcesReused);

	return true;
}

bool Level::ParseEntities(XMLElement* Root)
{
	XMLElement* Entities = Root->FirstChildElement("entities");
	if (Entities)
	{
		XMLElement* XMLEntity = Entities->FirstChildElement("entity");
		while (XMLEntity != nullptr)
		{
			const char* ClassName;
			XMLError Err = XMLEntity->QueryStringAttribute("Class", &ClassName);
			if (Err == XML_SUCCESS)
			{
				string ClassString = ClassName;
				Entity* Ent = GetEntity(ClassString);

				Vector3 Position = VectorFromString(XMLEntity->Attribute("Position"), ", ");
				Vector3 Rotation = VectorFromString(XMLEntity->Attribute("Rotation"), ", ");
				Vector3 Scale = VectorFromString(XMLEntity->Attribute("Scale"), ", ");

				Ent->RootComponent->SetPosition(Position);
				Ent->RootComponent->SetRotation(Rotation);
				Ent->RootComponent->SetScale(Scale);

				Ent->Name = XMLEntity->Attribute("Name");

				GScene->AddEntity(Ent);

				const char* EntityDefFile = XMLEntity->Attribute("EntityDef");
				if (EntityDefFile != NULL)
				{
					Ent->LoadEntityDef(EntityDefFile);
				}

				bool bIsVisible = XMLEntity->BoolAttribute("IsVisible");
				Ent->bVisible = bIsVisible;

				if (!Ent->Init())
				{
					WriteLog("Unable to init entity %s\n", Ent->Name.c_str());
					GIsLevelLoading = false;
					return false;
				}

				LevelEntities.push_back(Ent);
			}

			XMLEntity = XMLEntity->NextSiblingElement("entity");
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool Level::ParseLights(XMLElement* Root)
{
	XMLElement* Lights = Root->FirstChildElement("lights");
	if (Lights)
	{
		XMLElement* Light = Lights->FirstChildElement("light");
		while (Light != nullptr)
		{
			const char* TypeName;
			XMLError Err = Light->QueryStringAttribute("Type", &TypeName);
			if (Err == XML_SUCCESS)
			{
				string TypeString = TypeName;
				XMLElement* Properties = Light->FirstChildElement("properties");
				if (TypeString == "Spot")
				{
					if (Properties)
					{
						Vector3 Color = VectorFromString(Properties->Attribute("Color"), ", ");
						float AmbInt = Properties->FloatAttribute("AmbInt");
						float DiffInt = Properties->FloatAttribute("DiffInt");
						Vector3 Position = VectorFromString(Properties->Attribute("Position"), ", ");
						float ConstAtten = Properties->FloatAttribute("ConstAtten");
						float LinAtten = Properties->FloatAttribute("LinAtten");
						float ExpAtten = Properties->FloatAttribute("ExpAtten");
						Vector3 Direction = VectorFromString(Properties->Attribute("Direction"), ", ");
						float Cutoff = Properties->FloatAttribute("Cutoff");

						LightEntity* Light = new LightEntity();
						
						Light->LightComp->Type = LT_Spot;
						Light->LightComp->Spot.Color = Color;
						Light->LightComp->Spot.AmbientIntensity = AmbInt;
						Light->LightComp->Spot.DiffuseIntensity = DiffInt;
						Light->LightComp->Spot.Attenuation.Constant = ConstAtten;
						Light->LightComp->Spot.Attenuation.Linear = LinAtten;
						Light->LightComp->Spot.Attenuation.Exp = ExpAtten;
						Light->LightComp->Spot.Cutoff = Cutoff;
						
						Light->LightComp->SetPosition(Position);
						Light->LightComp->SetRotation(Direction);
						
						Light->Init();
						Light->bVisible = true;

						GScene->AddEntity(Light);
					}
				}
				if (TypeString == "Point")
				{
					if (Properties)
					{
						Vector3 Color = VectorFromString(Properties->Attribute("Color"), ", ");
						float AmbInt = Properties->FloatAttribute("AmbInt");
						float DiffInt = Properties->FloatAttribute("DiffInt");
						Vector3 Position = VectorFromString(Properties->Attribute("Position"), ", ");
						float ConstAtten = Properties->FloatAttribute("ConstAtten");
						float LinAtten = Properties->FloatAttribute("LinAtten");
						float ExpAtten = Properties->FloatAttribute("ExpAtten");

						LightEntity* Light = new LightEntity();
						
						Light->LightComp->Type = LT_Point;
						Light->LightComp->Point.Color = Color;
						Light->LightComp->Point.AmbientIntensity = AmbInt;
						Light->LightComp->Point.DiffuseIntensity = DiffInt;
						Light->LightComp->Point.Attenuation.Constant = ConstAtten;
						Light->LightComp->Point.Attenuation.Linear = LinAtten;
						Light->LightComp->Point.Attenuation.Exp = ExpAtten;
						
						Light->LightComp->SetPosition(Position);
						
						Light->Init();
						Light->bVisible = true;

						GScene->AddEntity(Light);
					}
				}
			}

			Light = Light->NextSiblingElement("light");
		}
	}
	else
	{
		return false;
	}

	return true;
}

Entity* Level::GetEntity(std::string ClassString)
{
	Entity* Ent = NULL;

	if (ClassString == "PlayerEntity")
	{
		GScene->Player = new PlayerEntity();
		Ent = GScene->Player;
	}
	if (ClassString == "StaticMeshEntity")
	{
		StaticMeshEntity* StaticMeshEnt = new StaticMeshEntity();
		Ent = StaticMeshEnt;
	}
	if (ClassString == "SkeletalMeshEntity")
	{
		SkeletalMeshEntity* SkeletalMeshEnt = new SkeletalMeshEntity();
		Ent = SkeletalMeshEnt;
	}
	if (ClassString == "SoundEntity")
	{
		SoundEntity* SoundEnt = new SoundEntity();
		Ent = SoundEnt;
	}

	return Ent;
}