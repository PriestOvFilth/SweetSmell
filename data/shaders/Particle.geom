#vlayout(points) in;
layout(points) out;
layout(max_vertices = 30) out;

in float Type0[];
in vec3 Position0[];
in vec3 Velocity0[];
in float Age0[];

out float Type1;
out vec3 Position1;
out vec3 Velocity1;
out float Age1;

uniform float DeltaTime;
uniform float Time;
uniform sampler1D RandomTex;
uniform float LauncherLifetime;
uniform float ShellLifetime;
uniform float SecondaryShellLifetime;

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f

vec3 GetRandomDir(float TexCoord)
{
	vec3 Dir = texture(RandomTex, TexCoord).xyz;
	Dir -= vec3(0.5, 0.5, 0.5);
	return Dir;
}

void main()
{
	float Age = Age0[0] + DeltaTime;

	if (Type0[0] == PARTICLE_TYPE_LAUNCHER)
	{
		if (Age >= LauncherLifetime)
		{
			Type1 = PARTICLE_TYPE_SHELL;
			Position1 = Position0[0];
			vec3 Dir = GetRandomDir(Time / 1000.0);
			Dir.y = max(Dir.y, 0.5);
			Velocity1 = normalize(Dir) / 20.0;
			Age1 = 0.0;
			EmitVertex();
			EndPrimitive();
			Age = 0.0;
		}

		Type1 = PARTICLE_TYPE_LAUNCHER;
		Position1 = Position0[0];
		Velocity1 = Velocity0[0];
		Age1 = Age;
		EmitVertex();
		EndPrimitive();
	}
	else
	{
		float DeltaTimeSecs = DeltaTime / 1000.0f;
		float Time1 = Age0[0] / 1000.0;
		float Time2 = Age / 1000.0;
		vec3 DeltaP = DeltaTimeSecs * Velocity0[0];
		vec3 DeltaV = vec3(DeltaTimeSecs) * vec3(0.0, -9.81, 0.0);

		if (Type0[0] == PARTICLE_TYPE_SHELL)
		{
			if (Age < ShellLifetime)
			{
				Type1 = PARTICLE_TYPE_SHELL;
				Position1 = Position0[0] + DeltaP;
				Velocity1 = Velocity0[0] + DeltaV;
				Age1 = Age;
				EmitVertex();
				EndPrimitive();
			}
			else
			{
				for (int i = 0; i < 10; i++)
				{
					Type1 = PARTICLE_TYPE_SECONDARY_SHELL;
					Position1 = Position0[0];
					vec3 Dir = GetRandomDir((Time + i) / 1000.0);
					Velocity1 = normalize(Dir) / 20.0;
					Age1 = 0.0f;
					EmitVertex();
					EndPrimitive();
				}
			}
		}
		else
		{
			if (Age < SecondaryShellLifetime)
			{
				Type1 = PARTICLE_TYPE_SECONDARY_SHELL;
				Position1 = Position0[0] + DeltaP;
				Velocity1 = Velocity0[0] + DeltaV;
				Age1 = Age;
				EmitVertex();
				EndPrimitive();
			}
		}
	}
}