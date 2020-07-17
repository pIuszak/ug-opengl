//#version 150 core
//in vec3 Color;
//in vec2 Texcoord;
//out vec4 outColor;
//uniform sampler2D bubbles;
//uniform sampler2D texuvtemplate;
//void main()
//{
//	outColor = mix(texture(texuvtemplate, Texcoord), texture(texuvtemplate, Texcoord), 0.5);
//}
#version 150

in vec4 inoutPos;
in vec3 inoutColor;
in vec2 inoutUV;

out vec4 outColor;

// Sampler tekstury
uniform sampler2D tex0;


void main()
{

	//outColor = texture( tex0, inoutUV );

}