const int MAX_POINT_LIGHTS = 8;
const int MAX_SPOT_LIGHTS = 8;

in vec4 LightSpacePos;
noperspective in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;

struct VSOutput
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 WorldPos;
	vec4 LightSpacePos;
};

out vec4 FragColor;

struct BaseLight
{
	vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
};

struct DirectionalLight
{
	BaseLight Base;
	vec3 Direction;
};

struct Attenuation
{
	float Constant;
	float Linear;
	float Exp;
};

struct PointLight
{
	BaseLight Base;
	vec3 Position;
	Attenuation Atten;
};

struct SpotLight
{
	PointLight Base;
	vec3 Direction;
	float Cutoff;
};

uniform int NumPointLights;
uniform int NumSpotLights;
uniform DirectionalLight DirLight;
uniform PointLight PointLights[MAX_POINT_LIGHTS];
uniform SpotLight SpotLights[MAX_SPOT_LIGHTS];
uniform sampler2D Sampler;
uniform sampler2D ShadowMap;
uniform vec3 EyeWorldPos;
uniform bool bEnableFog;
uniform float MinFogDist;
uniform vec3 FogColor;

float CalcShadowFactor(VSOutput In)
{
	vec3 ProjCoords = In.LightSpacePos.xyz / In.LightSpacePos.w;
	vec2 UVCoords;
	UVCoords.x = 0.5 * ProjCoords.x + 0.5;
	UVCoords.y = 0.5 * ProjCoords.y + 0.5;
	float Z = 0.5 * ProjCoords.z + 0.5;
	float Depth = texture(ShadowMap, UVCoords).x;

	float ShadowFactor = 1.0;
	if (Depth < Z + 0.00001)
	{
		ShadowFactor = 0.5;
	}

	return ShadowFactor;
}

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, VSOutput In, float ShadowFactor)
{
	vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0f);
	float DiffuseFactor = dot(In.Normal, -LightDirection);

	vec4 DiffuseColor;

	if (DiffuseFactor > 0)
	{
		DiffuseColor = vec4(Light.Color, 1.0f) * Light.DiffuseIntensity * DiffuseFactor;
	}

	return (AmbientColor + ShadowFactor * DiffuseColor);
}

vec4 CalcDirectionalLight(VSOutput In)
{
	return CalcLightInternal(DirLight.Base, DirLight.Direction, In, 1.0);
}

vec4 CalcPointLight(PointLight L, VSOutput In)
{
	vec3 LightDirection = In.WorldPos - L.Position;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);
	float ShadowFactor = CalcShadowFactor(In);

	vec4 Color = CalcLightInternal(L.Base, LightDirection, In, ShadowFactor);
	float Attenuation = L.Atten.Constant + L.Atten.Linear * Distance + L.Atten.Exp * Distance * Distance;

	return Color / Attenuation;
}

vec4 CalcSpotLight(SpotLight L, VSOutput In)
{
	vec3 LightToPixel = normalize(In.WorldPos - L.Base.Position);
	float SpotFactor = dot(LightToPixel, L.Direction);

	vec4 Color = vec4(0, 0, 0, 0);

	if (SpotFactor > L.Cutoff)
	{
		Color = CalcPointLight(L.Base, In);
		Color = Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - L.Cutoff));
	}

	return Color;
}

void main()
{
	VSOutput In;
	In.TexCoord = TexCoord0;
	In.Normal = normalize(Normal0);
	In.WorldPos = WorldPos0;
	In.LightSpacePos = LightSpacePos;

	vec4 TotalLight = CalcDirectionalLight(In);

	for (int i = 0; i < NumPointLights; i++)
	{
		TotalLight += CalcPointLight(PointLights[i], In);
	}

	for (int i = 0; i < NumSpotLights; i++)
	{
		TotalLight += CalcSpotLight(SpotLights[i], In);
	}

	vec4 LightColor = texture2D(Sampler, In.TexCoord.xy) * TotalLight;

	if (bEnableFog)
	{
		float FogDist = length(In.WorldPos - EyeWorldPos);
		float FogFactor = min(MinFogDist / FogDist, 1.0);
		LightColor = mix(vec4(FogColor, 1.0), LightColor, FogFactor);
	}

	FragColor = LightColor;
}