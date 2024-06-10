#include <string>
#include "shader.h"

Shader::Shader()
{
	ShaderProgram = 0;
}

Shader::~Shader()
{
	for (ShaderObjList::iterator it = ShaderObjs.begin(); it != ShaderObjs.end(); it++)
	{
		glDeleteShader(*it);
	}

	if (ShaderProgram != 0)
	{
		glDeleteProgram(ShaderProgram);
		ShaderProgram = 0;
	}
}

bool Shader::Init()
{
	ShaderProgram = glCreateProgram();

	if (ShaderProgram == 0)
	{
		WriteLog("Error creating shader program\n");
		return false;
	}

	return true;
}

void Shader::Bind()
{
	glUseProgram(ShaderProgram);
	bIsBound = true;
}

void Shader::Unbind()
{
	glUseProgram(0);
	bIsBound = false;
}

bool Shader::AddShader(GLenum ShaderType, const char* ShaderText)
{
	std::string ShaderString = ReadTextFile(ShaderText);
	if (!ShaderString.c_str())
	{
		WriteLog("Unable to read shader: %s\n", ShaderText);
		return false;
	}

	std::string Version = "#version 330\n";
	std::string FinalShader = Version + ShaderString;

	GLuint ShaderObj = glCreateShader(ShaderType);
	if (ShaderObj == 0)
	{
		WriteLog("Error creating shader type %d\n", ShaderType);
		return false;
	}

	ShaderObjs.push_back(ShaderObj);

	const GLchar* P[1] = { FinalShader.c_str() };
	GLint Lengths[1] = { (GLint)strlen(FinalShader.c_str()) };
	glShaderSource(ShaderObj, 1, P, Lengths);
	glCompileShader(ShaderObj);

	GLint Success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &Success);
	if (!Success)
	{
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, sizeof(InfoLog), NULL, InfoLog);
		WriteLog("Error compiling shader type %d (file %s): '%s'\n", ShaderType, ShaderText, InfoLog);
		return false;
	}

	glAttachShader(ShaderProgram, ShaderObj);

	return true;
}

bool Shader::Finalize()
{
	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glLinkProgram(ShaderProgram);

	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (!Success)
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		WriteLog("Error linking Shader program: %s\n", ErrorLog);
		return false;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success)
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		WriteLog("Error linking Shader program: %s\n", ErrorLog);
		return false;
	}

	for (ShaderObjList::iterator it = ShaderObjs.begin(); it != ShaderObjs.end(); it++)
	{
		glDeleteShader(*it);
	}

	ShaderObjs.clear();

	return GLCheckError();
}

GLint Shader::GetUniformLocation(char* UniformName)
{
	GLuint Location = glGetUniformLocation(ShaderProgram, UniformName);

	if (Location == INVALID)
	{
		WriteLog("Unable to get the location of uniform: %s\n", UniformName);
	}

	return Location;
}