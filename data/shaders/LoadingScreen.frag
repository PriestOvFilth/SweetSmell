in vec2 TexCoord0;

out vec4 FragColor;

uniform sampler2D Tex;

void main()
{
	FragColor = texture2D(Tex, TexCoord0.st);
}