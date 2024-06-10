#include "math3d.h"

const Vector3 Vector3::Infinity(INFINITY, INFINITY, INFINITY);
const Vector3 Vector3::NegInfinity(-INFINITY, -INFINITY, -INFINITY);
const Vector3 Vector3::Zero(0.0f, 0.0f, 0.0f);
const Vector2 Vector2::Zero(0.0f, 0.0f);

Vector3 Vector3::Cross(const Vector3& v) const
{
	const float _x = y * v.z - z * v.y;
	const float _y = z * v.x - x * v.z;
	const float _z = x * v.y - y * v.x;

	return Vector3(_x, _y, _z);
}

Vector3& Vector3::Normalize()
{
	const float Length = sqrtf(x * x + y * y + z * z);

	x /= Length;
	y /= Length;
	z /= Length;

	return *this;
}

void Vector3::Rotate(float Angle, const Vector3& Axe)
{
	const float SinHalfAngle = sinf(ToRadian(Angle / 2));
	const float CosHalfAngle = cosf(ToRadian(Angle / 2));

	const float Rx = Axe.x * SinHalfAngle;
	const float Ry = Axe.y * SinHalfAngle;
	const float Rz = Axe.z * SinHalfAngle;
	const float Rw = CosHalfAngle;
	Quaternion RotationQ(Rx, Ry, Rz, Rw);

	Quaternion ConjugateQ = RotationQ.Conjugate();
	//  ConjugateQ.Normalize();
	Quaternion W = RotationQ * (*this) * ConjugateQ;

	x = W.x;
	y = W.y;
	z = W.z;
}


void Matrix4::InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ)
{
	m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
	m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
	m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
	m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;
}

void Matrix4::InitRotateTransform(float RotateX, float RotateY, float RotateZ)
{
	Matrix4 rx, ry, rz;

	const float x = ToRadian(RotateX);
	const float y = ToRadian(RotateY);
	const float z = ToRadian(RotateZ);

	rx.m[0][0] = 1.0f; rx.m[0][1] = 0.0f; rx.m[0][2] = 0.0f; rx.m[0][3] = 0.0f;
	rx.m[1][0] = 0.0f; rx.m[1][1] = cosf(x); rx.m[1][2] = -sinf(x); rx.m[1][3] = 0.0f;
	rx.m[2][0] = 0.0f; rx.m[2][1] = sinf(x); rx.m[2][2] = cosf(x); rx.m[2][3] = 0.0f;
	rx.m[3][0] = 0.0f; rx.m[3][1] = 0.0f; rx.m[3][2] = 0.0f; rx.m[3][3] = 1.0f;

	ry.m[0][0] = cosf(y); ry.m[0][1] = 0.0f; ry.m[0][2] = -sinf(y); ry.m[0][3] = 0.0f;
	ry.m[1][0] = 0.0f; ry.m[1][1] = 1.0f; ry.m[1][2] = 0.0f; ry.m[1][3] = 0.0f;
	ry.m[2][0] = sinf(y); ry.m[2][1] = 0.0f; ry.m[2][2] = cosf(y); ry.m[2][3] = 0.0f;
	ry.m[3][0] = 0.0f; ry.m[3][1] = 0.0f; ry.m[3][2] = 0.0f; ry.m[3][3] = 1.0f;

	rz.m[0][0] = cosf(z); rz.m[0][1] = -sinf(z); rz.m[0][2] = 0.0f; rz.m[0][3] = 0.0f;
	rz.m[1][0] = sinf(z); rz.m[1][1] = cosf(z); rz.m[1][2] = 0.0f; rz.m[1][3] = 0.0f;
	rz.m[2][0] = 0.0f; rz.m[2][1] = 0.0f; rz.m[2][2] = 1.0f; rz.m[2][3] = 0.0f;
	rz.m[3][0] = 0.0f; rz.m[3][1] = 0.0f; rz.m[3][2] = 0.0f; rz.m[3][3] = 1.0f;

	*this = rz * ry * rx;
}


void Matrix4::InitRotateTransform(const Quaternion& quat)
{
	float yy2 = 2.0f * quat.y * quat.y;
	float xy2 = 2.0f * quat.x * quat.y;
	float xz2 = 2.0f * quat.x * quat.z;
	float yz2 = 2.0f * quat.y * quat.z;
	float zz2 = 2.0f * quat.z * quat.z;
	float wz2 = 2.0f * quat.w * quat.z;
	float wy2 = 2.0f * quat.w * quat.y;
	float wx2 = 2.0f * quat.w * quat.x;
	float xx2 = 2.0f * quat.x * quat.x;
	m[0][0] = -yy2 - zz2 + 1.0f;
	m[0][1] = xy2 + wz2;
	m[0][2] = xz2 - wy2;
	m[0][3] = 0;
	m[1][0] = xy2 - wz2;
	m[1][1] = -xx2 - zz2 + 1.0f;
	m[1][2] = yz2 + wx2;
	m[1][3] = 0;
	m[2][0] = xz2 + wy2;
	m[2][1] = yz2 - wx2;
	m[2][2] = -xx2 - yy2 + 1.0f;
	m[2][3] = 0.0f;
	m[3][0] = m[3][1] = m[3][2] = 0;
	m[3][3] = 1.0f;
}

void Matrix4::InitTranslationTransform(float x, float y, float z)
{
	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}


void Matrix4::InitCameraTransform(const Vector3& Target, const Vector3& Up)
{
	Vector3 N = Target;
	N.Normalize();
	Vector3 U = Up;
	U.Normalize();
	U = U.Cross(N);
	Vector3 V = N.Cross(U);

	m[0][0] = U.x;   m[0][1] = U.y;   m[0][2] = U.z;   m[0][3] = 0.0f;
	m[1][0] = V.x;   m[1][1] = V.y;   m[1][2] = V.z;   m[1][3] = 0.0f;
	m[2][0] = N.x;   m[2][1] = N.y;   m[2][2] = N.z;   m[2][3] = 0.0f;
	m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;
}

void Matrix4::InitPersProjTransform(const PersProjInfo& p)
{
	const float ar = p.Width / p.Height;
	const float zRange = p.zNear - p.zFar;
	const float tanHalfFOV = tanf(ToRadian(p.FOV / 2.0f));

	m[0][0] = 1.0f / (tanHalfFOV * ar); m[0][1] = 0.0f;            m[0][2] = 0.0f;            m[0][3] = 0.0;
	m[1][0] = 0.0f;                   m[1][1] = 1.0f / tanHalfFOV; m[1][2] = 0.0f;            m[1][3] = 0.0;
	m[2][0] = 0.0f;                   m[2][1] = 0.0f;            m[2][2] = (-p.zNear - p.zFar) / zRange; m[2][3] = 2.0f*p.zFar*p.zNear / zRange;
	m[3][0] = 0.0f;                   m[3][1] = 0.0f;            m[3][2] = 1.0f;            m[3][3] = 0.0;
}


void Matrix4::InitOrthoProjTransform(const OrthoProjInfo& p)
{
	float l = p.l;
	float r = p.r;
	float b = p.b;
	float t = p.t;
	float n = p.n;
	float f = p.f;

	m[0][0] = 2.0f / (r - l); m[0][1] = 0.0f;         m[0][2] = 0.0f;         m[0][3] = -(r + l) / (r - l);
	m[1][0] = 0.0f;         m[1][1] = 2.0f / (t - b); m[1][2] = 0.0f;         m[1][3] = -(t + b) / (t - b);
	m[2][0] = 0.0f;         m[2][1] = 0.0f;         m[2][2] = 2.0f / (f - n); m[2][3] = -(f + n) / (f - n);
	m[3][0] = 0.0f;         m[3][1] = 0.0f;         m[3][2] = 0.0f;         m[3][3] = 1.0;
}


float Matrix4::Determinant() const
{
	return m[0][0] * m[1][1] * m[2][2] * m[3][3] - m[0][0] * m[1][1] * m[2][3] * m[3][2] + m[0][0] * m[1][2] * m[2][3] * m[3][1] - m[0][0] * m[1][2] * m[2][1] * m[3][3]
		+ m[0][0] * m[1][3] * m[2][1] * m[3][2] - m[0][0] * m[1][3] * m[2][2] * m[3][1] - m[0][1] * m[1][2] * m[2][3] * m[3][0] + m[0][1] * m[1][2] * m[2][0] * m[3][3]
		- m[0][1] * m[1][3] * m[2][0] * m[3][2] + m[0][1] * m[1][3] * m[2][2] * m[3][0] - m[0][1] * m[1][0] * m[2][2] * m[3][3] + m[0][1] * m[1][0] * m[2][3] * m[3][2]
		+ m[0][2] * m[1][3] * m[2][0] * m[3][1] - m[0][2] * m[1][3] * m[2][1] * m[3][0] + m[0][2] * m[1][0] * m[2][1] * m[3][3] - m[0][2] * m[1][0] * m[2][3] * m[3][1]
		+ m[0][2] * m[1][1] * m[2][3] * m[3][0] - m[0][2] * m[1][1] * m[2][0] * m[3][3] - m[0][3] * m[1][0] * m[2][1] * m[3][2] + m[0][3] * m[1][0] * m[2][2] * m[3][1]
		- m[0][3] * m[1][1] * m[2][2] * m[3][0] + m[0][3] * m[1][1] * m[2][0] * m[3][2] - m[0][3] * m[1][2] * m[2][0] * m[3][1] + m[0][3] * m[1][2] * m[2][1] * m[3][0];
}


Matrix4& Matrix4::Inverse()
{
	// Compute the reciprocal determinant
	float det = Determinant();
	if (det == 0.0f)
	{
		// Matrix not invertible. Setting all elements to nan is not really
		// correct in a mathematical sense but it is easy to debug for the
		// programmer.
		/*const float nan = std::numeric_limits<float>::quiet_NaN();
		*this = Matrix4f(
		nan,nan,nan,nan,
		nan,nan,nan,nan,
		nan,nan,nan,nan,
		nan,nan,nan,nan);*/
		FILTH_ASSERT(0);
		return *this;
	}

	float invdet = 1.0f / det;

	Matrix4 res;
	res.m[0][0] = invdet  * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[1][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3]) + m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
	res.m[0][1] = -invdet * (m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[0][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3]) + m[0][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
	res.m[0][2] = invdet  * (m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) + m[0][2] * (m[1][3] * m[3][1] - m[1][1] * m[3][3]) + m[0][3] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]));
	res.m[0][3] = -invdet * (m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) + m[0][2] * (m[1][3] * m[2][1] - m[1][1] * m[2][3]) + m[0][3] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]));
	res.m[1][0] = -invdet * (m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[1][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
	res.m[1][1] = invdet  * (m[0][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[0][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[0][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
	res.m[1][2] = -invdet * (m[0][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) + m[0][2] * (m[1][3] * m[3][0] - m[1][0] * m[3][3]) + m[0][3] * (m[1][0] * m[3][2] - m[1][2] * m[3][0]));
	res.m[1][3] = invdet  * (m[0][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) + m[0][2] * (m[1][3] * m[2][0] - m[1][0] * m[2][3]) + m[0][3] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]));
	res.m[2][0] = invdet  * (m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) + m[1][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
	res.m[2][1] = -invdet * (m[0][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) + m[0][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[0][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
	res.m[2][2] = invdet  * (m[0][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) + m[0][1] * (m[1][3] * m[3][0] - m[1][0] * m[3][3]) + m[0][3] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
	res.m[2][3] = -invdet * (m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) + m[0][1] * (m[1][3] * m[2][0] - m[1][0] * m[2][3]) + m[0][3] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
	res.m[3][0] = -invdet * (m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) + m[1][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2]) + m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
	res.m[3][1] = invdet  * (m[0][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) + m[0][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2]) + m[0][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
	res.m[3][2] = -invdet * (m[0][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) + m[0][1] * (m[1][2] * m[3][0] - m[1][0] * m[3][2]) + m[0][2] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
	res.m[3][3] = invdet  * (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) + m[0][1] * (m[1][2] * m[2][0] - m[1][0] * m[2][2]) + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
	*this = res;

	return *this;
}

Quaternion::Quaternion(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

void Quaternion::Normalize()
{
	float Length = sqrtf(x * x + y * y + z * z + w * w);

	x /= Length;
	y /= Length;
	z /= Length;
	w /= Length;
}


Quaternion Quaternion::Conjugate()
{
	Quaternion ret(-x, -y, -z, w);
	return ret;
}

Quaternion Quaternion::operator*(const Quaternion& q)
{
	const float _w = (w * q.w) - (x * q.x) - (y * q.y) - (z * q.z);
	const float _x = (x * q.w) + (w * q.x) + (y * q.z) - (z * q.y);
	const float _y = (y * q.w) + (w * q.y) + (z * q.x) - (x * q.z);
	const float _z = (z * q.w) + (w * q.z) + (x * q.y) - (y * q.x);

	Quaternion ret(_x, _y, _z, _w);

	return ret;
}

Quaternion Quaternion::operator*(const Vector3& v)
{
	const float _w = -(x * v.x) - (y * v.y) - (z * v.z);
	const float _x = (w * v.x) + (y * v.z) - (z * v.y);
	const float _y = (w * v.y) + (z * v.x) - (x * v.z);
	const float _z = (w * v.z) + (x * v.y) - (y * v.x);

	Quaternion ret(_x, _y, _z, _w);

	return ret;
}


Vector3 Quaternion::ToDegrees()
{
	float f[3];

	f[0] = (float)atan2(x * z + y * w, x * w - y * z);
	f[1] = (float)acos(-x * x - y * y - z * z - w * w);
	f[2] = (float)atan2(x * z - y * w, x * w + y * z);

	f[0] = ToDegree(f[0]);
	f[1] = ToDegree(f[1]);
	f[2] = ToDegree(f[2]);

	return Vector3(f);
}

const Matrix4& Transform::GetProjTrans()
{
	ProjTrans.InitPersProjTransform(PersProj);
	return ProjTrans;
}


const Matrix4& Transform::GetVPTrans()
{
	GetViewTrans();
	GetProjTrans();

	VPTrans = ProjTrans * ViewTrans;
	return VPTrans;
}

const Matrix4& Transform::GetWorldTrans()
{
	/*Matrix4 ScaleTrans, RotateTrans, TranslationTrans;

	ScaleTrans.InitScaleTransform(Scale.x, Scale.y, Scale.z);
	RotateTrans.InitRotateTransform(Rotation.x, Rotation.y, Rotation.z);
	TranslationTrans.InitTranslationTransform(WorldPos.x, WorldPos.y, WorldPos.z);

	WorldTrans = TranslationTrans * RotateTrans * ScaleTrans;*/

	return WorldTrans;
}

const Matrix4& Transform::GetViewTrans()
{
	Matrix4 CameraTranslationTrans, CameraRotateTrans;

	CameraTranslationTrans.InitTranslationTransform(-camera.Pos.x, -camera.Pos.y, -camera.Pos.z);
	CameraRotateTrans.InitCameraTransform(camera.Target, camera.Up);

	ViewTrans = CameraRotateTrans * CameraTranslationTrans;

	return ViewTrans;
}

const Matrix4& Transform::GetWVPTrans()
{
	GetWorldTrans();
	GetVPTrans();

	WVPTrans = VPTrans * WorldTrans;
	return WVPTrans;
}


const Matrix4& Transform::GetWVOrthoPTrans()
{
	GetWorldTrans();
	GetViewTrans();

	Matrix4 P;
	P.InitOrthoProjTransform(OrthoProj);

	WVPTrans = P * ViewTrans * WorldTrans;
	return WVPTrans;
}


const Matrix4& Transform::GetWVTrans()
{
	GetWorldTrans();
	GetViewTrans();

	WVTrans = ViewTrans * WorldTrans;
	return WVTrans;
}


const Matrix4& Transform::GetWPTrans()
{
	Matrix4 PersProjTrans;

	GetWorldTrans();
	PersProjTrans.InitPersProjTransform(PersProj);

	WPTrans = PersProjTrans * WorldTrans;
	return WPTrans;
}

void AABB::ComputeFromVertices(std::vector<Vector3>& Vertices)
{
	Min = Vector3::Infinity;
	Max = Vector3::NegInfinity;

	for (uint32 i = 0; i < Vertices.size(); i++)
	{
		Max.x = MaxFloat(Max.x, Vertices[i].x);
		Max.y = MaxFloat(Max.y, Vertices[i].y);
		Max.z = MaxFloat(Max.z, Vertices[i].z);

		Min.x = MinFloat(Min.x, Vertices[i].x);
		Min.y = MinFloat(Min.y, Vertices[i].y);
		Min.z = MinFloat(Min.z, Vertices[i].z);
	}
}

bool AABB::IsPointInside(Vector3 Point)
{
	if (Point.x < Min.x || Point.x > Max.x ||
		Point.y < Min.y || Point.y > Max.y ||
		Point.z < Min.z || Point.z > Max.z)
	{
		return false;
	}

	return true;
}

bool AABB::IsAABBInside(AABB Box)
{
	if (Box.Max.x < Min.x || Box.Min.x > Max.x ||
		Box.Max.y < Min.y || Box.Min.y > Max.y ||
		Box.Max.z < Min.z || Box.Min.z > Max.z)
	{
		return false;
	}

	return true;
}

bool AABB::IsIntersecting(AABB Box)
{
	if (Box.Min.x < Min.x || Box.Max.x > Max.x ||
		Box.Min.y < Min.y || Box.Max.y > Max.y ||
		Box.Min.z < Min.z || Box.Max.z > Max.z)
	{
		return true;
	}

	return false;
}

AABB AABB::Transform(Matrix4 T)
{
	Vector3 NewCenter = T * GetCenter();
	Vector3 OldEdge = GetSize() * 0.5f;
	Vector3 NewEdge = Vector3
	(
		abs(T.m[0][0]) * OldEdge.x + abs(T.m[1][0]) * OldEdge.y + abs(T.m[2][0]) * OldEdge.z,
		abs(T.m[0][1]) * OldEdge.x + abs(T.m[1][1]) * OldEdge.y + abs(T.m[2][1]) * OldEdge.z,
		abs(T.m[0][2]) * OldEdge.x + abs(T.m[1][2]) * OldEdge.y + abs(T.m[2][2]) * OldEdge.z
	);

	return AABB(NewCenter - NewEdge, NewCenter + NewEdge);
}

// returns the distance between Start and Box
float RaycastAABB(Vector3 Start, Vector3 End, AABB Box)
{
	Vector3 Direction = (End - Start).Normalize();

	if (Box.IsZero())
	{
		return INFINITY;
	}

	if (Box.IsPointInside(Start))
	{
		return 0.0f;
	}

	float Dist = INFINITY;

	// Check for intersecting in the X-direction
	if (Start.x < Box.Min.x && Direction.x > 0.0f)
	{
		float x = (Box.Min.x - Start.x) / Direction.x;
		if (x < Dist)
		{
			Vector3 Point = Start + x * Direction;
			if (Point.y >= Box.Min.y && Point.y <= Box.Max.y && Point.z >= Box.Min.z && Point.z <= Box.Max.z)
			{
				Dist = x;
			}
		}
	}
	if (Start.x > Box.Max.x && Direction.x < 0.0f)
	{
		float x = (Box.Max.x - Start.x) / Direction.x;
		if (x < Dist)
		{
			Vector3 Point = Start + x * Direction;
			if (Point.y >= Box.Min.y && Point.y <= Box.Max.y && Point.z >= Box.Min.z && Point.z <= Box.Max.z)
			{
				Dist = x;
			}
		}
	}
	// Check for intersecting in the Y-direction
	if (Start.y < Box.Min.y && Direction.y > 0.0f)
	{
		float x = (Box.Min.y - Start.y) / Direction.y;
		if (x < Dist)
		{
			Vector3 Point = Start + x * Direction;
			if (Point.x >= Box.Min.x && Point.x <= Box.Max.x && Point.z >= Box.Min.z && Point.z <= Box.Max.z)
			{
				Dist = x;
			}
		}
	}
	if (Start.y > Box.Max.y && Direction.y < 0.0f)
	{
		float x = (Box.Max.y - Start.y) / Direction.y;
		if (x < Dist)
		{
			Vector3 Point = Start + x * Direction;
			if (Point.x >= Box.Min.x && Point.x <= Box.Max.x && Point.z >= Box.Min.z && Point.z <= Box.Max.z)
			{
				Dist = x;
			}
		}
	}
	// Check for intersecting in the Z-direction
	if (Start.z < Box.Min.z && Direction.z > 0.0f)
	{
		float x = (Box.Min.z - Start.z) / Direction.z;
		if (x < Dist)
		{
			Vector3 Point = Start + x * Direction;
			if (Point.x >= Box.Min.x && Point.x <= Box.Max.x && Point.y >= Box.Min.y && Point.y <= Box.Max.y)
			{
				Dist = x;
			}
		}
	}
	if (Start.z > Box.Max.z && Direction.z < 0.0f)
	{
		float x = (Box.Max.z - Start.z) / Direction.z;
		if (x < Dist)
		{
			Vector3 Point = Start + x * Direction;
			if (Point.x >= Box.Min.x && Point.x <= Box.Max.x && Point.y >= Box.Min.y && Point.y <= Box.Max.y)
			{
				Dist = x;
			}
		}
	}

	return Dist;
}