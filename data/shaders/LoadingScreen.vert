layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 TexCoord;

out vec2 TexCoord0;

void main()
{
	gl_Position = vec4(Position, 0.0, 1.0);

	TexCoord0 = vec2(TexCoord.x, 1.0 - TexCoord.y);
}