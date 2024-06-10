#include "camera.h"
#include "debug.h"
#include "render.h"

const static float MoveSpeed = 5.0f;
const static float LookSpeed = 0.15f;

Camera::Camera(SDL_Window* Window, int32 Width, int32 Height)
{
	WindowWidth = Width;
	WindowHeight = Height;

	Pos = Vector3(0.0f, 0.0f, 0.0f);

	Target = Vector3(0.0f, 0.0f, 1.0f);
	Target.Normalize();

	Up = Vector3(0.0f, 1.0f, 0.0f);

	bIsMovingForward = false;
	bIsMovingBackward = false;
	bIsMovingLeft = false;
	bIsMovingRight = false;

	bFirstMouse = true;
}

Camera::Camera(SDL_Window* Window, int32 Width, int32 Height, const Vector3& _Pos, const Vector3& _Target, const Vector3& _Up)
{
	WindowWidth = Width;
	WindowHeight = Height;
	
	Pos = _Pos;
	
	Target = _Target;
	Target.Normalize();

	Up = _Up;
	Up.Normalize();

	bIsMovingForward = false;
	bIsMovingBackward = false;
	bIsMovingLeft = false;
	bIsMovingRight = false;

	bFirstMouse = true;
}

void Camera::Init(SDL_Window* Window)
{
	Vector3 HTarget(Target.x, 0.0f, Target.z);
	HTarget.Normalize();

	if (HTarget.z >= 0.0f)
	{
		if (HTarget.x >= 0.0f)
		{
			Yaw = 360.0f - ToDegree(asin(HTarget.z));
		}
		else
		{
			Yaw = 180.0f + ToDegree(asin(HTarget.z));
		}
	}
	else
	{
		if (HTarget.x >= 0.0f)
		{
			Yaw = ToDegree(asin(-HTarget.z));
		}
		else
		{
			Yaw = 180.0f - ToDegree(asin(-HTarget.z));
		}
	}

	Pitch = -ToDegree(asin(Target.y));
}

void Camera::Move(enum MoveState State, bool bIsMoving)
{
	switch (State)
	{
		case MOVE_FORWARD: bIsMovingForward = (bIsMoving && !GIsPaused); break;
		case MOVE_BACKWARD: bIsMovingBackward = (bIsMoving && !GIsPaused); break;
		case MOVE_LEFT: bIsMovingLeft = (bIsMoving && !GIsPaused); break;
		case MOVE_RIGHT: bIsMovingRight = (bIsMoving && !GIsPaused); break;
	}
}

// FIXME: LookPos.Y needs to be clamped too, otherwise it locks the camera at 90 degrees vertically!
void Camera::Look(int32 X, int32 Y)
{
	if (bFirstMouse)
	{
		LookPos.X = X;
		LookPos.Y = Y;
		bFirstMouse = false;
	}

	LookPos.X += X;
	LookPos.Y += Y;

	Yaw = LookPos.X * LookSpeed;
	Pitch = LookPos.Y * LookSpeed;

	if (Pitch < -90.f)
	{
		Pitch = -90.f;
	}
	else if (Pitch > 90.f)
	{
		Pitch = 90.f;
	}
}

void Camera::Update()
{
	float Speed = MoveSpeed * (GDeltaTime / 1000.f);

	if (bIsMovingForward)
	{
		Pos += Vector3(Target.x * Speed, Target.y * Speed, Target.z * Speed);
	}
	if (bIsMovingBackward)
	{
		Pos -= Vector3(Target.x * Speed, Target.y * Speed, Target.z * Speed);
	}
	if (bIsMovingLeft)
	{
		Vector3 Left = Target.Cross(Up);
		Left.Normalize();
		Left *= Speed;
		Pos += Left;
	}
	if (bIsMovingRight)
	{
		Vector3 Right = Up.Cross(Target);
		Right.Normalize();
		Right *= Speed;
		Pos += Right;
	}
	
	Vector3 VAxis(0.0f, 1.0f, 0.0f);

	Vector3 View(1.0f, 0.0f, 0.0f);
	View.Rotate(Yaw, VAxis);
	View.Normalize();

	Vector3 HAxis = VAxis.Cross(View);
	HAxis.Normalize();
	View.Rotate(Pitch, HAxis);

	Target = View;
	Target.Normalize();

	Up = Target.Cross(HAxis);
	Up.Normalize();
}

Vector2 Camera::WorldToScreenPoint(Vector3 WorldPoint)
{
	return Vector2::Zero; // TODO
}

void Camera::ScreenToWorldPoint(Vector2 ScreenPoint, Vector3 &OutOrigin, Vector3 &OutDir)
{
	Vector4 RayStart((ScreenPoint.X / WindowWidth - 0.5f) * 2.0f, (ScreenPoint.Y / WindowHeight - 0.5f) * 2.0f, -1.0f, 1.0f);
	Vector4 RayEnd((ScreenPoint.X / WindowWidth - 0.5f) * 2.0f, (ScreenPoint.Y / WindowHeight - 0.5f) * 2.0f, 0.0f, 1.0f);

	Matrix4 M = ViewProjectionMatrix();
	M.Inverse();

	Vector4 RayStartWorld = M * RayStart;
	RayStartWorld /= RayStartWorld.w;

	Vector4 RayEndWorld = M * RayEnd;
	RayEndWorld /= RayEndWorld.w;

	Vector3 RayDirWorld = Vector4(RayEndWorld - RayStartWorld).ToVector3();
	RayDirWorld.Normalize();

	OutOrigin = RayStartWorld.ToVector3();
	OutDir = RayDirWorld.Normalize();
}

Matrix4 Camera::ViewProjectionMatrix()
{
	Transform T;
	T.SetCamera(Pos, Target, Up);
	T.SetPerspectiveProj(GRenderer->PersProj);
	return T.GetVPTrans();
}