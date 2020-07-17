#version 150

in vec4 inoutPos;
in vec2 inoutUV;

out vec4 outColor;

uniform sampler2D tex0;
uniform int RenderIndex;
void main()
{
	vec4 gl_FragColor = texture( tex0, inoutUV );// + vec4(myColor, 1.0)/2.0;
	float luminance = (gl_FragColor.r+gl_FragColor.g+gl_FragColor.b)/3.0;
	outColor= vec4(luminance, luminance, luminance, 1.0);;
}
