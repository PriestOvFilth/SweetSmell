in vec3 Position;
in vec4 ColorPointSize;

out vec4 Color;
uniform mat4 WVP;

void main()
{
	gl_Position = WVP * vec4(Position, 1.0);
	gl_PointSize = ColorPointSize.w;
	Color = vec4(ColorPointSize.xyz, 1.0);
}