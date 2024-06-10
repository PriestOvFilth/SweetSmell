layout (location = 0) in vec3 Position;

uniform mat4 WVP;

out vec3 TexCoord0;

void main()
{
	TexCoord0 = Position;
	vec4 Pos = WVP * vec4(Position, 1.0);
	gl_Position = Pos.xyww;
}