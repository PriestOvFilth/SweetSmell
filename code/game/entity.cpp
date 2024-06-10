#include "entity.h"
#include "component.h"
#include "scene.h"
#include "engine/sound.h"
#include "engine/camera.h"
#include "engine/debug.h"
#include "engine/mesh.h"

using namespace std;

Entity::Entity()
{
	Name = "";
	bVisible = true;
}

void Entity::Update(float DeltaTime)
{
	RootComponent->Update(DeltaTime);

	for (uint32 i = 0; i < OwnedComponents.size(); i++)
	{
		OwnedComponents[i]->Update(DeltaTime);
	}
}

void Entity::Destroy()
{
	for (uint32 i = 0; i < OwnedComponents.size(); i++)
	{
		OwnedComponents[i]->Destroy();
	}

	OwnedComponents.clear();

	EntityDef.Clear();
}

void Entity::AddComponent(class Component* Comp)
{
	Comp->Owner = this;
	OwnedComponents.push_back(Comp);
}

void Entity::AttachComponent(TransformComponent* Comp)
{
	Comp->Owner = this;

	if (!RootComponent)
	{
		RootComponent = Comp;
		
		MeshComponent* Mesh = dynamic_cast<MeshComponent*>(RootComponent);
		if (Mesh)
		{
			GScene->MeshComponents.push_back(Mesh);
		}
	}
	else
	{
		AttachChildToComponent(RootComponent, Comp);
	}
}

bool Entity::LoadEntityDef(const char* Path)
{
	EntityDefPath = Path;

	if (EntityDef.LoadFile(Path) != XML_SUCCESS)
	{
		WriteLog("Unable to parse XML document %s\n", Path);
		EntityDefLoaded = false;
	}
	else
	{
		EntityDefLoaded = true;

		XMLElement* Root = EntityDef.FirstChildElement("entity");
		if (Root)
		{
			Name = Root->Attribute("Name");
		}
	}

	return EntityDefLoaded;
}

string Entity::GetEntityDefStringValue(const char* ElementName, const char* AttributeName)
{
	string Result = "";

	if (EntityDefLoaded)
	{
		XMLElement* Root = EntityDef.FirstChildElement("entity");
		if (Root)
		{
			XMLElement* Element = Root->FirstChildElement(ElementName);
			if (Element)
			{
				Result = Element->Attribute(AttributeName);
			}
			else
			{
				WriteLog("Error: %s does not exist in entity def %s\n", ElementName, EntityDefPath.c_str());
			}
		}
		else
		{
			WriteLog("Error: Missing root element in entity def %s\n", EntityDefPath.c_str());
		}
	}

	return Result;
}

bool Entity::GetEntityDefBoolValue(const char* ElementName, const char* AttributeName)
{
	if (EntityDefLoaded)
	{
		XMLElement* Root = EntityDef.FirstChildElement("entity");
		if (Root)
		{
			XMLElement* Element = Root->FirstChildElement(ElementName);
			if (Element)
			{
				return Element->BoolAttribute(AttributeName);
			}
			else
			{
				WriteLog("Error: %s does not exist in entity def %s\n", ElementName, EntityDefPath.c_str());
				return false;
			}
		}
		else
		{
			WriteLog("Error: Missing root element in entity def %s\n", EntityDefPath.c_str());
			return false;
		}
	}

	return false;
}

int Entity::GetEntityDefIntValue(const char* ElementName, const char* AttributeName)
{
	int Value = 0;

	if (EntityDefLoaded)
	{
		XMLElement* Root = EntityDef.FirstChildElement("entity");
		if (Root)
		{
			XMLElement* Element = Root->FirstChildElement(ElementName);
			if (Element)
			{
				Value = Element->IntAttribute(AttributeName);
			}
			else
			{
				WriteLog("Error: %s does not exist in entity def %s\n", ElementName, EntityDefPath.c_str());
			}
		}
		else
		{
			WriteLog("Error: Missing root element in entity def %s\n", EntityDefPath.c_str());
		}
	}

	return Value;
}

float Entity::GetEntityDefFloatValue(const char* ElementName, const char* AttributeName)
{
	float Value = 0;

	if (EntityDefLoaded)
	{
		XMLElement* Root = EntityDef.FirstChildElement("entity");
		if (Root)
		{
			XMLElement* Element = Root->FirstChildElement(ElementName);
			if (Element)
			{
				Value = Element->FloatAttribute(AttributeName);
			}
			else
			{
				WriteLog("Error: %s does not exist in entity def %s\n", ElementName, EntityDefPath.c_str());
			}
		}
		else
		{
			WriteLog("Error: Missing root element in entity def %s\n", EntityDefPath.c_str());
		}
	}

	return Value;
}

void Entity::AttachChildToComponent(TransformComponent* Parent, TransformComponent* Child)
{
	if (!Parent || !Child)
	{
		return;
	}

	Child->Owner = this;
	Parent->AttachChild(Child);

	MeshComponent* Mesh = dynamic_cast<MeshComponent*>(Child);
	if (Mesh)
	{
		GScene->MeshComponents.push_back(Mesh);
	}
}

StaticMeshEntity::StaticMeshEntity()
{
	Collision = new CollisionComponent();
	AttachComponent(Collision);

	Mesh = new StaticMeshComponent();
	AttachComponent(Mesh);
}

bool StaticMeshEntity::Init()
{
	if (!EntityDefLoaded)
	{
		return false;
	}

	//Collision->BoundingBox = AABB(VectorFromString(GetEntityDefStringValue("collision", "BoundingBox"), ", "));
	
	string ShapeValue = GetEntityDefStringValue("collision", "Shape");
	if (ShapeValue == "Plane")
	{
		Collision->CollisionShape = CS_Plane;
	}
	else if (ShapeValue == "Box")
	{
		Collision->CollisionShape = CS_Box;
	}
	else if (ShapeValue == "Capsule")
	{
		Collision->CollisionShape = CS_Capsule;
	}
	else if (ShapeValue == "Sphere")
	{
		Collision->CollisionShape = CS_Sphere;
	}
	
	Collision->Init();
	Collision->SetCollisionFlags(btCollisionObject::CF_STATIC_OBJECT); // TODO: maybe expose this to xml def?

	string MeshFile = GetEntityDefStringValue("mesh", "File");
	if (!Mesh->InitModel(MeshFile.c_str()))
	{
		return false;
	}

	Collision->BoundingBox = Mesh->RenderMesh->BoundingBox;
	
	Mesh->bCastsShadow = GetEntityDefBoolValue("mesh", "CastsShadow");

	return true;
}

SkeletalMeshEntity::SkeletalMeshEntity()
{
	Collision = new CollisionComponent();
	AttachComponent(Collision);

	Mesh = new SkeletalMeshComponent();
	AttachComponent(Mesh);
}

bool SkeletalMeshEntity::Init()
{
	if (!EntityDefLoaded)
	{
		return false;
	}

	//Collision->BoundingBox = AABB(VectorFromString(GetEntityDefStringValue("collision", "BoundingBox"), ", "));

	string ShapeValue = GetEntityDefStringValue("collision", "Shape");
	if (ShapeValue == "Plane")
	{
		Collision->CollisionShape = CS_Plane;
	}
	else if (ShapeValue == "Box")
	{
		Collision->CollisionShape = CS_Box;
	}
	else if (ShapeValue == "Capsule")
	{
		Collision->CollisionShape = CS_Capsule;
	}
	else if (ShapeValue == "Sphere")
	{
		Collision->CollisionShape = CS_Sphere;
	}

	Collision->Init();
	Collision->SetCollisionFlags(btCollisionObject::CF_STATIC_OBJECT); // TODO: maybe expose this to xml def?

	string MeshFile = GetEntityDefStringValue("mesh", "File");
	if (!Mesh->InitModel(MeshFile.c_str()))
	{
		return false;
	}

	Collision->BoundingBox = Mesh->RenderMesh->BoundingBox;

	Mesh->bCastsShadow = GetEntityDefBoolValue("mesh", "CastsShadow");

	AnimToPlay = GetEntityDefIntValue("mesh", "AnimToPlay");

	return true;
}

SoundEntity::SoundEntity()
{
	Sound = new AudioComponent();
	AttachComponent(Sound);
}

bool SoundEntity::Init()
{
	if (!EntityDefLoaded)
	{
		return false;
	}

	string SoundFile = GetEntityDefStringValue("audio", "File");
	uint8 Type = GetEntityDefIntValue("audio", "Type");
	Sound->InitSound(Type, SoundFile.c_str());

	bool bIs3D = GetEntityDefBoolValue("audio", "Is3D");
	bool bIsLooping = GetEntityDefBoolValue("audio", "IsLooping");
	float Panning = GetEntityDefFloatValue("audio", "Panning");
	float Volume = GetEntityDefFloatValue("audio", "Volume");

	Sound->FireSound(bIs3D, bIsLooping, Panning, Volume);
	
	return true;
}

LightEntity::LightEntity()
{
	LightComp = new LightComponent();
	AttachComponent(LightComp);
}

bool LightEntity::Init()
{
	GScene->AddLight(LightComp);
	return true;
}