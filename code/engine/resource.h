#pragma once

#include <map>
#include <string>
#include "common/common.h"

class StaticMesh;
class SkeletalMesh;
class Texture;
class SoundWave;

enum EResourceTypes
{
	RT_StaticMesh,
	RT_SkeletalMesh,
	RT_Texture,
	RT_SoundWave
};

class ResourceManager
{
public:

	ResourceManager();
	~ResourceManager();

	void Clear();

	class StaticMesh* GetStaticMesh(const char* FileName);
	class SkeletalMesh* GetSkeletalMesh(const char* FileName);
	class Texture* GetTexture(const char* FileName);
	class SoundWave* GetSoundWave(const char* FileName);

	void DeleteResource(const char* FileName, uint8 Type);

	std::map<std::string, StaticMesh*> StaticMeshPool;
	std::map<std::string, SkeletalMesh*> SkeletalMeshPool;
	std::map<std::string, Texture*> TexturePool;
	std::map<std::string, SoundWave*> SoundWavePool;

	uint32 TotalResources;
	uint32 ResourcesReused;
};