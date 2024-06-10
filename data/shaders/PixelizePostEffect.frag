in vec2 TexCoord0;

uniform sampler2D Tex;
uniform vec2 ViewportSize;
uniform float PixelSize;

out vec4 FragColor;

void main()
{
	vec2 UV = gl_FragCoord.xy / ViewportSize.xy;
	vec2 Div = vec2(ViewportSize.x * PixelSize / ViewportSize.y, PixelSize);
	UV = floor(UV * Div) / Div;
	FragColor = texture(Tex, UV);
}