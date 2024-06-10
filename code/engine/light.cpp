#include "light.h"

bool LightShader::Init(bool Skinning)
{
	if (!Shader::Init())
	{
		return false;
	}

	bSkinning = Skinning;

	if (bSkinning)
	{
		if (!AddShader(GL_VERTEX_SHADER, "data/shaders/Skinning.vert"))
		{
			return false;
		}

		if (!AddShader(GL_FRAGMENT_SHADER, "data/shaders/BasicLit.frag"))
		{
			return false;
		}
	}
	else
	{
		if (!AddShader(GL_VERTEX_SHADER, "data/shaders/BasicLit.vert"))
		{
			return false;
		}

		if (!AddShader(GL_FRAGMENT_SHADER, "data/shaders/BasicLit.frag"))
		{
			return false;
		}
	}

	if (!Finalize())
	{
		return false;
	}

	WVPLoc = GetUniformLocation("WVP");
	LightWVPLoc = GetUniformLocation("LightWVP");
	WorldMatrixLoc = GetUniformLocation("World");
	SamplerLoc = GetUniformLocation("Sampler");
	ShadowMapLoc = GetUniformLocation("ShadowMap");
	EyeWorldPosLoc = GetUniformLocation("EyeWorldPos");
	EnableFogLoc = GetUniformLocation("bEnableFog");
	MinFogDistLoc = GetUniformLocation("MinFogDist");
	FogColorLoc = GetUniformLocation("FogColor");
	DirLightLoc.Color = GetUniformLocation("DirLight.Base.Color");
	DirLightLoc.AmbientIntensity = GetUniformLocation("DirLight.Base.AmbientIntensity");
	DirLightLoc.Direction = GetUniformLocation("DirLight.Direction");
	DirLightLoc.DiffuseIntensity = GetUniformLocation("DirLight.Base.DiffuseIntensity");
	NumPointLightsLoc = GetUniformLocation("NumPointLights");
	NumSpotLightsLoc = GetUniformLocation("NumSpotLights");

	for (uint32 i = 0; i < ARRAY_SIZE_IN_ELEMENTS(PointLightsLoc); i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));
		
		snprintf(Name, sizeof(Name), "PointLights[%d].Base.Color", i);
		PointLightsLoc[i].Color = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "PointLights[%d].Base.AmbientIntensity", i);
		PointLightsLoc[i].AmbientIntensity = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "PointLights[%d].Position", i);
		PointLightsLoc[i].Position = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "PointLights[%d].Base.DiffuseIntensity", i);
		PointLightsLoc[i].DiffuseIntensity = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "PointLights[%d].Atten.Constant", i);
		PointLightsLoc[i].Atten.Constant = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "PointLights[%d].Atten.Linear", i);
		PointLightsLoc[i].Atten.Linear = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "PointLights[%d].Atten.Exp", i);
		PointLightsLoc[i].Atten.Exp = GetUniformLocation(Name);

		if (PointLightsLoc[i].Color == INVALID ||
			PointLightsLoc[i].AmbientIntensity == INVALID ||
			PointLightsLoc[i].Position == INVALID ||
			PointLightsLoc[i].DiffuseIntensity == INVALID ||
			PointLightsLoc[i].Atten.Constant == INVALID ||
			PointLightsLoc[i].Atten.Linear == INVALID ||
			PointLightsLoc[i].Atten.Exp == INVALID)
		{
			return false;
		}
	}

	for (uint32 i = 0; i < ARRAY_SIZE_IN_ELEMENTS(SpotLightsLoc); i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));

		snprintf(Name, sizeof(Name), "SpotLights[%d].Base.Base.Color", i);
		SpotLightsLoc[i].Color = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "SpotLights[%d].Base.Base.AmbientIntensity", i);
		SpotLightsLoc[i].AmbientIntensity = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "SpotLights[%d].Base.Position", i);
		SpotLightsLoc[i].Position = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "SpotLights[%d].Direction", i);
		SpotLightsLoc[i].Direction = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "SpotLights[%d].Cutoff", i);
		SpotLightsLoc[i].Cutoff = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "SpotLights[%d].Base.Base.DiffuseIntensity", i);
		SpotLightsLoc[i].DiffuseIntensity = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "SpotLights[%d].Base.Atten.Constant", i);
		SpotLightsLoc[i].Atten.Constant = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "SpotLights[%d].Base.Atten.Linear", i);
		SpotLightsLoc[i].Atten.Linear = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "SpotLights[%d].Base.Atten.Exp", i);
		SpotLightsLoc[i].Atten.Exp = GetUniformLocation(Name);

		if (SpotLightsLoc[i].Color == INVALID ||
			SpotLightsLoc[i].AmbientIntensity == INVALID ||
			SpotLightsLoc[i].Position == INVALID ||
			SpotLightsLoc[i].Direction == INVALID ||
			SpotLightsLoc[i].Cutoff == INVALID ||
			SpotLightsLoc[i].DiffuseIntensity == INVALID ||
			SpotLightsLoc[i].Atten.Constant == INVALID ||
			SpotLightsLoc[i].Atten.Linear == INVALID ||
			SpotLightsLoc[i].Atten.Exp == INVALID)
		{
			return false;
		}
	}

	if (WVPLoc == INVALID ||
		LightWVPLoc == INVALID ||
		WorldMatrixLoc == INVALID ||
		SamplerLoc == INVALID ||
		ShadowMapLoc == INVALID ||
		EyeWorldPosLoc == INVALID ||
		EnableFogLoc == INVALID ||
		MinFogDistLoc == INVALID ||
		FogColorLoc == INVALID ||
		DirLightLoc.AmbientIntensity == INVALID ||
		DirLightLoc.Color == INVALID ||
		DirLightLoc.Direction == INVALID ||
		DirLightLoc.DiffuseIntensity == INVALID ||
		NumPointLightsLoc == INVALID ||
		NumSpotLightsLoc == INVALID)
	{
		return false;
	}

	if (bSkinning)
	{
		for (uint32 i = 0; i < ARRAY_SIZE_IN_ELEMENTS(BoneLoc); i++)
		{
			char Name[128];
			memset(Name, 0, sizeof(Name));
			snprintf(Name, sizeof(Name), "Bones[%d]", i);
			BoneLoc[i] = GetUniformLocation(Name);
		}
	}

	return true;
}

void LightShader::SetWVP(const Matrix4& WVP)
{
	glUniformMatrix4fv(WVPLoc, 1, GL_TRUE, (const GLfloat*)WVP.m);
}

void LightShader::SetLightWVP(const Matrix4& LightWVP)
{
	glUniformMatrix4fv(LightWVPLoc, 1, GL_TRUE, (const GLfloat*)LightWVP.m);
}

void LightShader::SetWorldMatrix(const Matrix4& WorldInverse)
{
	glUniformMatrix4fv(WorldMatrixLoc, 1, GL_TRUE, (const GLfloat*)WorldInverse.m);
}

void LightShader::SetTextureUnit(uint32 TextureUnit)
{
	glUniform1i(SamplerLoc, TextureUnit);
}

void LightShader::SetShadowMapTextureUnit(uint32 TextureUnit)
{
	glUniform1i(ShadowMapLoc, TextureUnit);
}

void LightShader::SetEyeWorldPos(const Vector3& EyeWorldPos)
{
	glUniform3f(EyeWorldPosLoc, EyeWorldPos.x, EyeWorldPos.y, EyeWorldPos.z);
}

void LightShader::SetFog(uint8 bEnableFog)
{
	glUniform1i(EnableFogLoc, bEnableFog);
}

void LightShader::SetMinFogDist(float MinFogDist)
{
	glUniform1f(MinFogDistLoc, MinFogDist);
}

void LightShader::SetFogColor(Vector3 FogColor)
{
	glUniform3f(FogColorLoc, FogColor.x, FogColor.y, FogColor.z);
}

void LightShader::SetDirectionalLight(const DirectionalLight& Light)
{
	glUniform3f(DirLightLoc.Color, Light.Color.x, Light.Color.y, Light.Color.z);
	glUniform1f(DirLightLoc.AmbientIntensity, Light.AmbientIntensity);
	Vector3 Direction = Light.Direction;
	Direction.Normalize();
	glUniform3f(DirLightLoc.Direction, Direction.x, Direction.y, Direction.z);
	glUniform1f(DirLightLoc.DiffuseIntensity, Light.DiffuseIntensity);
}

void LightShader::SetPointLights(std::vector<struct PointLight> Lights)
{
	uint32 NumLights = (uint32)Lights.size();
	
	glUniform1i(NumPointLightsLoc, NumLights);

	for (uint32 i = 0; i < NumLights; i++)
	{
		glUniform3f(PointLightsLoc[i].Color, Lights[i].Color.x, Lights[i].Color.y, Lights[i].Color.z);
		glUniform1f(PointLightsLoc[i].AmbientIntensity, Lights[i].AmbientIntensity);
		glUniform1f(PointLightsLoc[i].DiffuseIntensity, Lights[i].DiffuseIntensity);
		glUniform3f(PointLightsLoc[i].Position, Lights[i].Position.x, Lights[i].Position.y, Lights[i].Position.z);
		glUniform1f(PointLightsLoc[i].Atten.Constant, Lights[i].Attenuation.Constant);
		glUniform1f(PointLightsLoc[i].Atten.Linear, Lights[i].Attenuation.Linear);
		glUniform1f(PointLightsLoc[i].Atten.Exp, Lights[i].Attenuation.Exp);
	}
}

void LightShader::SetSpotLights(std::vector<struct SpotLight> Lights)
{
	uint32 NumLights = (uint32)Lights.size();

	glUniform1i(NumSpotLightsLoc, NumLights);

	for (uint32 i = 0; i < NumLights; i++)
	{
		glUniform3f(SpotLightsLoc[i].Color, Lights[i].Color.x, Lights[i].Color.y, Lights[i].Color.z);
		glUniform1f(SpotLightsLoc[i].AmbientIntensity, Lights[i].AmbientIntensity);
		glUniform1f(SpotLightsLoc[i].DiffuseIntensity, Lights[i].DiffuseIntensity);
		glUniform3f(SpotLightsLoc[i].Position, Lights[i].Position.x, Lights[i].Position.y, Lights[i].Position.z);
		
		Vector3 Direction = Lights[i].Direction;
		Direction.Normalize();
		glUniform3f(SpotLightsLoc[i].Direction, Lights[i].Direction.x, Lights[i].Direction.y, Lights[i].Direction.z);
		
		glUniform1f(SpotLightsLoc[i].Cutoff, cosf(ToRadian(Lights[i].Cutoff)));
		glUniform1f(SpotLightsLoc[i].Atten.Constant, Lights[i].Attenuation.Constant);
		glUniform1f(SpotLightsLoc[i].Atten.Linear, Lights[i].Attenuation.Linear);
		glUniform1f(SpotLightsLoc[i].Atten.Exp, Lights[i].Attenuation.Exp);
	}
}

void LightShader::SetBoneTransform(uint32 Index, const Matrix4& Transform)
{
	if (bSkinning)
	{
		FILTH_ASSERT(Index < MAX_BONES);
		glUniformMatrix4fv(BoneLoc[Index], 1, GL_TRUE, (const GLfloat*)Transform);
	}
}