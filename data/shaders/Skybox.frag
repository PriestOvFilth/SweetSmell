in vec3 TexCoord0;

out vec4 FragColor;

uniform samplerCube CubemapTex;
uniform bool bEnableFog;
uniform vec3 FogColor;

void main()
{
	vec4 TexColor = texture(CubemapTex, TexCoord0);

	// fog
	const float LOG2 = 1.442695;
	float Z = gl_FragCoord.z / gl_FragCoord.w;
	float FogFactor = exp2(-0.002 * 0.002 * Z * Z * LOG2);
	FogFactor = clamp(FogFactor, 0.0, 1.0);

	vec4 Color = TexColor;

	if (bEnableFog)
	{
		Color = mix(vec4(FogColor, 1.0), TexColor, FogFactor);
	}

	gl_FragColor = Color;
}