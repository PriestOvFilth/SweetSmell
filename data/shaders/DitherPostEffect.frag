in vec2 TexCoord0;

uniform sampler2D Tex;

out vec4 FragColor;

const int IndexMatrix8x8[64] = int[](0,  32, 8,  40, 2,  34, 10, 42,
                                     48, 16, 56, 24, 50, 18, 58, 26,
                                     12, 44, 4,  36, 14, 46, 6,  38,
                                     60, 28, 52, 20, 62, 30, 54, 22,
                                     3,  35, 11, 43, 1,  33, 9,  41,
                                     51, 19, 59, 27, 49, 17, 57, 25,
                                     15, 47, 7,  39, 13, 45, 5,  37,
                                     63, 31, 55, 23, 61, 29, 53, 21);

float IndexValue()
{
	int X = int(mod(gl_FragCoord.x, 8));
	int Y = int(mod(gl_FragCoord.y, 8));
	return IndexMatrix8x8[(X + Y * 8)] / 64.0;
}

float Dither(float Color)
{
	if (Color <= 0.05)
	{
		return 0;
	}
	else if (Color >= 0.85)
	{
		return 1;
	}
	else
	{
		float ClosestColor = (Color < 0.5) ? 0 : 1;
		float SecondClosestColor = 1 - ClosestColor;
		float D = IndexValue();
		float Distance = abs(ClosestColor - Color);
		return (Distance < D) ? ClosestColor : SecondClosestColor;
	}
}

float Luma(vec4 Color)
{
	return dot(Color.rgb, vec3(0.299, 0.587, 0.114));
}

void main()
{
	vec4 Color;
	Color.x = Dither(texture(Tex, TexCoord0).x);
	Color.y = Dither(texture(Tex, TexCoord0).y);
	Color.z = Dither(texture(Tex, TexCoord0).z);
	Color.w = 1.0;

	FragColor = mix(Color, texture(Tex, TexCoord0), 0.9);
}