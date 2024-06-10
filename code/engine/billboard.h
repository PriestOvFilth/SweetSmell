#pragma once

#include <GL/glew.h>
#include "common/math3d.h"
#include "common/common.h"
#include "shader.h"

class Texture;

class BillboardShader : public Shader
{
public:

	virtual bool Init() override;

	void SetVP(const Matrix4& VP);
	void SetCameraPos(const Vector3& Pos);
	void SetColorTextureUnit(uint32 TextureUnit);
	void SetBillboardSize(float Size);

	GLuint VPLoc;
	GLuint CamPosLoc;
	GLuint ColorMapLoc;
	GLuint BillboardSizeLoc;
};

class Billboard
{
public:

	Billboard();
	~Billboard();

	bool Init(const char* FileName);

	void Render(const Matrix4& VP, const Vector3 CamPos);

	void CreatePositionBuffer();

	GLuint VB;
	Texture* Tex;
	BillboardShader* Shader;
};