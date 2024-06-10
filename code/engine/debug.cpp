#include <stdarg.h>

#define DEBUG_DRAW_IMPLEMENTATION
#include "debug.h"
#include "game/scene.h"
#include "game/entity.h"
#include "game/component.h"
#include "camera.h"
#include "render.h"

bool DebugLinePointShader::Init()
{
	if (!Shader::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "data/shaders/DebugLinePoint.vert"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, "data/shaders/DebugLinePoint.frag"))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	WVPLoc = GetUniformLocation("WVP");

	if (WVPLoc == INVALID)
	{
		return false;
	}

	return true;
}

void DebugLinePointShader::SetWVP(Matrix4& WVP)
{
	glUniformMatrix4fv(WVPLoc, 1, GL_TRUE, (const GLfloat*)WVP.m);
}

DDRenderInterfaceGL::DDRenderInterfaceGL()
{
	glDisable(GL_BLEND);
	glEnable(GL_PROGRAM_POINT_SIZE);

	Shader = new DebugLinePointShader();
	if (!Shader->Init())
	{
		WriteLog("Unable to init debug shader\n");
	}

	glGenVertexArrays(1, &LinePointVAO);
	glGenBuffers(1, &LinePointVBO);
	GLCheckError();

	glBindVertexArray(LinePointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, LinePointVBO);

	// RenderInterface will never be called with a batch larger than
	// DEBUG_DRAW_VERTEX_BUFFER_SIZE vertexes, so we can allocate the same amount here.
	glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
	GLCheckError();

	// Set the vertex format expected by 3D points and lines:
	std::size_t offset = 0;

	glEnableVertexAttribArray(0); // Position (vec3)
	glVertexAttribPointer(
		/* index     = */ 0,
		/* size      = */ 3,
		/* type      = */ GL_FLOAT,
		/* normalize = */ GL_FALSE,
		/* stride    = */ sizeof(dd::DrawVertex),
		/* offset    = */ reinterpret_cast<void *>(offset));
	offset += sizeof(float) * 3;

	glEnableVertexAttribArray(1); // ColorPointSize (vec4)
	glVertexAttribPointer(
		/* index     = */ 1,
		/* size      = */ 4,
		/* type      = */ GL_FLOAT,
		/* normalize = */ GL_FALSE,
		/* stride    = */ sizeof(dd::DrawVertex),
		/* offset    = */ reinterpret_cast<void *>(offset));

	GLCheckError();

	// VAOs can be a pain in the neck if left enabled...
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLCheckError();
}

DDRenderInterfaceGL::~DDRenderInterfaceGL()
{
	SAFE_DELETE(Shader);

	glDeleteVertexArrays(1, &LinePointVAO);
	glDeleteBuffers(1, &LinePointVBO);
}

void DDRenderInterfaceGL::drawPointList(const dd::DrawVertex * points, int count, bool depthEnabled)
{
	FILTH_ASSERT(points != nullptr);
	FILTH_ASSERT(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

	glBindVertexArray(LinePointVAO);
	Shader->Bind();

	Shader->SetWVP(WVP);

	if (depthEnabled)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	// NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
	glBindBuffer(GL_ARRAY_BUFFER, LinePointVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), points);

	// Issue the draw call:
	glDrawArrays(GL_POINTS, 0, count);

	Shader->Unbind();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLCheckError();
}

void DDRenderInterfaceGL::drawLineList(const dd::DrawVertex * lines, int count, bool depthEnabled)
{
	FILTH_ASSERT(lines != nullptr);
	FILTH_ASSERT(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

	glBindVertexArray(LinePointVAO);
	Shader->Bind();

	Shader->SetWVP(WVP);

	if (depthEnabled)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	// NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
	glBindBuffer(GL_ARRAY_BUFFER, LinePointVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), lines);

	// Issue the draw call:
	glDrawArrays(GL_LINES, 0, count);

	Shader->Unbind();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLCheckError();
}

void DebugRenderer::Init()
{
#if FILTH_DEBUG
	gltInit();
	dd::initialize(&LinePointRenderer);

#endif
}

void DebugRenderer::AddText(bool PrintToLog, uint32 Frames, char *Text, ...)
{
#if FILTH_DEBUG
	if (GFrame > 0)
	{
		char Buffer[999];
		va_list va;
		va_start(va, Text);
		vsprintf(Buffer, Text, va);
		va_end(va);

		DebugText Temp;
		Temp.TextObj = gltCreateText();
		Temp.String = Buffer;
		Temp.Frames = Frames;
		TextToRender.push_back(Temp);

		if (PrintToLog)
		{
			WriteLog(Buffer);
		}
	}
#endif
}

void DebugRenderer::AddPoint(Vector3 Position, Vector3 Color, int32 DurationMS)
{
	ddVec3 Pos = { Position.x, Position.y, Position.z };
	ddVec3 Col = { Color.x, Color.y, Color.z };
	dd::point(Pos, Col, 5.f, DurationMS);
}

void DebugRenderer::AddLine(Vector3 From, Vector3 To, Vector3 Color, int32 DurationMS)
{
	ddVec3 _From = { From.x, From.y, From.z };
	ddVec3 _To = { To.x, To.y, To.z };
	ddVec3 Col = { Color.x, Color.y, Color.z };
	dd::line(_From, _To, Col, DurationMS);
}

void DebugRenderer::AddArrow(Vector3 From, Vector3 To, Vector3 Color, float Size)
{
	ddVec3 _From = { From.x, From.y, From.z };
	ddVec3 _To = { To.x, To.y, To.z };
	ddVec3 Col = { Color.x, Color.y, Color.z };
	dd::arrow(_From, _To, Col, Size);
}

void DebugRenderer::AddCross(Vector3 Center, float Length)
{
	ddVec3 _Center = { Center.x, Center.y, Center.z };
	dd::cross(_Center, Length);
}

void DebugRenderer::AddCircle(Vector3 Center, Vector3 PlaneNormal, Vector3 Color, float Radius, float NumSteps)
{
	ddVec3 _Center = { Center.x, Center.y, Center.z };
	ddVec3 _PlaneNormal = { PlaneNormal.x, PlaneNormal.y, PlaneNormal.z };
	ddVec3 Col = { Color.x, Color.y, Color.z };
	dd::circle(_Center, _PlaneNormal, Col, Radius, NumSteps);
}

void DebugRenderer::AddPlane(Vector3 Center, Vector3 PlaneNormal, Vector3 PlaneColor, Vector3 NormalVecColor, float PlaneScale, float NormalVecScale)
{
	ddVec3 _Center = { Center.x, Center.y, Center.z };
	ddVec3 _PlaneNormal = { PlaneNormal.x, PlaneNormal.y, PlaneNormal.z };
	ddVec3 PlaneCol = { PlaneColor.x, PlaneColor.y, PlaneColor.z };
	ddVec3 NormalVecCol = { NormalVecColor.x, NormalVecColor.y, NormalVecColor.z };
	dd::plane(_Center, _PlaneNormal, PlaneCol, NormalVecCol, PlaneScale, NormalVecScale);
}

void DebugRenderer::AddSphere(Vector3 Center, Vector3 Color, float Radius)
{
	ddVec3 _Center = { Center.x, Center.y, Center.z };
	ddVec3 Col = { Color.x, Color.y, Color.z };
	dd::sphere(_Center, Col, Radius);
}

void DebugRenderer::AddCone(Vector3 Apex, Vector3 Direction, Vector3 Color, float BaseRadius, float ApexRadius)
{
	ddVec3 _Apex = { Apex.x, Apex.y, Apex.z };
	ddVec3 Dir = { Direction.x, Direction.y, Direction.z };
	ddVec3 Col = { Color.x, Color.y, Color.z };
	dd::cone(_Apex, Dir, Col, BaseRadius, ApexRadius);
}

void DebugRenderer::AddBox(Vector3 Center, Vector3 Color, float Width, float Height, float Depth)
{
	ddVec3 _Center = { Center.x, Center.y, Center.z };
	ddVec3 Col = { Color.x, Color.y, Color.z };
	dd::box(_Center, Col, Width, Height, Depth);
}

void DebugRenderer::AddAABB(Vector3 Min, Vector3 Max, Vector3 Color)
{
	ddVec3 _Min = { Min.x, Min.y, Min.z };
	ddVec3 _Max = { Max.x, Max.y, Max.z };
	ddVec3 Col = { Color.x, Color.y, Color.z };
	dd::aabb(_Min, _Max, Col);
}

void DebugRenderer::DrawText()
{
#if FILTH_DEBUG
	if (GFrame > 0)
	{
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		float PosY = 0;
		for (uint32 i = 0; i < TextToRender.size(); i++)
		{
			gltSetText(TextToRender[i].TextObj, TextToRender[i].String.c_str());
			gltColor(1.f, 1.f, 0.f, 1.f);
			gltDrawText2D(TextToRender[i].TextObj, 0, PosY, 1.f);
			TextToRender[i].Frames--;
			PosY += 20;
		}

		for (auto It = TextToRender.begin(); It != TextToRender.end();)
		{
			if (It._Ptr->Frames <= 0)
			{
				gltDeleteText(It._Ptr->TextObj);
				It._Ptr->String.clear();
				It = TextToRender.erase(It);
			}
			else
			{
				++It;
			}
		}

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
#endif
}

void DebugRenderer::DrawShapes()
{
#if FILTH_DEBUG
	Camera* Cam = GScene->GetPlayerCamera()->Cam;

	Transform T;
	T.SetCamera(Cam->Pos, Cam->Target, Cam->Up);
	T.SetPerspectiveProj(GRenderer->PersProj);
	LinePointRenderer.WVP = T.GetVPTrans();

	dd::flush((int64)GCurrentTime);
#endif
}

void DebugRenderer::Destroy()
{
#if FILTH_DEBUG
	for (uint32 i = 0; i < TextToRender.size(); i++)
	{
		gltDeleteText(TextToRender[i].TextObj);
	}
	
	gltTerminate();
	TextToRender.clear();

	dd::shutdown();
#endif
}