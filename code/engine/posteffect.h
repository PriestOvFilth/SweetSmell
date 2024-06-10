#pragma once

#include <vector>
#include "common/common.h"
#include "common/math3d.h"
#include "shader.h"

class PostEffectShader : public Shader
{
public:

	PostEffectShader() {}
	
	virtual bool Init() override;

	void SetTextureUnit(uint32 TextureUnit);

	GLuint TexLoc;

	char* FragShaderFile;
	char* VertShaderFile;
};

class PixelizeShader : public PostEffectShader
{
public:

	PixelizeShader();

	virtual bool Init() override;

	void SetViewportSize(Vector2 ViewportSize);
	void SetPixelSize(float PixelSize);

	GLuint ViewportSizeLoc;
	GLuint PixelSizeLoc;
};

class DitherShader : public PostEffectShader
{
public:

	DitherShader();
};

class LoadingScreenShader : public PostEffectShader
{
public:

	LoadingScreenShader();
};

class FullscreenQuad
{
public:

	FullscreenQuad();

	bool Init();
	void Draw(uint32 FBTex);

	std::vector<Vector2> Points;
	std::vector<Vector2> TexCoords;

	uint32 VertexArraySize;
	uint32 TexArraySize;

	uint32 BO;
	uint32 VAO;
};