#pragma once

#include <SDL.h>
#include "common/math3d.h"
#include "common/common.h"

enum MoveState
{
	MOVE_FORWARD,
	MOVE_BACKWARD,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_NONE
};

struct LookVector
{
	int32 X;
	int32 Y;
};

class Camera
{
public:

	Camera(SDL_Window* Window, int32 Width, int32 Height);

	Camera(SDL_Window* Window, int32 Width, int32 Height, const Vector3& _Pos, const Vector3& _Target, const Vector3& _Up);

	void Init(SDL_Window* Window);

	void Move(enum MoveState State, bool bIsMoving);

	void Look(int32 X, int32 Y);

	void Update(void);

	Vector2 WorldToScreenPoint(Vector3 WorldPoint);
	void ScreenToWorldPoint(Vector2 ScreenPoint, Vector3 &OutOrigin, Vector3 &OutDir);

	Matrix4 ViewProjectionMatrix();

	Vector3 Pos;
	Vector3 Target;
	Vector3 Up;

	int32 WindowWidth;
	int32 WindowHeight;

	float Yaw;
	float Pitch;

	LookVector LookPos;

	bool bFirstMouse;

	bool bIsMovingForward;
	bool bIsMovingBackward;
	bool bIsMovingLeft;
	bool bIsMovingRight;
};