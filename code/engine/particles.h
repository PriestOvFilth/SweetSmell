#pragma once

#include <GL/glew.h>
#include "common/common.h"
#include "common/math3d.h"
#include "shader.h"

class ParticleShader;
class BillboardShader;
class RandomTexture;
class Texture;

class ParticleShader : public Shader
{
public:

	virtual bool Init();

	void SetDeltaTime(float DeltaTime);
	void SetTime(float Time);
	void SetRandomTextureUnit(uint32 TextureUnit);
	void SetLauncherLifetime(float Lifetime);
	void SetShellLifetime(float Lifetime);
	void SetSecondaryShellLifetime(float Lifetime);

	GLuint DeltaTimeLoc;
	GLuint RandomTexLoc;
	GLuint TimeLoc;
	GLuint LauncherLifetimeLoc;
	GLuint ShellLifetimeLoc;
	GLuint SecondaryShellLifetimeLoc;
};

class ParticleSystem
{
public:

	ParticleSystem();
	~ParticleSystem();

	bool InitParticleSystem(const Vector3 Pos);
	void Render(float DeltaTime, const Matrix4 VP, const Vector3 CameraPos);

	void UpdateParticles(float DeltaTime);
	void RenderParticles(const Matrix4 VP, const Vector3 CameraPos);

	bool bIsFirst;
	uint32 CurrentVB;
	uint32 CurrentTFB;
	GLuint ParticleBuffer[2];
	GLuint TransformFeedback[2];
	ParticleShader* PShader;
	BillboardShader* BBShader;
	RandomTexture* RandTex;
	Texture* Tex;
	float Time;
};