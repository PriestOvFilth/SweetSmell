#pragma once

#include <vector>
#include "common/common.h"
#include "common/math3d.h"
#include "shader.h"

#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS 8
#define MAX_BONES 100

struct BaseLight
{
	Vector3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;

	BaseLight()
	{
		Color = Vector3(0.0f, 0.0f, 0.0f);
		AmbientIntensity = 0.0f;
		DiffuseIntensity = 0.0f;
	}
};

struct DirectionalLight : public BaseLight
{
	Vector3 Direction;

	DirectionalLight()
	{
		Direction = Vector3(0.0f, 0.0f, 0.0f);
	}
};

struct PointLight : BaseLight
{
	Vector3 Position;

	struct
	{
		float Constant;
		float Linear;
		float Exp;
	} Attenuation;

	PointLight()
	{
		Position = Vector3(0.0f, 0.0f, 0.0f);
		Attenuation.Constant = 1.0f;
		Attenuation.Linear = 0.0f;
		Attenuation.Exp = 0.0f;
	}
};

struct SpotLight : public PointLight
{
	Vector3 Direction;
	float Cutoff;

	SpotLight()
	{
		Direction = Vector3(0.0f, 0.0f, 0.0f);
		Cutoff = 0.0f;
	}
};

class LightShader : public Shader
{
public:

	LightShader() {}

	virtual bool Init(bool Skinning);

	void SetWVP(const Matrix4& WVP);
	void SetLightWVP(const Matrix4& LightWVP);
	void SetWorldMatrix(const Matrix4& WorldInverse);
	void SetTextureUnit(uint32 TextureUnit);
	void SetShadowMapTextureUnit(uint32 TextureUnit);
	void SetEyeWorldPos(const Vector3& EyeWorldPos);
	void SetFog(uint8 bEnableFog);
	void SetMinFogDist(float MinFogDist);
	void SetFogColor(Vector3 FogColor);
	void SetDirectionalLight(const DirectionalLight& Light);
	void SetPointLights(std::vector<struct PointLight> Lights);
	void SetSpotLights(std::vector<struct SpotLight> Lights);
	void SetBoneTransform(uint32 Index, const Matrix4& Transform);

	bool bSkinning;

	GLuint WVPLoc;
	GLuint LightWVPLoc;
	GLuint WorldMatrixLoc;
	GLuint SamplerLoc;
	GLuint ShadowMapLoc;
	GLuint EyeWorldPosLoc;
	GLuint EnableFogLoc;
	GLuint MinFogDistLoc;
	GLuint FogColorLoc;
	GLuint NumPointLightsLoc;
	GLuint NumSpotLightsLoc;
	
	GLuint BoneLoc[MAX_BONES];

	struct
	{
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint Direction;
		GLuint DiffuseIntensity;
	} DirLightLoc;

	struct
	{
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint DiffuseIntensity;
		GLuint Position;
		
		struct
		{
			GLuint Constant;
			GLuint Linear;
			GLuint Exp;
		} Atten;
	} PointLightsLoc[MAX_POINT_LIGHTS];

	struct
	{
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint DiffuseIntensity;
		GLuint Position;
		GLuint Direction;
		GLuint Cutoff;

		struct
		{
			GLuint Constant;
			GLuint Linear;
			GLuint Exp;
		} Atten;
	} SpotLightsLoc[MAX_SPOT_LIGHTS];
};