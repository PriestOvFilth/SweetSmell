uniform sampler2D ColorMap;

in vec2 TexCoord;
out vec4 FragColor;

void main()
{
	FragColor = texture2D(ColorMap, TexCoord);

	if (FragColor.r == 0 && FragColor.g == 0 && FragColor.b == 0)
	{
		discard;
	}
}