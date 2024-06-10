layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform mat4 VP;
uniform vec3 CamPos;
uniform float Size;

out vec2 TexCoord;

void main()
{
	vec3 Pos = gl_in[0].gl_Position.xyz;
	vec3 ToCam = normalize(CamPos - Pos);
	vec3 Up = vec3(0.0, 1.0, 0.0);
	vec3 Right = cross(ToCam, Up) * Size;

	Pos -= (Right * 0.5);
	gl_Position = VP * vec4(Pos, 1.0);
	TexCoord = vec2(0.0, 0.0);
	EmitVertex();

	Pos.y += Size;
	gl_Position = VP * vec4(Pos, 1.0);
	TexCoord = vec2(0.0, 1.0);
	EmitVertex();

	Pos.y -= Size;
	Pos += Right;
	gl_Position = VP * vec4(Pos, 1.0);
	TexCoord = vec2(1.0, 0.0);
	EmitVertex();

	Pos.y += Size;
	gl_Position = VP * vec4(Pos, 1.0);
	TexCoord = vec2(1.0, 1.0);
	EmitVertex();

	EndPrimitive();
}