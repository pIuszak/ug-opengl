#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;
layout( location = 2 ) in vec2 inUV;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

uniform mat4 lightProj;
uniform mat4 lightView;

out vec4 inoutPos;
out vec2 inoutUV;
out vec4 ourPosLight;
out vec3 ourNormal;
out vec4 ourPosition;
// struktura danych wyjsciowych
out VS_OUT
{
	vec4 coord;
//	vec4 color;

} vs_out;


void main()
{
	gl_Position = matProj * matView * matModel * inPosition;
	inoutPos = inPosition;
	ourPosition = inPosition;
	ourPosLight = lightProj * lightView * matModel * inPosition;
	ourNormal = inNormal;
	inoutUV = inUV;
	vs_out.coord = inPosition;
}
