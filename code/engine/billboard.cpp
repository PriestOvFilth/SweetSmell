#include "billboard.h"
#include "texture.h"
#include "resource.h"

bool BillboardShader::Init()
{
	if (!Shader::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "data/shaders/Billboard.vert"))
	{
		return false;
	}

	if (!AddShader(GL_GEOMETRY_SHADER, "data/shaders/Billboard.geom"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, "data/shaders/Billboard.frag"))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	VPLoc = GetUniformLocation("VP");
	CamPosLoc = GetUniformLocation("CamPos");
	ColorMapLoc = GetUniformLocation("ColorMap");
	BillboardSizeLoc = GetUniformLocation("Size");

	if (VPLoc == INVALID ||
		CamPosLoc == INVALID ||
		ColorMapLoc == INVALID ||
		BillboardSizeLoc == INVALID)
	{
		return false;
	}

	return true;
}

void BillboardShader::SetVP(const Matrix4& VP)
{
	glUniformMatrix4fv(VPLoc, 1, GL_TRUE, (const GLfloat*)VP.m);
}

void BillboardShader::SetCameraPos(const Vector3& Pos)
{
	glUniform3f(CamPosLoc, Pos.x, Pos.y, Pos.z);
}

void BillboardShader::SetColorTextureUnit(uint32 TextureUnit)
{
	glUniform1i(ColorMapLoc, TextureUnit);
}

void BillboardShader::SetBillboardSize(float Size)
{
	glUniform1f(BillboardSizeLoc, Size);
}

Billboard::Billboard()
{
	Tex = NULL;
	VB = INVALID;
}

Billboard::~Billboard()
{
	SAFE_DELETE(Tex);

	if (VB != INVALID)
	{
		glDeleteBuffers(1, &VB);
	}
}

bool Billboard::Init(const char* FileName)
{
	Tex = GResourceMan->GetTexture(FileName);
	if (!Tex)
	{
		return false;
	}

	CreatePositionBuffer();

	Shader = new BillboardShader();
	if (!Shader->Init())
	{
		return false;
	}

	return true;
}

void Billboard::CreatePositionBuffer()
{
	Vector3 Pos(0.0f, 0.0f, 0.0f);
	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Pos), &Pos, GL_STATIC_DRAW);
}

void Billboard::Render(const Matrix4& VP, const Vector3 CamPos)
{
	Shader->Bind();
	Shader->SetVP(VP);
	Shader->SetCameraPos(CamPos);

	Tex->Bind(GL_TEXTURE0);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);

	glDrawArrays(GL_POINTS, 0, 10 * 10);

	glDisableVertexAttribArray(0);
}