#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

unsigned char* Image::Load(void)
{
	int X, Y, N;
	stbi_uc* Data = stbi_load(FileName, &X, &Y, &N, STBI_rgb_alpha);
	if (!Data)
	{
		WriteLog("Unable to load image: %s\n", FileName);
		return false;
	}

	ImageWidth = X;
	ImageHeight = Y;

	ImageData = Data;

	return ImageData;
}

void Image::Free(void)
{
	if (ImageData != NULL)
	{
		stbi_image_free(ImageData);
	}
}

using namespace std;

static const GLenum Types[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

Cubemap::Cubemap(const string& Directory, const string& PosXFileName, const string& NegXFileName, const string& PosYFileName, const string& NegYFileName, const string& PosZFileName, const string& NegZFileName)
{
	FileNames[0] = Directory + PosXFileName;
	FileNames[1] = Directory + NegXFileName;
	FileNames[2] = Directory + PosYFileName;
	FileNames[3] = Directory + NegYFileName;
	FileNames[4] = Directory + PosZFileName;
	FileNames[5] = Directory + NegZFileName;

	TextureObj = 0;
}

Cubemap::~Cubemap()
{
	if (TextureObj != 0)
	{
		glDeleteTextures(1, &TextureObj);
	}
}

bool Cubemap::Init()
{
	glGenTextures(1, &TextureObj);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureObj);

	Image* Img = NULL;

	for (uint32 i = 0; i < ARRAY_SIZE_IN_ELEMENTS(Types); i++)
	{
		Img = new Image(FileNames[i].c_str());
		unsigned char* Data = Img->Load();

		if (Data == NULL)
		{
			delete Img;
			return false;
		}

		glTexImage2D(Types[i], 0, GL_RGB, Img->ImageWidth, Img->ImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		delete Img;
	}

	return true;
}

void Cubemap::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureObj);
}

void Texture::Init()
{
	Img = new Image(Filename);
	unsigned char* Data = Img->Load();

	if (Data == NULL)
	{
		delete Img;
		bIsLoaded = false;
		return;
	}

	int32 Width = Img->ImageWidth;
	int32 Height = Img->ImageHeight;

	glGenTextures(1, &TextureObj);
	glBindTexture(TextureTarget, TextureObj);
	glTexImage2D(TextureTarget, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
	glTexParameterf(TextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(TextureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	bIsLoaded = true;
}

void Texture::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(TextureTarget, TextureObj);
}

void Texture::Unload(void)
{
	Img->Free();
}

RandomTexture::RandomTexture()
{
	TexObj = 0;
}

RandomTexture::~RandomTexture()
{
	if (TexObj != 0)
	{
		glDeleteTextures(1, &TexObj);
	}
}

bool RandomTexture::Init(uint32 Size)
{
	Vector3* RandomData = new Vector3[Size];
	
	for (uint32 i = 0; i < Size; i++)
	{
		RandomData[i].x = RandomFloat();
		RandomData[i].y = RandomFloat();
		RandomData[i].z = RandomFloat();
	}

	glGenTextures(1, &TexObj);
	glBindTexture(GL_TEXTURE_1D, TexObj);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, Size, 0, GL_RGB, GL_FLOAT, RandomData);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	delete[] RandomData;

	return GLCheckError();
}

void RandomTexture::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_1D, TexObj);
}