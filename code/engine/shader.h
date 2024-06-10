#pragma once

#include <GL/glew.h>
#include <list>
#include "common/common.h"

class Shader
{
public:

	Shader();

	~Shader();

	virtual bool Init();

	void Bind();

	void Unbind();

	bool AddShader(GLenum ShaderType, const char* ShaderText);

	bool Finalize();

	GLint GetUniformLocation(char* UniformName);

	GLuint ShaderProgram;

	typedef std::list<GLuint> ShaderObjList;
	ShaderObjList ShaderObjs;

	bool bIsBound;
};