#include <SDL_syswm.h>
#include "imgui_impl.h"
#include "leveleditor.h"

static void RenderDrawLists(ImDrawData* DrawData)
{
	ImGuiIO& io = ImGui::GetIO();
	int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width == 0 || fb_height == 0)
		return;
	DrawData->ScaleClipRects(io.DisplayFramebufferScale);

	// Backup GL state
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
	GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
	GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
	GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	// Setup orthographic projection matrix
	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
	const float ortho_projection[4][4] =
	{
		{ 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
	{ 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
	{ 0.0f,                  0.0f,                  -1.0f, 0.0f },
	{ -1.0f,                  1.0f,                   0.0f, 1.0f },
	};
	glUseProgram(GLevelEditor->GUI->ShaderHandle);
	glUniform1i(GLevelEditor->GUI->AttribLocationTex, 0);
	glUniformMatrix4fv(GLevelEditor->GUI->AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
	glBindVertexArray(GLevelEditor->GUI->VaoHandle);

	for (int n = 0; n < DrawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = DrawData->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, GLevelEditor->GUI->VboHandle);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLevelEditor->GUI->ElementsHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}

	// Restore modified GL state
	glUseProgram(last_program);
	glActiveTexture(last_active_texture);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindVertexArray(last_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFunc(last_blend_src, last_blend_dst);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

static const char* GetClipboardText(void*)
{
	return SDL_GetClipboardText();
}

static void SetClipboardText(void*, const char* text)
{
	SDL_SetClipboardText(text);
}

ImGui_Impl::ImGui_Impl()
{
	int32 ShaderHandle = 0;
	int32 VertHandle = 0;
	int32 FragHandle = 0;

	int32 AttribLocationTex = 0;
	int32 AttribLocationProjMtx = 0;
	int32 AttribLocationPosition = 0;
	int32 AttribLocationUV = 0;
	int32 AttribLocationColor = 0;

	uint32 VboHandle = 0;
	uint32 VaoHandle = 0;
	uint32 ElementsHandle = 0;

	GLuint FontTexture = 0;
}

bool ImGui_Impl::Init(SDL_Window* Window)
{
	GameWindow = Window;

	MousePressed[0] = false;
	MousePressed[1] = false;
	MousePressed[2] = false;

	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = SDLK_a;
	io.KeyMap[ImGuiKey_C] = SDLK_c;
	io.KeyMap[ImGuiKey_V] = SDLK_v;
	io.KeyMap[ImGuiKey_X] = SDLK_x;
	io.KeyMap[ImGuiKey_Y] = SDLK_y;
	io.KeyMap[ImGuiKey_Z] = SDLK_z;

	io.RenderDrawListsFn = RenderDrawLists;
	io.SetClipboardTextFn = SetClipboardText;
	io.GetClipboardTextFn = GetClipboardText;
	io.ClipboardUserData = NULL;

	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(GameWindow, &wmInfo);
	io.ImeWindowHandle = wmInfo.info.win.window;

	return true;
}

void ImGui_Impl::ProcessEvent(SDL_Event* Event)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (Event->type)
	{
	case SDL_MOUSEWHEEL:
	{
		if (Event->wheel.y > 0)
			MouseWheel = 1;
		if (Event->wheel.y < 0)
			MouseWheel = -1;
	} break;
	case SDL_MOUSEBUTTONDOWN:
	{
		if (Event->button.button == SDL_BUTTON_LEFT) MousePressed[0] = true;
		if (Event->button.button == SDL_BUTTON_RIGHT) MousePressed[1] = true;
		if (Event->button.button == SDL_BUTTON_MIDDLE) MousePressed[2] = true;
	} break;
	case SDL_TEXTINPUT:
	{
		io.AddInputCharactersUTF8(Event->text.text);
	} break;
	case SDL_KEYDOWN:
	case SDL_KEYUP:
	{
		int key = Event->key.keysym.sym & ~SDLK_SCANCODE_MASK;
		io.KeysDown[key] = (Event->type == SDL_KEYDOWN);
		io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
		io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
		io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
		io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
	} break;
	}
}

void ImGui_Impl::Destroy()
{
	InvalidateDeviceObjects();
	ImGui::Shutdown();
}

void ImGui_Impl::NewFrame()
{
	if (!FontTexture)
		CreateDeviceObjects();

	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int display_w, display_h;
	SDL_GetWindowSize(GameWindow, &w, &h);
	SDL_GL_GetDrawableSize(GameWindow, &display_w, &display_h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

	// Setup time step
	io.DeltaTime = GDeltaTime;

	// Setup inputs
	// (we already got mouse wheel, keyboard keys & characters from SDL_PollEvent())
	int mx, my;
	Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
	if (SDL_GetWindowFlags(GameWindow) & SDL_WINDOW_MOUSE_FOCUS)
		io.MousePos = ImVec2((float)mx, (float)my);   // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
	else
		io.MousePos = ImVec2(-1, -1);

	io.MouseDown[0] = MousePressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;		// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
	io.MouseDown[1] = MousePressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
	io.MouseDown[2] = MousePressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
	MousePressed[0] = MousePressed[1] = MousePressed[2] = false;

	io.MouseWheel = MouseWheel;
	MouseWheel = 0.0f;

	// Hide OS mouse cursor if ImGui is drawing it
	//SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);

	// Start the frame
	ImGui::NewFrame();
}

void ImGui_Impl::CreateDeviceObjects()
{
	GLint last_texture, last_array_buffer, last_vertex_array;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

	const GLchar *vertex_shader =
		"#version 330\n"
		"uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec2 UV;\n"
		"in vec4 Color;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"	Frag_UV = UV;\n"
		"	Frag_Color = Color;\n"
		"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar* fragment_shader =
		"#version 330\n"
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
		"}\n";

	ShaderHandle = glCreateProgram();
	VertHandle = glCreateShader(GL_VERTEX_SHADER);
	FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(VertHandle, 1, &vertex_shader, 0);
	glShaderSource(FragHandle, 1, &fragment_shader, 0);
	glCompileShader(VertHandle);
	glCompileShader(FragHandle);
	glAttachShader(ShaderHandle, VertHandle);
	glAttachShader(ShaderHandle, FragHandle);
	glLinkProgram(ShaderHandle);

	AttribLocationTex = glGetUniformLocation(ShaderHandle, "Texture");
	AttribLocationProjMtx = glGetUniformLocation(ShaderHandle, "ProjMtx");
	AttribLocationPosition = glGetAttribLocation(ShaderHandle, "Position");
	AttribLocationUV = glGetAttribLocation(ShaderHandle, "UV");
	AttribLocationColor = glGetAttribLocation(ShaderHandle, "Color");

	glGenBuffers(1, &VboHandle);
	glGenBuffers(1, &ElementsHandle);

	glGenVertexArrays(1, &VaoHandle);
	glBindVertexArray(VaoHandle);
	glBindBuffer(GL_ARRAY_BUFFER, VboHandle);
	glEnableVertexAttribArray(AttribLocationPosition);
	glEnableVertexAttribArray(AttribLocationUV);
	glEnableVertexAttribArray(AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
	glVertexAttribPointer(AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

	CreateFontsTexture();

	// Restore modified GL state
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindVertexArray(last_vertex_array);
}

void ImGui_Impl::InvalidateDeviceObjects()
{
	if (VaoHandle) glDeleteVertexArrays(1, &VaoHandle);
	if (VboHandle) glDeleteBuffers(1, &VboHandle);
	if (ElementsHandle) glDeleteBuffers(1, &ElementsHandle);
	VaoHandle = VboHandle = ElementsHandle = 0;

	if (ShaderHandle && VertHandle) glDetachShader(ShaderHandle, VertHandle);
	if (VertHandle) glDeleteShader(VertHandle);
	VertHandle = 0;

	if (ShaderHandle && FragHandle) glDetachShader(ShaderHandle, FragHandle);
	if (FragHandle) glDeleteShader(FragHandle);
	FragHandle = 0;

	if (ShaderHandle) glDeleteProgram(ShaderHandle);
	ShaderHandle = 0;

	if (FontTexture)
	{
		glDeleteTextures(1, &FontTexture);
		ImGui::GetIO().Fonts->TexID = 0;
		FontTexture = 0;
	}
}

void ImGui_Impl::CreateFontsTexture()
{
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

															  // Upload texture to graphics system
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &FontTexture);
	glBindTexture(GL_TEXTURE_2D, FontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)FontTexture;

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);
}