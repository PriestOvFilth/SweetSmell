#include <vector>
#include "game/scene.h"
#include "game/component.h"
#include "render.h"
#include "billboard.h"
#include "camera.h"
#include "light.h"
#include "particles.h"
#include "posteffect.h"
#include "texture.h"
#include "mesh.h"
#include "resource.h"

using namespace std;

Renderer::Renderer(SDL_Window* Window, int32 Width, int32 Height)
{
	GameWindow = Window;
	WindowWidth = Width;
	WindowHeight = Height;
	
	bEnableFog = false;
	FogDistance = 0.0f;
	FogColor = Vector3::Zero;

	bRenderSky = true;
}

Renderer::~Renderer()
{
	SAFE_DELETE(Sky);
	SAFE_DELETE(ShadowTech);
	SAFE_DELETE(DitherRT);
	SAFE_DELETE(PixelizeRT);
	SAFE_DELETE(Quad);
	SAFE_DELETE(DitherEffect);
	SAFE_DELETE(PixelizeEffect);
	SAFE_DELETE(DefaultShader);
	SAFE_DELETE(SkinningShader);
	SAFE_DELETE(LoadingScreen);
	SAFE_DELETE(LoadingScreenTexture);
}

bool Renderer::Init()
{
	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	SetPerspectiveProjection();

	LoadingScreen = new LoadingScreenShader();
	if (!LoadingScreen->Init())
	{
		WriteLog("Unable to init loading screen\n");
		return false;
	}

	LoadingScreenTexture = new Texture(GL_TEXTURE_2D, "data/textures/loading.jpg");
	LoadingScreenTexture->Init();
	if (!LoadingScreenTexture->bIsLoaded)
	{
		WriteLog("Unable to load loading screen texture\n");
		return false;
	}

	ShadowTech = new ShadowMap();
	if (!ShadowTech->Init(WindowWidth, WindowHeight))
	{
		WriteLog("Unable to init shadow mapper\n");
		return false;
	}

	ShadowTech->Shader->Bind();

	DitherRT = new Framebuffer();
	if (!DitherRT->Init(WindowWidth, WindowHeight))
	{
		WriteLog("Unable to init dither framebuffer\n");
		return false;
	}

	DitherEffect = new DitherShader();
	if (!DitherEffect->Init())
	{
		WriteLog("Unable to init dither shader\n");
		return false;
	}

	PixelizeRT = new Framebuffer();
	if (!PixelizeRT->Init(WindowWidth, WindowHeight))
	{
		WriteLog("Unable to init pixelize framebuffer\n");
	}

	PixelizeEffect = new PixelizeShader();
	if (!PixelizeEffect->Init())
	{
		WriteLog("Unable to init pixelize shader\n");
		return false;
	}

	Quad = new FullscreenQuad();
	if (!Quad->Init())
	{
		WriteLog("Unable to init fullscreen quad\n");
		return false;
	}

	Sky = new Skybox();

	return true;
}

void Renderer::RenderFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	ShadowMapPass();

	if (!GDisablePostEffects)
	{
		PixelizeRT->Bind();
	}
	
	LightingPass();

	if (bRenderSky)
	{
		Sky->Render(GScene->GetPlayerCamera()->Cam, PersProj);
	}

	if (!GDisablePostEffects)
	{
		PixelizePass();
		DitherPass();
	}
}

void Renderer::ShadowMapPass()
{
	ShadowTech->BindForWriting();
	
	glClear(GL_DEPTH_BUFFER_BIT);

	ShadowTech->Shader->Bind();

	glCullFace(GL_FRONT);
	GScene->Render(true);
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::LightingPass()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShadowTech->BindForReading(GL_TEXTURE1);

	GScene->Render(false);
}

void Renderer::PixelizePass()
{
	PixelizeRT->Unbind();
	DitherRT->Bind();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	
	PixelizeEffect->Bind();
	PixelizeEffect->SetTextureUnit(0);
	PixelizeEffect->SetViewportSize(Vector2((float)WindowWidth, (float)WindowHeight));
	PixelizeEffect->SetPixelSize(200.f);
	
	Quad->Draw(PixelizeRT->FBTex);
	
	glCullFace(GL_BACK);
}

void Renderer::DitherPass()
{
	DitherRT->Unbind();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	
	DitherEffect->Bind();
	DitherEffect->SetTextureUnit(0);
	
	Quad->Draw(DitherRT->FBTex);
	
	glCullFace(GL_BACK);
}

void Renderer::DrawLoadingScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	
	LoadingScreen->Bind();
	LoadingScreen->SetTextureUnit(0);
	
	Quad->Draw(LoadingScreenTexture->TextureObj);
	
	glCullFace(GL_BACK);

	// get the texture up on the screen immediately
	SDL_GL_SwapWindow(GameWindow);
}

void Renderer::SetShaderTransforms(bool bShadowPass, bool bSkinning, uint8 TypeOfLight, uint32 LightIndex, MeshComponent* Comp)
{
	Transform T = Comp->Trans;

	if (!bShadowPass)
	{
		Camera* PlayerCamera = GScene->GetPlayerCamera()->Cam;
		T.SetPerspectiveProj(PersProj);
		T.SetCamera(PlayerCamera->Pos, PlayerCamera->Target, PlayerCamera->Up);

		GRenderer->GetLightShader(bSkinning)->SetWVP(T.GetWVPTrans());
		GRenderer->GetLightShader(bSkinning)->SetWorldMatrix(T.GetWorldTrans());

		if (TypeOfLight == LT_Spot)
		{
			SpotLight SL = GScene->SceneLights.SpotLights[LightIndex];
			T.SetCamera(SL.Position, SL.Direction, Vector3(0.0f, 1.0f, 0.0f));
			GRenderer->GetLightShader(bSkinning)->SetLightWVP(T.GetWVPTrans());
		}
	}
	else
	{
		if (Comp->bCastsShadow)
		{
			GRenderer->ShadowTech->Shader->SetSkinningMode(bSkinning ? 1 : 0);

			if (TypeOfLight == LT_Spot)
			{
				SpotLight SL = GScene->SceneLights.SpotLights[LightIndex];
				T.SetCamera(SL.Position, SL.Direction, Vector3(0.0f, 1.0f, 0.0f));
				T.SetPerspectiveProj(PersProj);
				GRenderer->ShadowTech->Shader->SetWVP(T.GetWVPTrans());
			}
		}
	}
}

void Renderer::RenderMesh(bool bShadowPass, MeshComponent* Comp)
{
	std::vector<Matrix4> BoneTransforms;

	bool bSkeletal = false;
	SkeletalMeshComponent* SkelMesh = dynamic_cast<SkeletalMeshComponent*>(Comp);
	if (SkelMesh)
	{
		bSkeletal = true;
		BoneTransforms = SkelMesh->BoneTransforms;
	}

	if (!bShadowPass)
	{
		GRenderer->GetLightShader(bSkeletal)->Bind();

		GRenderer->GetLightShader(bSkeletal)->SetPointLights(GScene->SceneLights.PointLights);
		GRenderer->GetLightShader(bSkeletal)->SetSpotLights(GScene->SceneLights.SpotLights);
		GRenderer->GetLightShader(bSkeletal)->SetDirectionalLight(GScene->SceneLights.Ambient);

		Camera* PlayerCamera = GScene->GetPlayerCamera()->Cam;
		GRenderer->GetLightShader(bSkeletal)->SetEyeWorldPos(PlayerCamera->Pos);
		GRenderer->SetLightFog(bSkeletal);

		if (bSkeletal)
		{
			for (uint32 i = 0; i < BoneTransforms.size(); i++)
			{
				GRenderer->GetLightShader(true)->SetBoneTransform(i, BoneTransforms[i]);
			}
		}
		
	}
	else
	{
		if (bSkeletal)
		{
			for (uint32 i = 0; i < BoneTransforms.size(); i++)
			{
				GRenderer->ShadowTech->Shader->SetBoneTransform(i, BoneTransforms[i]);
			}
		}
	}

	// render for every spot light in the scene so we can set the wvp for each light for shadowmapping
	for (uint32 i = 0; i < GScene->SceneLights.SpotLights.size(); i++)
	{
		SetShaderTransforms(bShadowPass, bSkeletal, LT_Spot, i, Comp);
		Comp->RenderMesh->DrawMesh();
	}

	SetShaderTransforms(bShadowPass, bSkeletal, LT_Other, 0, Comp);
	Comp->RenderMesh->DrawMesh();
}

void Renderer::SetSkyboxFog()
{
	if (!Sky)
	{
		return;
	}

	if (bEnableFog)
	{
		Sky->Shader->SetFog(1);
		Sky->Shader->SetFogColor(FogColor);
	}
	else
	{
		Sky->Shader->SetFog(0);
	}
}

void Renderer::SetLightFog(bool bSkinning)
{
	if (!DefaultShader || !SkinningShader)
	{
		return;
	}

	if (bEnableFog)
	{
		GetLightShader(bSkinning)->SetFog(1);
		GetLightShader(bSkinning)->SetMinFogDist(FogDistance);
		GetLightShader(bSkinning)->SetFogColor(FogColor);
	}
	else
	{
		GetLightShader(bSkinning)->SetFog(0);
	}
}

void Renderer::SetPerspectiveProjection(void)
{
	PersProj.FOV = 60.f;
	PersProj.Height = (float)WindowHeight;
	PersProj.Width = (float)WindowWidth;
	PersProj.zNear = 0.1f;
	PersProj.zFar = 1000.f;
}

bool Renderer::InitLightShader(uint8 Type)
{
	switch (Type)
	{
		case 0:
		{
			DefaultShader = new LightShader();
			if (!DefaultShader->Init(false))
			{
				WriteLog("Unable to init default lighting shader\n");
				return false;
			}

			DefaultShader->Bind();
			DefaultShader->SetTextureUnit(0);
			DefaultShader->SetShadowMapTextureUnit(1);
		} break;
		case 1:
		{
			SkinningShader = new LightShader();
			if (!SkinningShader->Init(true))
			{
				WriteLog("Unable to init GPU skinning shader\n");
				return false;
			}

			SkinningShader->Bind();
			SkinningShader->SetTextureUnit(0);
			SkinningShader->SetShadowMapTextureUnit(1);
		} break;
	}

	return true;
}

LightShader* Renderer::GetLightShader(bool bSkinning)
{
	if (bSkinning)
	{
		return SkinningShader;
	}

	return DefaultShader;
}

Framebuffer::~Framebuffer()
{
	glDeleteTextures(1, &FBTex);
	glDeleteRenderbuffersEXT(1, &RB);
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &FB);
}

bool Framebuffer::Init(uint32 Width, uint32 Height)
{
	glGenTextures(1, &FBTex);
	glBindTexture(GL_TEXTURE_2D, FBTex);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &RB);
	glBindRenderbuffer(GL_RENDERBUFFER, RB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &FB);
	glBindFramebuffer(GL_FRAMEBUFFER, FB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBTex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RB);

	GLenum DrawBufs[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBufs);

	if (!CheckFBOStatus())
	{
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FB);
	glBindRenderbuffer(GL_RENDERBUFFER, RB);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool Framebuffer::CheckFBOStatus()
{
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	const char* ErrorString = 0;
	char Buffer[80];

	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (Status)
		{
		case GL_FRAMEBUFFER_UNSUPPORTED:
			ErrorString = "UNSUPPORTED";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			ErrorString = "INCOMPLETE ATTACHMENT";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			ErrorString = "INCOMPLETE DRAW BUFFER";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			ErrorString = "INCOMPLETE READ BUFFER";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			ErrorString = "INCOMPLETE MISSING ATTACHMENT";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			ErrorString = "INCOMPLETE MULTISAMPLE";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			ErrorString = "INCOMPLETE LAYER TARGETS";
			break;
		default:
			sprintf(Buffer, "0x%x", Status);
			ErrorString = Buffer;
			break;
		}

		WriteLog("Error: glCheckFramebufferStatus() returned '%s'\n", ErrorString);

		return false;
	}

	return true;
}

bool ShadowMapShader::Init()
{
	if (!Shader::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "data/shaders/ShadowMap.vert"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, "data/shaders/ShadowMap.frag"))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	WVPLoc = GetUniformLocation("WVP");
	TexLoc = GetUniformLocation("ShadowMap");
	SkinningModeLoc = GetUniformLocation("bSkinningMode");

	if (WVPLoc == INVALID ||
		TexLoc == INVALID ||
		SkinningModeLoc == INVALID)
	{
		return false;
	}

	for (uint32 i = 0; i < ARRAY_SIZE_IN_ELEMENTS(BoneLoc); i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));
		snprintf(Name, sizeof(Name), "Bones[%d]", i);
		BoneLoc[i] = GetUniformLocation(Name);
	}

	return true;
}

void ShadowMapShader::SetWVP(const Matrix4& WVP)
{
	glUniformMatrix4fv(WVPLoc, 1, GL_TRUE, (const GLfloat*)WVP.m);
}

void ShadowMapShader::SetTextureUnit(uint32 TextureUnit)
{
	glUniform1i(TexLoc, TextureUnit);
}

void ShadowMapShader::SetBoneTransform(uint32 Index, const Matrix4& Transform)
{
	FILTH_ASSERT(Index < MAX_BONES);
	glUniformMatrix4fv(BoneLoc[Index], 1, GL_TRUE, (const GLfloat*)Transform);
}

void ShadowMapShader::SetSkinningMode(uint8 bSkinningMode)
{
	glUniform1i(SkinningModeLoc, bSkinningMode);
}

ShadowMap::ShadowMap()
{
	Shader = NULL;
	FBO = 0;
	ShadowMapTex = 0;
}

ShadowMap::~ShadowMap()
{
	SAFE_DELETE(Shader);

	if (FBO != 0)
	{
		glDeleteFramebuffers(1, &FBO);
	}

	if (ShadowMapTex != 0)
	{
		glDeleteTextures(1, &ShadowMapTex);
	}
}

bool ShadowMap::Init(uint32 WindowWidth, uint32 WindowHeight)
{
	glGenFramebuffers(1, &FBO);

	glGenTextures(1, &ShadowMapTex);
	glBindTexture(GL_TEXTURE_2D, ShadowMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ShadowMapTex, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		WriteLog("Framebuffer error, status: 0x%x\n", Status);
		return false;
	}

	Shader = new ShadowMapShader();
	if (!Shader->Init())
	{
		return false;
	}

	return true;
}

void ShadowMap::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
}

void ShadowMap::BindForReading(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, ShadowMapTex);
}

bool SkyboxShader::Init()
{
	if (!Shader::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "data/shaders/Skybox.vert"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, "data/shaders/Skybox.frag"))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	WVPLoc = GetUniformLocation("WVP");
	TextureLoc = GetUniformLocation("CubemapTex");
	EnableFogLoc = GetUniformLocation("bEnableFog");
	FogColorLoc = GetUniformLocation("FogColor");

	if (WVPLoc == INVALID ||
		TextureLoc == INVALID ||
		EnableFogLoc == INVALID ||
		FogColorLoc == INVALID)
	{
		return false;
	}

	return true;
}

void SkyboxShader::SetWVP(const Matrix4& WVP)
{
	glUniformMatrix4fv(WVPLoc, 1, GL_TRUE, (const GLfloat*)WVP.m);
}

void SkyboxShader::SetTextureUnit(uint32 TextureUnit)
{
	glUniform1i(TextureLoc, TextureUnit);
}

void SkyboxShader::SetFog(uint8 bEnableFog)
{
	glUniform1i(EnableFogLoc, bEnableFog);
}

void SkyboxShader::SetFogColor(Vector3 FogColor)
{
	glUniform3f(FogColorLoc, FogColor.x, FogColor.y, FogColor.z);
}

Skybox::Skybox()
{
	Shader = NULL;
	CubemapTex = NULL;
}

Skybox::~Skybox()
{
	SAFE_DELETE(Shader);

	SAFE_DELETE(CubemapTex);
}

bool Skybox::Init(const string& Directory, const string& PosXFileName, const string& NegXFileName, const string& PosYFileName, const string& NegYFileName, const string& PosZFileName, const string& NegZFileName)
{
	Shader = new SkyboxShader();

	if (!Shader->Init())
	{
		WriteLog("Error initializing skybox shader\n");
		return false;
	}

	Shader->Bind();
	Shader->SetTextureUnit(0);

	CubemapTex = new Cubemap(Directory, PosXFileName, NegXFileName, PosYFileName, NegYFileName, PosZFileName, NegZFileName);
	if (!CubemapTex->Init())
	{
		return false;
	}

	SkySphere = GResourceMan->GetStaticMesh("data/models/sphere.dae");
	if (!SkySphere->bIsLoaded)
	{
		return false;
	}

	return true;
}

void Skybox::Render(Camera* Cam, PersProjInfo& P)
{
	GLint OldCullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);

	GLint OldDepthFuncMode;
	glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	Shader->Bind();

	GRenderer->SetSkyboxFog();

	Transform T;
	T.SetScale(999.f, 999.f, 999.f);
	T.SetRotation(0.0f, 0.0f, 0.0f);
	T.SetWorldPos(Cam->Pos.x, Cam->Pos.y, Cam->Pos.z);
	T.SetCamera(Cam->Pos, Cam->Target, Cam->Up);
	T.SetPerspectiveProj(P);
	Shader->SetWVP(T.GetWVPTrans());
	CubemapTex->Bind(GL_TEXTURE0);
	SkySphere->DrawMesh();

	glCullFace(OldCullFaceMode);
	glDepthFunc(OldDepthFuncMode);
}