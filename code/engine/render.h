#pragma once

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include "common/common.h"
#include "common/math3d.h"
#include "shader.h"

class ParticleSystem;
class Scene;
class FullscreenQuad;
class DitherShader;
class PixelizeShader;
class LightShader;
class LoadingScreenShader;
class Texture;
class Cubemap;
class Camera;
class StaticMesh;

#define MAX_BONES 100

class Framebuffer
{
public:

	Framebuffer() {}
	~Framebuffer();

	bool Init(uint32 Width, uint32 Height);
	void Bind();
	void Unbind();
	bool CheckFBOStatus();

	GLuint FB;
	GLuint RB;
	GLuint FBTex;
	uint32 Width;
	uint32 Height;
};

class ShadowMapShader : public Shader
{
public:

	virtual bool Init() override;

	void SetWVP(const Matrix4& WVP);
	void SetTextureUnit(uint32 TextureUnit);
	void SetBoneTransform(uint32 Index, const Matrix4& Transform);
	void SetSkinningMode(uint8 bSkinningMode);

	GLuint WVPLoc;
	GLuint TexLoc;
	GLuint SkinningModeLoc;
	GLuint BoneLoc[MAX_BONES];
};

class ShadowMap
{
public:

	ShadowMap();
	~ShadowMap();

	bool Init(uint32 WindowWidth, uint32 WindowHeight);

	void BindForWriting();
	void BindForReading(GLenum TextureUnit);

	GLuint FBO;
	GLuint ShadowMapTex;

	class ShadowMapShader* Shader;
};

class SkyboxShader : public Shader
{
public:

	SkyboxShader() {}

	virtual bool Init() override;
	void SetWVP(const Matrix4& WVP);
	void SetTextureUnit(uint32 TextureUnit);
	void SetFog(uint8 bEnableFog);
	void SetFogColor(Vector3 FogColor);

	GLuint WVPLoc;
	GLuint TextureLoc;
	GLuint EnableFogLoc;
	GLuint FogColorLoc;
};

class Skybox
{
public:

	Skybox();
	~Skybox();

	bool Init(const std::string& Directory, const std::string& PosXFileName, const std::string& NegXFileName, const std::string& PosYFileName, const std::string& NegYFileName, const std::string& PosZFileName, const std::string& NegZFileName);
	void Render(Camera* Cam, PersProjInfo& P);

	SkyboxShader* Shader;
	Cubemap* CubemapTex;
	StaticMesh* SkySphere;
};

class Renderer
{
public:

	Renderer(SDL_Window* Window, int32 Width, int32 Height);

	~Renderer();

	bool Init();

	void RenderFrame();

	void ShadowMapPass();
	void LightingPass();
	void DitherPass();
	void PixelizePass();

	void DrawLoadingScreen();

	// the parameters TypeOfLight and LightIndex are for spot lights
	// as that is currently the only light type supported for shadows
	void SetShaderTransforms(bool bShadowPass, bool bSkinning, uint8 TypeOfLight, uint32 LightIndex, class MeshComponent* Comp);
	void RenderMesh(bool bShadowPass, class MeshComponent* Comp);

	void SetSkyboxFog();
	void SetLightFog(bool bSkinning);

	void SetPerspectiveProjection(void);

	// 0 - default, 1 - skinning
	bool InitLightShader(uint8 Type);

	LightShader* GetLightShader(bool bSkinning);

	PersProjInfo PersProj;
	
	uint32 WindowWidth;
	uint32 WindowHeight;
	SDL_Window* GameWindow;
	
	class Skybox* Sky;

	class ShadowMap* ShadowTech;
	
	class Framebuffer* DitherRT;
	class Framebuffer* PixelizeRT;
	
	class FullscreenQuad* Quad;
	
	class DitherShader* DitherEffect;
	class PixelizeShader* PixelizeEffect;

	class LightShader* DefaultShader;
	class LightShader* SkinningShader;

	class LoadingScreenShader* LoadingScreen;

	class Texture* LoadingScreenTexture;

	bool bEnableFog;
	float FogDistance;
	Vector3 FogColor;

	bool bRenderSky;
};