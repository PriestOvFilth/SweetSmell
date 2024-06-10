#pragma once

#include <vector>
#include <string>
#include <tinyxml2.h>
#include "common/common.h"
#include "common/math3d.h"

using namespace tinyxml2;

class Entity;

class Level
{
public:

	Level();
	~Level();

	void Clear();

	bool LoadLevel(const char* FilePath);
	bool ParseEntities(class XMLElement* Root);
	bool ParseLights(class XMLElement* Root);

	class Entity* GetEntity(std::string ClassString);

	std::vector<class Entity*> LevelEntities;

	const char* FileName;
	XMLDocument LevelFile;
};