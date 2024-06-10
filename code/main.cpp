#include "common/common.h"
#include "engine/render.h"
#include "engine/sound.h"
#include "engine/debug.h"
#include "game/input.h"
#include "game/scene.h"
#include "engine/resource.h"
#include "editor/leveleditor.h"
#include "editor/imgui_impl.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

class Renderer* GRenderer = NULL;
class AudioSystem* GAudio = NULL;
class DebugRenderer* GDebug = NULL;
class Scene* GScene = NULL;
class Input* GInput = NULL;
class ResourceManager* GResourceMan = NULL;
class LevelEditor* GLevelEditor = NULL;

bool GQuit = false;
bool GIsPaused = false;
bool GDisablePostEffects = false;
bool GIsLevelLoading = false;
bool GShowImGuiDemo = false;

uint64 GFrame = 0;
float GDeltaTime = 0.0f;
float GCurrentTime = 0.0f;

static float ProfilerLastTime = 0.0f;
static float Alpha = 0.2f;
static float FrameTime = 0.0f;

void PrintFPS(float UpdateTime, float RenderTime)
{
#if FILTH_DEBUG
	float ProfilerDeltaTime = GCurrentTime - ProfilerLastTime;
	FrameTime = Alpha * ProfilerDeltaTime + (1.0f - Alpha) * FrameTime;
	
	float FPS = 1000.0f / FrameTime;
	float MS = 1000.0f / FPS;

	ProfilerLastTime = GCurrentTime;

	GDebug->AddText(false, 1, "FPS: %.2f", FPS);
	GDebug->AddText(false, 1, "Total Frame Time: %.4f ms", MS);
	GDebug->AddText(false, 1, "Update Time: %.7f ms", UpdateTime);
	GDebug->AddText(false, 1, "Render Time: %.7f ms", RenderTime);
#endif
}

void Cleanup(SDL_Window* Window, SDL_GLContext GLContext)
{
	GLevelEditor->Destroy();
	SAFE_DELETE(GLevelEditor);

	GDebug->Destroy();
	SAFE_DELETE(GDebug);

	WriteLog("Destroying input handler...\n");
	SAFE_DELETE(GInput);

	WriteLog("Destroying resource manager...\n");
	SAFE_DELETE(GResourceMan);

	WriteLog("Destroying scene...\n");
	SAFE_DELETE(GScene);

	WriteLog("Destroying audio system...\n");
	GAudio->Destroy();
	SAFE_DELETE(GAudio);

	WriteLog("Destroying renderer...\n");
	SAFE_DELETE(GRenderer);

	WriteLog("Destroying OpenGL context...\n");
	SDL_GL_DeleteContext(GLContext);

	WriteLog("Destroying window...\n");
	SDL_DestroyWindow(Window);

	WriteLog("Closing engine...\n");
	SDL_Quit();
}

int main(int argc, char* args[])
{
	uint8 Initialized;
	SDL_Window *Window;
	SDL_Event Event;
	SDL_GLContext GLContext;
	GLenum GLEW;

	GQuit = false;

	// seed random number generator
	srand((uint32)time_t(NULL));
	
	InitGamePaths();
	InitLogFile();

	WriteLog("Initializing engine...\n");
	Initialized = SDL_Init(SDL_INIT_EVERYTHING);
	if (Initialized < 0)
	{
		WriteLog("SDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	WriteLog("Creating window with width %d and height %d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
	Window = SDL_CreateWindow("Sweet Smell", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	if (!Window)
	{
		WriteLog("SDL_CreateWindow failed: %s\n", SDL_GetError());
		return 1;
	}

	uint32 WindowID = SDL_GetWindowID(Window);

	SDL_DisableScreenSaver();

	WriteLog("Creating OpenGL context...\n");
	GLContext = SDL_GL_CreateContext(Window);

	WriteLog("Initializing GLEW...\n");
	GLEW = glewInit();
	if (GLEW != GLEW_OK)
	{
		WriteLog("Error initializing GLEW: %s\n", glewGetErrorString(GLEW));
		Cleanup(Window, GLContext);
		return 1;
	}

	WriteLog("OpenGL version: %s\n", glGetString(GL_VERSION));

	WriteLog("Initializing resource manager...\n");
	GResourceMan = new ResourceManager();

	WriteLog("Initializing renderer...\n");
	GRenderer = new Renderer(Window, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (!GRenderer->Init())
	{
		WriteLog("Unable to init renderer\n");
		Cleanup(Window, GLContext);
		return 1;
	}

	GDebug = new DebugRenderer();
	GDebug->Init();

	WriteLog("Initializing audio system...\n");
	GAudio = new AudioSystem();
	if (!GAudio->Init())
	{
		WriteLog("Error initializing audio system\n");
		Cleanup(Window, GLContext);
		return 1;
	}

	WriteLog("Initializing input handler...\n");
	GInput = new Input();

	WriteLog("Initializing scene manager...\n");
	GScene = new Scene();
	if (!GScene->Init())
	{
		WriteLog("Unable to init scene manager\n");
		Cleanup(Window, GLContext);
		return 1;
	}

#if FILTH_EDITOR
	WriteLog("Initializing level editor...\n");
	GLevelEditor = new LevelEditor();
	if (!GLevelEditor->Init(Window))
	{
		WriteLog("Error initializing level editor\n");
		Cleanup(Window, GLContext);
		return 1;
	}
#endif

	GScene->LoadNewLevel(GScene->StartupLevel.c_str());

	float LastTime = 0.0f;
	float DeltaTime = 0.0f;

	float StartTime = 0.0f;
	float EndTime = 0.0f;
	float UpdateTime = 0.0f;
	float RenderTime = 0.0f;

	while(!GQuit)
	{
		GCurrentTime = (float)SDL_GetTicks();
		DeltaTime = GCurrentTime - LastTime;
		GDeltaTime = DeltaTime;
		
		if (!GIsLevelLoading)
		{
			StartTime = GetDebugPerfCounter();
			GInput->HandleInput(&Event);

#if FILTH_EDITOR
			GLevelEditor->GUI->ProcessEvent(&Event);
#endif

			if (!GIsPaused)
			{
#if FILTH_EDITOR
				GLevelEditor->Update(DeltaTime);
#endif
				GScene->Update(DeltaTime);
				GAudio->Update();
			}
			EndTime = GetDebugPerfCounter();
			UpdateTime = GetDebugPerfTime(StartTime, EndTime);
		}

		StartTime = GetDebugPerfCounter();
		GRenderer->RenderFrame();
		EndTime = GetDebugPerfCounter();
		RenderTime = GetDebugPerfTime(StartTime, EndTime);

#if FILTH_EDITOR
		// display fps stats in debug
		PrintFPS(UpdateTime, RenderTime);

		GDebug->DrawShapes();

		GDebug->DrawText();

		GLevelEditor->Render();
#endif

		SDL_GL_SwapWindow(Window);

		LastTime = GCurrentTime;
		
		GFrame++;
	}

	Cleanup(Window, GLContext);

	return 0;
}