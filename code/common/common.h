#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#if FILTH_DEBUG || FILTH_EDITOR
#include <assert.h>
#endif

#define MAX_TEXT_FILE_LENGTH 4096
#define MAX_MSG_LENGTH 1024
#define INVALID 0xFFFFFFFF

#define BUFFER_OFFSET(a) ((char*)NULL + (a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

#if FILTH_DEBUG || FILTH_EDITOR
#define FILTH_ASSERT(expr) assert(expr)
#else
#define FILTH_ASSERT(expr)
#endif

//types
typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef	signed char	int8;
typedef signed short int int16;
typedef signed int int32;
typedef signed long long int64;

typedef wchar_t wchar;

// globals
extern class Renderer* GRenderer;
extern class AudioSystem* GAudio;
extern class DebugRenderer* GDebug;
extern class Scene* GScene;
extern class Input* GInput;
extern class ResourceManager* GResourceMan;
extern class LevelEditor* GLevelEditor;

extern bool GQuit;
extern bool GIsPaused;
extern bool GDisablePostEffects;
extern bool GIsLevelLoading;
extern bool GShowImGuiDemo;

extern uint64 GFrame;
extern float GDeltaTime;
extern float GCurrentTime;

// function declarations
void InitGamePaths(void);

void InitLogFile(void);

char* ReadTextFile(const char* FileName);

void WriteLog(const char* fmt, ...);

bool CheckGLError(const char *file, int line);
#define GLCheckError() CheckGLError(__FILE__,__LINE__)

// crash the game if we reach this
inline void DebugBreak()
{
#if FILTH_DEBUG
	*((int32*)3) = 13;
#endif
}

// use this if you just want a breakpoint that doesn't crash the game
inline void Breakpoint()
{
#if FILTH_DEBUG
	int32 i = 42;
#endif
}

float GetDebugPerfCounter();
float GetDebugPerfTime(float StartTime, float EndTime);