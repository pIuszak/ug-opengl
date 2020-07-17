#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;

out vec3 ourColor;

uniform mat4 Matrix_proj_mv;

uniform vec3 Light_Ambient;
uniform vec3 Light_Diffuse;
uniform vec3 Light_Position;


void main()
{
	gl_Position = Matrix_proj_mv * inPosition;


	// Obliczenie wektora (swiatlo - wierzcholek)
	// czyli kierunku padania swiatla na wierzcholek
	vec3 lightDirection = normalize(Light_Position - vec3(inPosition));

	// obliczenie kata pomiedzy wektorem lightDir oraz wektorem normalnym
	// wartosc kata okresla pod jakim katem padaja promienie
	float lightCoeff = max(dot(inNormal, lightDirection), 0.0);

	// obliczenie skladowej diffuse
	vec3 resultDiffuse = lightCoeff * Light_Diffuse;

	// Zastosowanie oswietlenia do fragmentu
	vec3 result = (Light_Ambient + resultDiffuse);

	ourColor = result;

}
