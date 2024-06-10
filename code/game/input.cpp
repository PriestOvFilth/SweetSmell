#include "input.h"
#include "engine/debug.h"
#include "engine/sound.h"
#include "engine/camera.h"
#include "engine/render.h"
#include "scene.h"
#include "component.h"
#include "entity.h"
#include "player.h"

using namespace std;

void Input::HandleInput(SDL_Event *Event)
{
#ifndef FILTH_EDITOR
	SDL_SetRelativeMouseMode(SDL_TRUE);
#endif

	while (SDL_PollEvent(Event))
	{
		switch (Event->type)
		{
			case SDL_QUIT:
			{
				WriteLog("User requested quit\n");
				GQuit = true;
			} break;
			case SDL_MOUSEWHEEL:
			{
				if (Event->wheel.y > 0) ME.MouseWheel = 1;
				if (Event->wheel.y < 0) ME.MouseWheel = -1;
			} break;
			case SDL_MOUSEBUTTONDOWN:
			{
				if (Event->button.button == SDL_BUTTON_LEFT) ME.MousePressed[0] = (Event->button.clicks == SDL_PRESSED);
				if (Event->button.button == SDL_BUTTON_RIGHT) ME.MousePressed[1] = (Event->button.clicks == SDL_PRESSED);
				if (Event->button.button == SDL_BUTTON_MIDDLE) ME.MousePressed[2] = (Event->button.clicks == SDL_PRESSED);
				//GScene->EditorPlayer->HandleMouseEvents(ME);
			} break;
			case SDL_MOUSEMOTION:
			{
				ME.X = Event->motion.x;
				ME.Y = Event->motion.y;
				ME.XRel = Event->motion.xrel;
				ME.YRel = Event->motion.yrel;
			} break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				KE.Key = GetKeycodeFromSDL(Event->key.keysym.scancode);
				KE.bPressed = (Event->key.state == SDL_PRESSED);
				KE.bShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
				KE.bCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
				KE.bAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
				KE.bSuper = ((SDL_GetModState() & KMOD_GUI) != 0);

				if (KE.Key == KEY_ESCAPE)
				{
					WriteLog("User requested quit\n");
					GQuit = true;
				}
			} break;
		}
	}
}

uint32 Input::GetKeycodeFromSDL(uint32 SDL)
{
	for (int32 i = 0; i < 256; i++)
	{
		ScancodeMapping[i] = 0;
	}

	ScancodeMapping[SDL_SCANCODE_A] = KEY_A;
	ScancodeMapping[SDL_SCANCODE_B] = KEY_B;
	ScancodeMapping[SDL_SCANCODE_C] = KEY_C;
	ScancodeMapping[SDL_SCANCODE_D] = KEY_D;
	ScancodeMapping[SDL_SCANCODE_E] = KEY_E;
	ScancodeMapping[SDL_SCANCODE_F] = KEY_F;
	ScancodeMapping[SDL_SCANCODE_G] = KEY_G;
	ScancodeMapping[SDL_SCANCODE_H] = KEY_H;
	ScancodeMapping[SDL_SCANCODE_I] = KEY_I;
	ScancodeMapping[SDL_SCANCODE_J] = KEY_J;
	ScancodeMapping[SDL_SCANCODE_K] = KEY_K;
	ScancodeMapping[SDL_SCANCODE_L] = KEY_L;
	ScancodeMapping[SDL_SCANCODE_M] = KEY_M;
	ScancodeMapping[SDL_SCANCODE_N] = KEY_N;
	ScancodeMapping[SDL_SCANCODE_O] = KEY_O;
	ScancodeMapping[SDL_SCANCODE_P] = KEY_P;
	ScancodeMapping[SDL_SCANCODE_Q] = KEY_Q;
	ScancodeMapping[SDL_SCANCODE_R] = KEY_R;
	ScancodeMapping[SDL_SCANCODE_S] = KEY_S;
	ScancodeMapping[SDL_SCANCODE_T] = KEY_T;
	ScancodeMapping[SDL_SCANCODE_U] = KEY_U;
	ScancodeMapping[SDL_SCANCODE_V] = KEY_V;
	ScancodeMapping[SDL_SCANCODE_W] = KEY_W;
	ScancodeMapping[SDL_SCANCODE_X] = KEY_X;
	ScancodeMapping[SDL_SCANCODE_Y] = KEY_Y;
	ScancodeMapping[SDL_SCANCODE_Z] = KEY_Z;

	ScancodeMapping[SDL_SCANCODE_1] = KEY_1;
	ScancodeMapping[SDL_SCANCODE_2] = KEY_2;
	ScancodeMapping[SDL_SCANCODE_3] = KEY_3;
	ScancodeMapping[SDL_SCANCODE_4] = KEY_4;
	ScancodeMapping[SDL_SCANCODE_5] = KEY_5;
	ScancodeMapping[SDL_SCANCODE_6] = KEY_6;
	ScancodeMapping[SDL_SCANCODE_7] = KEY_7;
	ScancodeMapping[SDL_SCANCODE_8] = KEY_8;
	ScancodeMapping[SDL_SCANCODE_9] = KEY_9;
	ScancodeMapping[SDL_SCANCODE_0] = KEY_0;

	ScancodeMapping[SDL_SCANCODE_BACKSPACE] = KEY_BACKSPACE;
	ScancodeMapping[SDL_SCANCODE_TAB] = KEY_TAB;
	ScancodeMapping[SDL_SCANCODE_RETURN] = KEY_RETURN;
	ScancodeMapping[SDL_SCANCODE_LCTRL] = KEY_CONTROL;
	ScancodeMapping[SDL_SCANCODE_RCTRL] = KEY_CONTROL;
	ScancodeMapping[SDL_SCANCODE_LALT] = KEY_ALT;
	ScancodeMapping[SDL_SCANCODE_RALT] = KEY_ALT;
	ScancodeMapping[SDL_SCANCODE_LSHIFT] = KEY_SHIFT;
	ScancodeMapping[SDL_SCANCODE_RSHIFT] = KEY_SHIFT;
	ScancodeMapping[SDL_SCANCODE_PAUSE] = KEY_PAUSE;
	ScancodeMapping[SDL_SCANCODE_CAPSLOCK] = KEY_CAPSLOCK;
	ScancodeMapping[SDL_SCANCODE_ESCAPE] = KEY_ESCAPE;
	ScancodeMapping[SDL_SCANCODE_SPACE] = KEY_SPACE;
	ScancodeMapping[SDL_SCANCODE_PAGEDOWN] = KEY_PAGE_DOWN;
	ScancodeMapping[SDL_SCANCODE_PAGEUP] = KEY_PAGE_UP;
	ScancodeMapping[SDL_SCANCODE_END] = KEY_END;
	ScancodeMapping[SDL_SCANCODE_HOME] = KEY_HOME;
	ScancodeMapping[SDL_SCANCODE_LEFT] = KEY_LEFT;
	ScancodeMapping[SDL_SCANCODE_UP] = KEY_UP;
	ScancodeMapping[SDL_SCANCODE_RIGHT] = KEY_RIGHT;
	ScancodeMapping[SDL_SCANCODE_DOWN] = KEY_DOWN;
	ScancodeMapping[SDL_SCANCODE_PRINTSCREEN] = KEY_PRINT;
	ScancodeMapping[SDL_SCANCODE_INSERT] = KEY_INSERT;
	ScancodeMapping[SDL_SCANCODE_DELETE] = KEY_DELETE;
	ScancodeMapping[SDL_SCANCODE_HELP] = KEY_HELP;

	ScancodeMapping[SDL_SCANCODE_GRAVE] = KEY_TILDE;
	ScancodeMapping[SDL_SCANCODE_MINUS] = KEY_MINUS;
	ScancodeMapping[SDL_SCANCODE_EQUALS] = KEY_EQUALS;
	ScancodeMapping[SDL_SCANCODE_LEFTBRACKET] = KEY_LBRACKET;
	ScancodeMapping[SDL_SCANCODE_RIGHTBRACKET] = KEY_RBRACKET;
	ScancodeMapping[SDL_SCANCODE_BACKSLASH] = KEY_BACKSLASH;
	ScancodeMapping[SDL_SCANCODE_SEMICOLON] = KEY_SEMICOLON;
	ScancodeMapping[SDL_SCANCODE_APOSTROPHE] = KEY_APOSTROPHE;
	ScancodeMapping[SDL_SCANCODE_COMMA] = KEY_COMMA;
	ScancodeMapping[SDL_SCANCODE_PERIOD] = KEY_PERIOD;
	ScancodeMapping[SDL_SCANCODE_SLASH] = KEY_SLASH;
	ScancodeMapping[SDL_SCANCODE_KP_0] = KEY_NUMPAD0;
	ScancodeMapping[SDL_SCANCODE_KP_1] = KEY_NUMPAD1;
	ScancodeMapping[SDL_SCANCODE_KP_2] = KEY_NUMPAD2;
	ScancodeMapping[SDL_SCANCODE_KP_3] = KEY_NUMPAD3;
	ScancodeMapping[SDL_SCANCODE_KP_4] = KEY_NUMPAD4;
	ScancodeMapping[SDL_SCANCODE_KP_5] = KEY_NUMPAD5;
	ScancodeMapping[SDL_SCANCODE_KP_6] = KEY_NUMPAD6;
	ScancodeMapping[SDL_SCANCODE_KP_7] = KEY_NUMPAD7;
	ScancodeMapping[SDL_SCANCODE_KP_8] = KEY_NUMPAD8;
	ScancodeMapping[SDL_SCANCODE_KP_9] = KEY_NUMPAD9;
	ScancodeMapping[SDL_SCANCODE_KP_MULTIPLY] = KEY_MULTIPLY;
	ScancodeMapping[SDL_SCANCODE_KP_PLUS] = KEY_ADD;
	ScancodeMapping[SDL_SCANCODE_KP_EQUALS] = KEY_SEPARATOR;
	ScancodeMapping[SDL_SCANCODE_KP_MINUS] = KEY_SUBTRACT;
	ScancodeMapping[SDL_SCANCODE_KP_PERIOD] = KEY_DECIMAL;
	ScancodeMapping[SDL_SCANCODE_KP_DIVIDE] = KEY_DIVIDE;
	ScancodeMapping[SDL_SCANCODE_KP_ENTER] = KEY_NUMPADENTER;

	ScancodeMapping[SDL_SCANCODE_F1] = KEY_F1;
	ScancodeMapping[SDL_SCANCODE_F2] = KEY_F2;
	ScancodeMapping[SDL_SCANCODE_F3] = KEY_F3;
	ScancodeMapping[SDL_SCANCODE_F4] = KEY_F4;
	ScancodeMapping[SDL_SCANCODE_F5] = KEY_F5;
	ScancodeMapping[SDL_SCANCODE_F6] = KEY_F6;
	ScancodeMapping[SDL_SCANCODE_F7] = KEY_F7;
	ScancodeMapping[SDL_SCANCODE_F8] = KEY_F8;
	ScancodeMapping[SDL_SCANCODE_F9] = KEY_F9;
	ScancodeMapping[SDL_SCANCODE_F10] = KEY_F10;
	ScancodeMapping[SDL_SCANCODE_F11] = KEY_F11;
	ScancodeMapping[SDL_SCANCODE_F12] = KEY_F12;
	ScancodeMapping[SDL_SCANCODE_F13] = KEY_F13;
	ScancodeMapping[SDL_SCANCODE_F14] = KEY_F14;
	ScancodeMapping[SDL_SCANCODE_F15] = KEY_F15;
	ScancodeMapping[SDL_SCANCODE_F16] = KEY_F16;
	ScancodeMapping[SDL_SCANCODE_F17] = KEY_F17;
	ScancodeMapping[SDL_SCANCODE_F18] = KEY_F18;
	ScancodeMapping[SDL_SCANCODE_F19] = KEY_F19;
	ScancodeMapping[SDL_SCANCODE_F20] = KEY_F20;
	ScancodeMapping[SDL_SCANCODE_F21] = KEY_F21;
	ScancodeMapping[SDL_SCANCODE_F22] = KEY_F22;
	ScancodeMapping[SDL_SCANCODE_F23] = KEY_F23;
	ScancodeMapping[SDL_SCANCODE_F24] = KEY_F24;
	ScancodeMapping[SDL_SCANCODE_LCTRL] = KEY_LCONTROL;
	ScancodeMapping[SDL_SCANCODE_RCTRL] = KEY_RCONTROL;
	ScancodeMapping[SDL_SCANCODE_LALT] = KEY_LALT;
	ScancodeMapping[SDL_SCANCODE_RALT] = KEY_RALT;
	ScancodeMapping[SDL_SCANCODE_LSHIFT] = KEY_LSHIFT;
	ScancodeMapping[SDL_SCANCODE_RSHIFT] = KEY_RSHIFT;

	return ScancodeMapping[SDL];
}

void Input::BindKey(uint32 Key, string Action)
{
	KeyBindings.insert_or_assign(Action, Key);
}