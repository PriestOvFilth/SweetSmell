#pragma once

#include <vector>
#include <string>
#include <GL/glew.h>
#include "common/common.h"
#include "common/math3d.h"
#include "shader.h"

#if FILTH_DEBUG
#define GLT_DEBUG
#define GLT_DEBUG_PRINT
#endif
#include <gltext.h>
#include <debug_draw.hpp>

struct DebugText
{
	GLTtext* TextObj;
	std::string String;
	uint32 Frames;
};

class DebugLinePointShader : public Shader
{
public:

	virtual bool Init();

	void SetWVP(Matrix4& WVP);

	GLuint WVPLoc;
};

class DDRenderInterfaceGL : public dd::RenderInterface
{
public:

	DDRenderInterfaceGL();
	~DDRenderInterfaceGL();

	virtual void drawPointList(const dd::DrawVertex * points, int count, bool depthEnabled) override;
	virtual void drawLineList(const dd::DrawVertex * lines, int count, bool depthEnabled) override;

	class DebugLinePointShader* Shader;

	GLuint LinePointVAO;
	GLuint LinePointVBO;

	Matrix4 WVP;
};

class DebugRenderer
{
public:

	std::vector<struct DebugText> TextToRender;
	class DDRenderInterfaceGL LinePointRenderer;

	void Init();

	// the Frames parameter is the number of frames to display the text
	// if you want to display it for a number of seconds just do 60*seconds
	// this is lazy and bad cause it's framerate-dependent, but it doesn't matter...for now
	void AddText(bool PrintToLog, uint32 Frames, char* Text, ...);

	void AddPoint(Vector3 Position, Vector3 Color, int32 DurationMS);
	void AddLine(Vector3 From, Vector3 To, Vector3 Color, int32 DurationMS);
	void AddArrow(Vector3 From, Vector3 To, Vector3 Color, float Size);
	void AddCross(Vector3 Center, float Length);
	void AddCircle(Vector3 Center, Vector3 PlaneNormal, Vector3 Color, float Radius, float NumSteps);
	void AddPlane(Vector3 Center, Vector3 PlaneNormal, Vector3 PlaneColor, Vector3 NormalVecColor, float PlaneScale, float NormalVecScale);
	void AddSphere(Vector3 Center, Vector3 Color, float Radius);
	void AddCone(Vector3 Apex, Vector3 Direction, Vector3 Color, float BaseRadius, float ApexRadius);
	void AddBox(Vector3 Center, Vector3 Color, float Width, float Height, float Depth);
	void AddAABB(Vector3 Min, Vector3 Max, Vector3 Color);
	
	void DrawText();
	void DrawShapes();
	
	void Destroy();
};