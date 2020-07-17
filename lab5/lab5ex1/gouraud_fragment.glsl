#version 150 core

in vec3 ourColor;

out vec4 outColor;


// Parametry oswietlenia
uniform vec3 Light_Ambient;
uniform vec3 Light_Diffuse;
uniform vec3 Light_Position;


void main()
{

	// ustawienie domyslnego koloru fragmentu
	// mozna tutaj uwzglednic tekstury i inne parametry
	vec3 objectColor = vec3(0.2, 0.8, 0.2);

	vec3 result = ourColor * objectColor;

	outColor = vec4(result, 1.0);

}
