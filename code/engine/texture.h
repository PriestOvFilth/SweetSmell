#pragma once

#include <string>
#include <GL/glew.h>
#include "common/math3d.h"
#include "common/common.h"

class Image
{
public:

	Image(const char *Name)
	{
		FileName = Name;
	}

	unsigned char* Load(void);
	void Free(void);

	const char *FileName;
	unsigned char *ImageData;
	int32 ImageWidth;
	int32 ImageHeight;
};

class Cubemap
{
public:

	Cubemap(const std::string& Directory, const std::string& PosXFileName, const std::string& NegXFileName, const std::string& PosYFileName, const std::string& NegYFileName, const std::string& PosZFileName, const std::string& NegZFileName);

	~Cubemap();

	bool Init();
	void Bind(GLenum TextureUnit);

	std::string FileNames[6];
	GLuint TextureObj;
};

class Texture
{
public:

	Texture(GLenum Target, const char* FileName)
	{
		TextureTarget = Target;
		Filename = FileName;
		bIsLoaded = false;
	}

	~Texture()
	{
		Unload();
	}

	void Init(void);

	void Unload(void);

	void Bind(GLenum TextureUnit);

	const char* Filename;
	class Image* Img;
	GLenum TextureTarget;
	GLuint TextureObj;
	bool bIsLoaded;
};

class RandomTexture
{
public:

	RandomTexture();
	~RandomTexture();

	bool Init(uint32 Size);
	void Bind(GLenum TextureUnit);

	GLuint TexObj;
};