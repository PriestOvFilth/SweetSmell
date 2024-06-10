#include "posteffect.h"

bool PostEffectShader::Init()
{
	if (!Shader::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, VertShaderFile))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, FragShaderFile))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	TexLoc = GetUniformLocation("Tex");

	if (TexLoc == INVALID)
	{
		return false;
	}

	return true;
}

void PostEffectShader::SetTextureUnit(uint32 TextureUnit)
{
	glUniform1i(TexLoc, TextureUnit);
}

PixelizeShader::PixelizeShader()
{
	VertShaderFile = "data/shaders/PostEffectPassthrough.vert";
	FragShaderFile = "data/shaders/PixelizePostEffect.frag";
}

bool PixelizeShader::Init()
{
	if (!Shader::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, VertShaderFile))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, FragShaderFile))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	TexLoc = GetUniformLocation("Tex");
	ViewportSizeLoc = GetUniformLocation("ViewportSize");
	PixelSizeLoc = GetUniformLocation("PixelSize");

	if (TexLoc == INVALID)
	{
		return false;
	}

	return true;
}

void PixelizeShader::SetViewportSize(Vector2 ViewportSize)
{
	glUniform2f(ViewportSizeLoc, ViewportSize.X, ViewportSize.Y);
}

void PixelizeShader::SetPixelSize(float PixelSize)
{
	glUniform1f(PixelSizeLoc, PixelSize);
}

DitherShader::DitherShader()
{
	VertShaderFile = "data/shaders/PostEffectPassthrough.vert";
	FragShaderFile = "data/shaders/DitherPostEffect.frag";
}

LoadingScreenShader::LoadingScreenShader()
{
	VertShaderFile = "data/shaders/LoadingScreen.vert";
	FragShaderFile = "data/shaders/LoadingScreen.frag";
}

FullscreenQuad::FullscreenQuad()
{
	Points.push_back(Vector2(-1.0, -1.0));
	Points.push_back(Vector2(1.0, -1.0));
	Points.push_back(Vector2(1.0, 1.0));
	Points.push_back(Vector2(1.0, 1.0));
	Points.push_back(Vector2(-1.0, 1.0));
	Points.push_back(Vector2(-1.0, -1.0));

	TexCoords.push_back(Vector2(0.0, 0.0));
	TexCoords.push_back(Vector2(1.0, 0.0));
	TexCoords.push_back(Vector2(1.0, 1.0));
	TexCoords.push_back(Vector2(1.0, 1.0));
	TexCoords.push_back(Vector2(0.0, 1.0));
	TexCoords.push_back(Vector2(0.0, 0.0));
}

bool FullscreenQuad::Init()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	VertexArraySize = (uint32)Points.size() * sizeof(Vector2);
	TexArraySize = (uint32)TexCoords.size() * sizeof(Vector2);

	glGenBuffers(1, &BO);
	glBindBuffer(GL_ARRAY_BUFFER, BO);
	glBufferData(GL_ARRAY_BUFFER, VertexArraySize + TexArraySize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, VertexArraySize, (const GLvoid*)(&Points[0]));
	glBufferSubData(GL_ARRAY_BUFFER, VertexArraySize, TexArraySize, (const GLvoid*)(&TexCoords[0]));

	glEnableVertexAttribArray(0); // position
	glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1); // texcoord
	glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(VertexArraySize));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return GLCheckError();
}

void FullscreenQuad::Draw(uint32 FBTex)
{
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FBTex);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}