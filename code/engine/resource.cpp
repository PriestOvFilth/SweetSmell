#include "resource.h"
#include "mesh.h"
#include "texture.h"
#include "sound.h"

using namespace std;

ResourceManager::ResourceManager()
{
	Clear();
}

ResourceManager::~ResourceManager()
{
	Clear();
}

void ResourceManager::Clear()
{
	for (auto It = StaticMeshPool.begin(); It != StaticMeshPool.end(); ++It)
	{
		delete It->second;
	}

	StaticMeshPool.clear();

	for (auto It = SkeletalMeshPool.begin(); It != SkeletalMeshPool.end(); ++It)
	{
		delete It->second;
	}

	SkeletalMeshPool.clear();

	for (auto It = TexturePool.begin(); It != TexturePool.end(); ++It)
	{
		delete It->second;
	}

	TexturePool.clear();

	for (auto It = SoundWavePool.begin(); It != SoundWavePool.end(); ++It)
	{
		delete It->second;
	}

	SoundWavePool.clear();

	TotalResources = 0;
	ResourcesReused = 0;
}

StaticMesh* ResourceManager::GetStaticMesh(const char* FileName)
{
	string Path = FileName;

	if (StaticMeshPool.count(Path) > 0)
	{
		WriteLog("Static mesh %s already loaded, returning reference\n", Path.c_str());
		ResourcesReused++;
		return StaticMeshPool.at(Path);
	}

	WriteLog("Loading static mesh %s\n", Path.c_str());
	StaticMesh* Mesh = new StaticMesh();
	Mesh->LoadModel(Path.c_str());
	StaticMeshPool.insert(std::pair<string, StaticMesh*>(Path, Mesh));
	TotalResources++;
	return Mesh;
}

SkeletalMesh* ResourceManager::GetSkeletalMesh(const char* FileName)
{
	string Path = FileName;

	if (SkeletalMeshPool.count(Path) > 0)
	{
		WriteLog("Skeletal mesh %s already loaded, returning reference\n", Path.c_str());
		ResourcesReused++;
		return SkeletalMeshPool.at(Path);
	}

	WriteLog("Loading skeletal mesh %s\n", Path.c_str());
	SkeletalMesh* Mesh = new SkeletalMesh();
	Mesh->LoadModel(Path.c_str());
	SkeletalMeshPool.insert(std::pair<string, SkeletalMesh*>(Path, Mesh));
	TotalResources++;
	return Mesh;
}

Texture* ResourceManager::GetTexture(const char* FileName)
{
	string Path = FileName;

	if (TexturePool.count(Path) > 0)
	{
		WriteLog("Texture %s already loaded, returning reference\n", Path.c_str());
		ResourcesReused++;
		return TexturePool.at(Path);
	}

	WriteLog("Loading texture %s\n", Path.c_str());
	Texture* Tex = new Texture(GL_TEXTURE_2D, Path.c_str());
	Tex->Init();
	TexturePool.insert(std::pair<string, Texture*>(Path, Tex));
	TotalResources++;
	return Tex;
}

SoundWave* ResourceManager::GetSoundWave(const char* FileName)
{
	string Path = FileName;

	if (SoundWavePool.count(Path) > 0)
	{
		WriteLog("Sound wave %s already loaded, returning reference\n", Path.c_str());
		ResourcesReused++;
		return SoundWavePool.at(Path);
	}

	WriteLog("Loading sound wave %s\n", Path.c_str());
	SoundWave* Wave = new SoundWave();
	Wave->Preload(Path.c_str());
	SoundWavePool.insert(std::pair<string, SoundWave*>(Path, Wave));
	TotalResources++;
	return Wave;
}

void ResourceManager::DeleteResource(const char* FileName, uint8 Type)
{
	string Path = FileName;

	switch (Type)
	{
		case RT_StaticMesh:
		{
			auto It = StaticMeshPool.find(Path);
			if (It != StaticMeshPool.end())
			{
				delete It->second;
				StaticMeshPool.erase(It);
				TotalResources--;
			}
		} break;
		case RT_SkeletalMesh:
		{
			auto It = SkeletalMeshPool.find(Path);
			if (It != SkeletalMeshPool.end())
			{
				delete It->second;
				SkeletalMeshPool.erase(It);
				TotalResources--;
			}
		} break;
		case RT_Texture:
		{
			auto It = TexturePool.find(Path);
			if (It != TexturePool.end())
			{
				delete It->second;
				TexturePool.erase(It);
				TotalResources--;
			}
		} break;
		case RT_SoundWave:
		{
			auto It = SoundWavePool.find(Path);
			if (It != SoundWavePool.end())
			{
				delete It->second;
				SoundWavePool.erase(It);
				TotalResources--;
			}
		} break;
	}
}