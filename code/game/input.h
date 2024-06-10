#pragma once

#include <SDL.h>
#include <map>
#include <string>
#include "common/common.h"

enum Keycodes
{
	KEY_NULL = 0x000,
	KEY_BACKSPACE = 0x001,
	KEY_TAB = 0x002,
	KEY_RETURN = 0x003,
	KEY_CONTROL = 0x004,
	KEY_ALT = 0x005,
	KEY_SHIFT = 0x006,
	KEY_PAUSE = 0x007,
	KEY_CAPSLOCK = 0x008,
	KEY_ESCAPE = 0x009,
	KEY_SPACE = 0x00a,
	KEY_PAGE_DOWN = 0x00b,
	KEY_PAGE_UP = 0x00c,
	KEY_END = 0x00d,
	KEY_HOME = 0x00e,
	KEY_LEFT = 0x00f,
	KEY_UP = 0x010,
	KEY_RIGHT = 0x011,
	KEY_DOWN = 0x012,
	KEY_PRINT = 0x013,
	KEY_INSERT = 0x014,
	KEY_DELETE = 0x015,
	KEY_HELP = 0x016,

	KEY_0 = 0x017,
	KEY_1 = 0x018,
	KEY_2 = 0x019,
	KEY_3 = 0x01a,
	KEY_4 = 0x01b,
	KEY_5 = 0x01c,
	KEY_6 = 0x01d,
	KEY_7 = 0x01e,
	KEY_8 = 0x01f,
	KEY_9 = 0x020,

	KEY_A = 0x021,
	KEY_B = 0x022,
	KEY_C = 0x023,
	KEY_D = 0x024,
	KEY_E = 0x025,
	KEY_F = 0x026,
	KEY_G = 0x027,
	KEY_H = 0x028,
	KEY_I = 0x029,
	KEY_J = 0x02a,
	KEY_K = 0x02b,
	KEY_L = 0x02c,
	KEY_M = 0x02d,
	KEY_N = 0x02e,
	KEY_O = 0x02f,
	KEY_P = 0x030,
	KEY_Q = 0x031,
	KEY_R = 0x032,
	KEY_S = 0x033,
	KEY_T = 0x034,
	KEY_U = 0x035,
	KEY_V = 0x036,
	KEY_W = 0x037,
	KEY_X = 0x038,
	KEY_Y = 0x039,
	KEY_Z = 0x03a,

	KEY_TILDE = 0x03b,
	KEY_MINUS = 0x03c,
	KEY_EQUALS = 0x03d,
	KEY_LBRACKET = 0x03e,
	KEY_RBRACKET = 0x03f,
	KEY_BACKSLASH = 0x040,
	KEY_SEMICOLON = 0x041,
	KEY_APOSTROPHE = 0x042,
	KEY_COMMA = 0x043,
	KEY_PERIOD = 0x044,
	KEY_SLASH = 0x045,
	KEY_NUMPAD0 = 0x046,
	KEY_NUMPAD1 = 0x047,
	KEY_NUMPAD2 = 0x048,
	KEY_NUMPAD3 = 0x049,
	KEY_NUMPAD4 = 0x04a,
	KEY_NUMPAD5 = 0x04b,
	KEY_NUMPAD6 = 0x04c,
	KEY_NUMPAD7 = 0x04d,
	KEY_NUMPAD8 = 0x04e,
	KEY_NUMPAD9 = 0x04f,
	KEY_MULTIPLY = 0x050,
	KEY_ADD = 0x051,
	KEY_SEPARATOR = 0x052,
	KEY_SUBTRACT = 0x053,
	KEY_DECIMAL = 0x054,
	KEY_DIVIDE = 0x055,
	KEY_NUMPADENTER = 0x056,

	KEY_F1 = 0x057,
	KEY_F2 = 0x058,
	KEY_F3 = 0x059,
	KEY_F4 = 0x05a,
	KEY_F5 = 0x05b,
	KEY_F6 = 0x05c,
	KEY_F7 = 0x05d,
	KEY_F8 = 0x05e,
	KEY_F9 = 0x05f,
	KEY_F10 = 0x060,
	KEY_F11 = 0x061,
	KEY_F12 = 0x062,
	KEY_F13 = 0x063,
	KEY_F14 = 0x064,
	KEY_F15 = 0x065,
	KEY_F16 = 0x066,
	KEY_F17 = 0x067,
	KEY_F18 = 0x068,
	KEY_F19 = 0x069,
	KEY_F20 = 0x06a,
	KEY_F21 = 0x06b,
	KEY_F22 = 0x06c,
	KEY_F23 = 0x06d,
	KEY_F24 = 0x06e,

	KEY_NUMLOCK = 0x06f,
	KEY_SCROLLLOCK = 0x070,
	KEY_LCONTROL = 0x071,
	KEY_RCONTROL = 0x072,
	KEY_LALT = 0x073,
	KEY_RALT = 0x074,
	KEY_LSHIFT = 0x075,
	KEY_RSHIFT = 0x076,
	KEY_WIN_LWINDOW = 0x077,
	KEY_WIN_RWINDOW = 0x078,
	KEY_WIN_APPS = 0x079,
	KEY_OEM_102 = 0x080,

	KEY_MAC_OPT = 0x090,
	KEY_MAC_LOPT = 0x091,
	KEY_MAC_ROPT = 0x092,

	KEY_ANYKEY = 0xfffe,
};

struct KeyboardEvent
{
	uint32 Key;
	bool bPressed;
	bool bShift;
	bool bCtrl;
	bool bAlt;
	bool bSuper;
};

struct MouseEvent
{
	float MouseWheel;
	bool MousePressed[3];
	int32 X;
	int32 Y;
	int32 XRel;
	int32 YRel;
};

class Input
{
public:

	void HandleInput(SDL_Event *Event);
	uint32 GetKeycodeFromSDL(uint32 SDL);
	void BindKey(uint32 Key, std::string Action);

	uint32 ScancodeMapping[256];
	std::map<std::string, uint32> KeyBindings;

	KeyboardEvent KE;
	MouseEvent ME;
};