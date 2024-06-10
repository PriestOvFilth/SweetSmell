#include "particles.h"
#include "billboard.h"
#include "texture.h"
#include "resource.h"

#define MAX_PARTICLES 1000
#define PARTICLE_LIFETIME 10.0f

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f

struct Particle
{
	float Type;
	Vector3 Pos;
	Vector3 Velocity;
	float Lifetime;
};

bool ParticleShader::Init()
{
	if (!Shader::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "data/shaders/Particle.vert"))
	{
		return false;
	}

	if (!AddShader(GL_GEOMETRY_SHADER, "data/shaders/Particle.geom"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, "data/shaders/Particle.frag"))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	const GLchar* Varyings[4];
	Varyings[0] = "Type1";
	Varyings[1] = "Position1";
	Varyings[2] = "Velocity1";
	Varyings[3] = "Age1";

	glTransformFeedbackVaryings(ShaderProgram, 4, Varyings, GL_INTERLEAVED_ATTRIBS);

	if (!Finalize())
	{
		return false;
	}

	DeltaTimeLoc = GetUniformLocation("DeltaTime");
	RandomTexLoc = GetUniformLocation("RandomTex");
	TimeLoc = GetUniformLocation("Time");
	LauncherLifetimeLoc = GetUniformLocation("LauncherLifetime");
	ShellLifetimeLoc = GetUniformLocation("ShellLifetime");
	SecondaryShellLifetimeLoc = GetUniformLocation("SecondaryShellLifetime");

	if (DeltaTimeLoc == INVALID ||
		RandomTexLoc == INVALID ||
		TimeLoc == INVALID ||
		LauncherLifetimeLoc == INVALID ||
		ShellLifetimeLoc == INVALID ||
		SecondaryShellLifetimeLoc == INVALID)
	{
		return false;
	}

	return true;
}

void ParticleShader::SetDeltaTime(float DeltaTime)
{
	glUniform1f(DeltaTimeLoc, DeltaTime);
}

void ParticleShader::SetTime(float Time)
{
	glUniform1f(TimeLoc, Time);
}

void ParticleShader::SetRandomTextureUnit(uint32 TextureUnit)
{
	glUniform1i(RandomTexLoc, TextureUnit);
}

void ParticleShader::SetLauncherLifetime(float Lifetime)
{
	glUniform1f(LauncherLifetimeLoc, Lifetime);
}

void ParticleShader::SetShellLifetime(float Lifetime)
{
	glUniform1f(ShellLifetimeLoc, Lifetime);
}

void ParticleShader::SetSecondaryShellLifetime(float Lifetime)
{
	glUniform1f(SecondaryShellLifetimeLoc, Lifetime);
}

ParticleSystem::ParticleSystem()
{
	CurrentVB = 0;
	CurrentTFB = 1;
	bIsFirst = true;
	Time = 0;
	Tex = NULL;
	PShader = NULL;
	BBShader = NULL;

	ZERO_MEM(TransformFeedback);
	ZERO_MEM(ParticleBuffer);
}

ParticleSystem::~ParticleSystem()
{
	SAFE_DELETE(Tex);
	SAFE_DELETE(PShader);
	SAFE_DELETE(BBShader);

	if (TransformFeedback[0] != 0)
	{
		glDeleteTransformFeedbacks(2, TransformFeedback);
	}

	if (ParticleBuffer[0] != 0)
	{
		glDeleteBuffers(2, ParticleBuffer);
	}
}

bool ParticleSystem::InitParticleSystem(const Vector3 Pos)
{
	Particle Particles[MAX_PARTICLES];
	ZERO_MEM(Particles);

	Particles[0].Type = PARTICLE_TYPE_LAUNCHER;
	Particles[0].Pos = Pos;
	Particles[0].Velocity = Vector3(2.0f, 1.0f, 5.f);
	Particles[0].Lifetime = 0.0f;

	glGenTransformFeedbacks(2, TransformFeedback);
	glGenBuffers(2, ParticleBuffer);

	for (uint32 i = 0; i < 2; i++)
	{
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, TransformFeedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, ParticleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particles), Particles, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, ParticleBuffer[i]);
	}

	PShader = new ParticleShader();
	if (!PShader->Init())
	{
		return false;
	}

	PShader->Bind();
	PShader->SetRandomTextureUnit(3);
	PShader->SetLauncherLifetime(100.f);
	PShader->SetShellLifetime(10000.f);
	PShader->SetSecondaryShellLifetime(2500.f);

	RandTex = new RandomTexture();
	if (!RandTex->Init(1000))
	{
		return false;
	}

	RandTex->Bind(GL_TEXTURE3);

	BBShader = new BillboardShader();
	if (!BBShader->Init())
	{
		return false;
	}

	BBShader->Bind();
	BBShader->SetColorTextureUnit(0);
	BBShader->SetBillboardSize(0.1f);

	Tex = GResourceMan->GetTexture("data/textures/fireworks_red.jpg");
	if (!Tex)
	{
		return false;
	}

	return GLCheckError();
}

void ParticleSystem::Render(float DeltaTime, const Matrix4 VP, const Vector3 CameraPos)
{
	Time += DeltaTime;

	UpdateParticles(DeltaTime);
	RenderParticles(VP, CameraPos);

	CurrentVB = CurrentTFB;
	CurrentTFB = (CurrentTFB + 1) & 0x1;
}

void ParticleSystem::UpdateParticles(float DeltaTime)
{
	PShader->Bind();
	PShader->SetTime(Time);
	PShader->SetDeltaTime(DeltaTime);

	RandTex->Bind(GL_TEXTURE3);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindBuffer(GL_ARRAY_BUFFER, ParticleBuffer[CurrentVB]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, TransformFeedback[CurrentTFB]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), 0); // type
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4); // position
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)16); // velocity
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)28); // lifetime

	glBeginTransformFeedback(GL_POINTS);

	if (bIsFirst)
	{
		glDrawArrays(GL_POINTS, 0, 1);
		bIsFirst = false;
	}
	else
	{
		glDrawTransformFeedback(GL_POINTS, TransformFeedback[CurrentVB]);
	}

	glEndTransformFeedback();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}

void ParticleSystem::RenderParticles(const Matrix4 VP, const Vector3 CameraPos)
{
	BBShader->Bind();
	BBShader->SetCameraPos(CameraPos);
	BBShader->SetVP(VP);
	Tex->Bind(GL_TEXTURE0);

	glDisable(GL_RASTERIZER_DISCARD);

	glBindBuffer(GL_ARRAY_BUFFER, ParticleBuffer[CurrentTFB]);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4); // position

	glDrawTransformFeedback(GL_POINTS, TransformFeedback[CurrentTFB]);

	glDisableVertexAttribArray(0);
}