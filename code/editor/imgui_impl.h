#include <GL/glew.h>
#include <SDL.h>
#include "imgui/imgui.h"
#include "common/common.h"
#include "common/math3d.h"

static void RenderDrawLists(ImDrawData* DrawData);
static const char* GetClipboardText(void*);
static void SetClipboardText(void*, const char* text);

class ImGui_Impl
{
public:

	ImGui_Impl();

	bool Init(SDL_Window* Window);
	void ProcessEvent(SDL_Event* Event);
	void Destroy();

	void NewFrame();
	void CreateDeviceObjects();
	void InvalidateDeviceObjects();
	void CreateFontsTexture();

	int32 ShaderHandle;
	int32 VertHandle;
	int32 FragHandle;

	int32 AttribLocationTex;
	int32 AttribLocationProjMtx;
	int32 AttribLocationPosition;
	int32 AttribLocationUV;
	int32 AttribLocationColor;

	uint32 VboHandle;
	uint32 VaoHandle;
	uint32 ElementsHandle;

	GLuint FontTexture;

	bool MousePressed[3];
	float MouseWheel;

	SDL_Window* GameWindow;
};