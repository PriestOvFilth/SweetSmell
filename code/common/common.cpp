#include <SDL.h>
#include <GL/glew.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include "common.h"

using namespace std;

static const char *LogFileName = "log.txt";

static const char *DataPath;
static const char *GamePath;

static string LogPath;
static FILE* LogFile;

float GetDebugPerfCounter()
{
#if FILTH_DEBUG
	return (float)SDL_GetTicks();
#else
	return 0.0f;
#endif
}

float GetDebugPerfTime(float StartTime, float EndTime)
{
#if FILTH_DEBUG
	return (EndTime - StartTime) / 1000.f;
#else
	return 0.0f;
#endif
}

void InitGamePaths(void)
{
	char* BaseGamePath = SDL_GetBasePath();

	if (BaseGamePath)
	{
		GamePath = BaseGamePath;
	}
	else
	{
		GamePath = strdup("./");
	}

	char GameDataPath[1024];
	strcpy(GameDataPath, GamePath);
	strcat(GameDataPath, "data/");
	DataPath = GameDataPath;
}

void InitLogFile(void)
{
	char LogFilePath[1024];
	strcpy(LogFilePath, GamePath);
	strcat(LogFilePath, LogFileName);
	LogPath = LogFilePath;

	LogFile = fopen(LogPath.c_str(), "w");

	if (LogFile)
	{
		char* LogText = "";

		fputs(LogText, LogFile);

		fclose(LogFile);
	}
}

char* ReadTextFile(const char* FileName)
{
	char* OutString;
	char FilePath[1024];
	strcpy(FilePath, GamePath);
	strcat(FilePath, FileName);

	FILE* File = fopen(FilePath, "rt");

	if (File)
	{
		OutString = (char*)malloc(sizeof(char) * MAX_TEXT_FILE_LENGTH);
		int i = 0;
		char c;

		while((c = fgetc(File)) != EOF && i < MAX_TEXT_FILE_LENGTH - 1)
		{
			OutString[i] = c;
			i++;
		}

		OutString[i] = '\0';

		fclose(File);
	}
	else
	{
		WriteLog("Unable to open text file: %s", FilePath);
		return NULL;
	}

	return OutString;
}

void WriteLog(const char* fmt, ...)
{
	LogFile = fopen(LogPath.c_str(), "a");

	if (LogFile)
	{
		char* LogText;

		LogText = (char*)malloc(sizeof(char) * MAX_TEXT_FILE_LENGTH);
		int i = 0;
		char c;

		while ((c = fgetc(LogFile)) != EOF && i < MAX_TEXT_FILE_LENGTH - 1)
		{
			LogText[i] = c;
			i++;
		}

		LogText[i] = '\0';

		char Msg[MAX_MSG_LENGTH];
		va_list ap;
		
		if (fmt == NULL)
		{
			return;
		}

		va_start(ap, fmt);
		vsprintf(Msg, fmt, ap);
		va_end(ap);

		string NewLogText = *LogText + Msg;

		fputs(NewLogText.c_str(), LogFile);

		printf(Msg);

		fclose(LogFile);
	}
}

// adapted from https://gist.github.com/mortennobel/4648058
bool CheckGLError(const char *file, int line)
{
#if FILTH_DEBUG || FILTH_EDITOR
	GLenum GLErr = glGetError();

	while (GLErr != GL_NO_ERROR)
	{
		string ErrorString;

		switch (GLErr)
		{
			case GL_INVALID_OPERATION: ErrorString = "INVALID_OPERATION"; break;
			case GL_INVALID_ENUM: ErrorString = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE: ErrorString = "INVALID_VALUE"; break;
			case GL_OUT_OF_MEMORY: ErrorString = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: ErrorString = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}

		WriteLog("GL_%s - %s:%d\n", ErrorString.c_str(), file, line);
		GLErr = glGetError();
	}

	return (GLErr == GL_NO_ERROR);
#else
	return true;
#endif
}