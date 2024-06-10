#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <math.h>
#include <assimp/vector3.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include "common.h"

#define Pi (3.141592654f)
#define Epsilon (1.e-8f)
#define ToRadian(x) (float)(((x) * Pi / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / Pi))

static bool IsNaN(float A)
{
	return isnan(A) != 0;
}

inline float RandomFloat()
{
	return ((float)rand() / RAND_MAX);
}

inline float Clamp(float Val, float Min, float Max)
{
	if (Val < Min)
	{
		Val = Min;
	}
	else if (Val > Max)
	{
		Val = Max;
	}

	return Val;
}

inline float Lerp(float A, float B, float T)
{
	return A * (1.0f - T) + (B * T); 
}

inline float Square(float N)
{
	return N*N;
}

inline float InterpTo(float Current, float Target, float DeltaTime, float Speed)
{
	if (Speed <= 0.0f)
	{
		return Target;
	}

	float Dist = Target - Current;

	if (Square(Dist) < Epsilon)
	{
		return Target;
	}

	float DeltaMove = Dist * Clamp(DeltaTime * Speed, 0.0f, 1.0f);

	return Current + DeltaMove;
}

inline float MinFloat(float A, float B)
{
	return (A <= B) ? A : B;
}

inline float MaxFloat(float A, float B)
{
	return (A >= B) ? A : B;
}

struct PerspectiveProj
{
    float FOV;
    float Width; 
    float Height;
    float zNear;
    float zFar;
};

class Vector2
{
public:

	float X;
	float Y;

	Vector2() { }

	Vector2(float x, float y)
	{
		X = x;
		Y = y;
	}

	Vector2(const float* f)
	{
		X = f[0];
		Y = f[0];
	}

	Vector2(float f)
	{
		X = Y = f;
	}

	inline Vector2 operator-(const Vector2& v)
	{
		return Vector2(X - v.X, Y - v.Y);
	}

	static const Vector2 Zero;
};

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3()
	{
		x = y = z = 0.0f;
	}

	Vector3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3(const float* pFloat)
	{
		x = pFloat[0];
		y = pFloat[0];
		z = pFloat[0];
	}

	Vector3(float f)
	{
		x = y = z = f;
	}

	Vector3& operator+=(const Vector3& r)
	{
		x += r.x;
		y += r.y;
		z += r.z;

		return *this;
	}

	Vector3& operator-=(const Vector3& r)
	{
		x -= r.x;
		y -= r.y;
		z -= r.z;

		return *this;
	}

	Vector3& operator*=(float f)
	{
		x *= f;
		y *= f;
		z *= f;

		return *this;
	}

	Vector3& operator*=(const Vector3& V)
	{
		x *= V.x;
		y *= V.y;
		z *= V.z;

		return *this;
	}

	operator const float*() const
	{
		return &(x);
	}

	inline Vector3 operator+(const Vector3& v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);;
	}

	inline Vector3 operator-(const Vector3& v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	inline Vector3 operator*(float f)
	{
		return Vector3(x * f, y * f, z * f);
	}

	inline Vector3 operator*(const Vector3& v)
	{
		return Vector3(x * v.x, y * v.y, z * v.z);
	}

	inline Vector3 operator/(const Vector3& v)
	{
		return Vector3(x / v.x, y / v.y, z / v.z);
	}

	Vector3 Cross(const Vector3& v) const;

	Vector3& Normalize();

	void Rotate(float Angle, const Vector3& Axis);

	inline void Print(bool bWriteToLogFile = false) const
	{
#if FILTH_DEBUG
		if (!bWriteToLogFile)
		{
			printf("(%.02f, %.02f, %.02f)\n", x, y, z);
		}
		else
		{
			WriteLog("(%.02f, %.02f, %.02f)\n", x, y, z);
		}
#endif
	}

	inline bool IsZero()
	{
		return ((x*x) <= Epsilon) && ((y*y) <= Epsilon) && ((z*z) <= Epsilon);
	}

	inline Vector3 Inverse()
	{
		return Vector3(-x, -y, -z);
	}

	static const Vector3 Infinity;
	static const Vector3 NegInfinity;
	static const Vector3 Zero;
};

inline Vector3 operator*(float lhs, Vector3 rhs) { return rhs * lhs; }

inline Vector3 RotateVector(Vector3 In, float Angle, const Vector3& Axis)
{
	In.Rotate(Angle, Axis);
	return In;
}

struct Vector4
{
	float x;
	float y;
	float z;
	float w;

	Vector4()
	{
	}

	Vector4(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	inline Vector4 operator/(float f)
	{
		return Vector4(x / f, y / f, z / f, w / f);
	}

	inline Vector4 operator/=(float f)
	{
		x /= f;
		y /= f;
		z /= f;
		w /= f;

		return *this;
	}

	inline Vector4 operator-(Vector4 V)
	{
		return Vector4(x - V.x, y - V.y, z - V.z, w - V.w);
	}

	void Print(bool bWriteToLogFile = false) const
	{
#if FILTH_DEBUG
		if (!bWriteToLogFile)
		{
			printf("(%.02f, %.02f, %.02f, %.02f)\n", x, y, z, w);
		}
		else
		{
			WriteLog("(%.02f, %.02f, %.02f, %.02f)\n", x, y, z, w);
		}
#endif
	}

	Vector3 ToVector3() const
	{
		return Vector3(x, y, z);
	}
};

struct PersProjInfo
{
	float FOV;
	float Width;
	float Height;
	float zNear;
	float zFar;
};


struct OrthoProjInfo
{
	float r;        // right
	float l;        // left
	float b;        // bottom
	float t;        // top
	float n;        // z near
	float f;        // z far
};

struct Quaternion
{
	float x, y, z, w;

	Quaternion(float _x, float _y, float _z, float _w);

	void Normalize();

	Quaternion Conjugate();

	Vector3 ToDegrees();

	Quaternion operator*(const Quaternion& q);

	Quaternion operator*(const Vector3& v);

	inline void Print(bool bWriteToLogFile = false)
	{
#if FILTH_DEBUG
		if (!bWriteToLogFile)
		{
			printf("(%.02f, %.02f, %.02f, %.02f)\n", x, y, z, w);
		}
		else
		{
			WriteLog("(%.02f, %.02f, %.02f, %.02f)\n", x, y, z, w);
		}
#endif
	}
};


struct Matrix4
{
	float m[4][4];

	Matrix4() {}

	Matrix4(const aiMatrix4x4& AssimpMatrix)
	{
		m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = AssimpMatrix.a4;
		m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = AssimpMatrix.b4;
		m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = AssimpMatrix.c4;
		m[3][0] = AssimpMatrix.d1; m[3][1] = AssimpMatrix.d2; m[3][2] = AssimpMatrix.d3; m[3][3] = AssimpMatrix.d4;
	}

	Matrix4(const aiMatrix3x3& AssimpMatrix)
	{
		m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = 0.0f;
		m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = 0.0f;
		m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	Matrix4(float a00, float a01, float a02, float a03,
		float a10, float a11, float a12, float a13,
		float a20, float a21, float a22, float a23,
		float a30, float a31, float a32, float a33)
	{
		m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03;
		m[1][0] = a10; m[1][1] = a11; m[1][2] = a12; m[1][3] = a13;
		m[2][0] = a20; m[2][1] = a21; m[2][2] = a22; m[2][3] = a23;
		m[3][0] = a30; m[3][1] = a31; m[3][2] = a32; m[3][3] = a33;
	}

	inline void SetZero()
	{
		ZERO_MEM(m);
	}

	inline Matrix4 Transpose() const
	{
		Matrix4 n;

		for (unsigned int i = 0; i < 4; i++)
		{
			for (unsigned int j = 0; j < 4; j++)
			{
				n.m[i][j] = m[j][i];
			}
		}

		return n;
	}


	inline void InitIdentity()
	{
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	inline Matrix4 operator*(const Matrix4& Right) const
	{
		Matrix4 Ret;

		for (unsigned int i = 0; i < 4; i++)
		{
			for (unsigned int j = 0; j < 4; j++)
			{
				Ret.m[i][j] = m[i][0] * Right.m[0][j] +
					m[i][1] * Right.m[1][j] +
					m[i][2] * Right.m[2][j] +
					m[i][3] * Right.m[3][j];
			}
		}

		return Ret;
	}

	inline Vector4 operator*(const Vector4& v) const
	{
		Vector4 r;

		r.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
		r.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
		r.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
		r.w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;

		return r;
	}

	inline Vector3 operator*(Vector3& v) const
	{
		Vector4 r;
		
		r.x = (v.x * m[0][0]) + (v.y * m[1][0]) + (v.z * m[2][0]) + m[3][0];
		r.y = (v.x * m[0][1]) + (v.y * m[1][1]) + (v.z * m[2][1]) + m[3][1];
		r.z = (v.x * m[0][2]) + (v.y * m[1][2]) + (v.z * m[2][2]) + m[3][2];
		r.w = 1 / ((v.x * m[0][3]) + (v.y * m[1][3]) + (v.z * m[2][3]) + m[3][3]);

		return Vector3(r.x * r.w, r.y * r.w, r.z * r.w);
	}

	operator const float*() const
	{
		return &(m[0][0]);
	}

	inline void Print(bool bWriteToLogFile = false) const
	{
#if FILTH_DEBUG
		for (int i = 0; i < 4; i++)
		{
			if (!bWriteToLogFile)
			{
				printf("%f %f %f %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
			}
			else
			{
				WriteLog("%f %f %f %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
			}
		}
#endif
	}

	inline Vector4 GetColumn(uint32 Column)
	{
		Vector4 Ret;
		Ret.x = m[0][Column];
		Ret.y = m[1][Column];
		Ret.z = m[2][Column];
		Ret.w = m[3][Column];
		return Ret;
	}

	float Determinant() const;

	Matrix4& Inverse();

	void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
	void InitRotateTransform(float RotateX, float RotateY, float RotateZ);
	void InitRotateTransform(const Quaternion& quat);
	void InitTranslationTransform(float x, float y, float z);
	void InitCameraTransform(const Vector3& Target, const Vector3& Up);
	void InitPersProjTransform(const PersProjInfo& p);
	void InitOrthoProjTransform(const OrthoProjInfo& p);
};

inline Vector3 operator*(Vector3& lhs, Matrix4& rhs) { return rhs * lhs; }

struct Transform
{
	Transform()
	{
		Scale = Vector3(1.0f, 1.0f, 1.0f);
		WorldPos = Vector3(0.0f, 0.0f, 0.0f);
		Rotation = Vector3(0.0f, 0.0f, 0.0f);
	}

	void SetWorldPos(float PosX, float PosY, float PosZ)
	{
		WorldPos = Vector3(PosX, PosY, PosZ);
	}

	void SetWorldPos(Vector3& Pos)
	{
		WorldPos = Pos;
	}

	void SetRotation(float RotateX, float RotateY, float RotateZ)
	{
		Rotation.x = RotateX;
		Rotation.y = RotateY;
		Rotation.z = RotateZ;
	}

	void SetRotation(Vector3& r)
	{
		SetRotation(r.x, r.y, r.z);
	}

	void SetScale(float ScaleX, float ScaleY, float ScaleZ)
	{
		Scale.x = ScaleX;
		Scale.y = ScaleY;
		Scale.z = ScaleZ;
	}

	void SetScale(Vector3& S)
	{
		Scale = S;
	}

	void SetPerspectiveProj(PersProjInfo& p)
	{
		PersProj = p;
	}

	void SetOrthographicProj(OrthoProjInfo& p)
	{
		OrthoProj = p;
	}

	void SetCamera(Vector3& Pos, Vector3& Target, Vector3& Up)
	{
		camera.Pos = Pos;
		camera.Target = Target;
		camera.Up = Up;
	}

	const Matrix4& GetWPTrans();
	const Matrix4& GetWVTrans();
	const Matrix4& GetVPTrans();
	const Matrix4& GetWVPTrans();
	const Matrix4& GetWVOrthoPTrans();
	const Matrix4& GetWorldTrans();
	const Matrix4& GetViewTrans();
	const Matrix4& GetProjTrans();

	Vector3 Scale;
	Vector3 WorldPos;
	Vector3 Rotation;

	PersProjInfo PersProj;
	OrthoProjInfo OrthoProj;

	struct {
		Vector3 Pos;
		Vector3 Target;
		Vector3 Up;
	} camera;

	Matrix4 WVPTrans;
	Matrix4 VPTrans;
	Matrix4 WPTrans;
	Matrix4 WVTrans;
	Matrix4 WorldTrans;
	Matrix4 ViewTrans;
	Matrix4 ProjTrans;
};

struct AABB
{
	Vector3 Min;
	Vector3 Max;

	AABB()
	{
		Min = Vector3::Zero;
		Max = Vector3::Zero;
	}

	AABB(Vector3 V)
	{
		Min = Vector3(-V.x, -V.y, -V.z);
		Max = V;
	}

	AABB(Vector3 _Min, Vector3 _Max)
	{
		Min = _Min;
		Max = _Max;
	}

	inline bool IsZero()
	{
		if ((Min == Vector3::Zero) || (Max == Vector3::Zero))
		{
			return true;
		}

		return false;
	}

	inline Vector3 GetCenter()
	{
		return (Max + Min) * 0.5f;
	}

	inline Vector3 GetSize()
	{
		return Max - Min;
	}

	inline Vector3 GetExtents()
	{
		return (Max - Min) * 0.5f;
	}

	void ComputeFromVertices(std::vector<Vector3>& Vertices);
	bool IsPointInside(Vector3 Point);
	bool IsAABBInside(AABB Box);
	bool IsIntersecting(AABB Box);
	AABB Transform(Matrix4 T);
};

inline Vector3 VectorFromString(std::string S, char* Delimiter)
{
	using namespace std;
	vector<string> Tokens;
	string Token;
	istringstream TokenStream = istringstream(S);
	
	while (getline(TokenStream, Token, *Delimiter))
	{
		Tokens.push_back(Token);
	}

	Vector3 V = Vector3(stof(Tokens[0]), stof(Tokens[1]), stof(Tokens[2]));
	return V;
}

float RaycastAABB(Vector3 Start, Vector3 End, AABB Box);