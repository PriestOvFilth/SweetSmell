layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

const int MAX_BONES = 100;

uniform mat4 Bones[MAX_BONES];

uniform mat4 WVP;
uniform mat4 LightWVP;
uniform mat4 World;

out vec4 LightSpacePos;
out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;

void main()
{
	mat4 BoneTransform = Bones[BoneIDs[0]] * Weights[0];
	BoneTransform += Bones[BoneIDs[1]] * Weights[1];
	BoneTransform += Bones[BoneIDs[2]] * Weights[2];
	BoneTransform += Bones[BoneIDs[3]] * Weights[3];

	vec4 PosL = BoneTransform * vec4(Position, 1.0);
	vec4 NormalL = BoneTransform * vec4(Normal, 0.0);

	gl_Position = WVP * PosL;
	TexCoord0 = TexCoord;
	LightSpacePos = LightWVP * PosL;
	Normal0 = (World * NormalL).xyz;
	WorldPos0 = (World * PosL).xyz;
}