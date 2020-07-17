#version 150 core
in vec4 inoutPos;
in vec3 ourNormal;
out vec4 outColor;

// Parametry oswietlenia
uniform vec3 Light_Ambient;
uniform vec3 Light_Diffuse;
uniform vec3 Light_Position;
float distance(int x1, int y1, int x2, int y2)
{
    // Calculating distance
    return sqrt(pow(x2 - x1, 2) +
    pow(y2 - y1, 2) * 1.0);
}

out float visibility;
const float density = 0.07;
const float gradient = 1.5;

in vec3 inoutColor;
in vec2 inoutUV;

// Sampler tekstury
uniform sampler2D tex0;

void main()
{
    #define MAX_LIGHTS 3

    vec3 Light_Positions[MAX_LIGHTS];
    Light_Positions[0] = vec3(22, 3, 12);
    Light_Positions[1] = vec3(19, 1, -5);
    Light_Positions[2] = vec3(-5, 1, 7);

    vec3 pointLights = vec3(0.0);

    // ustawienie domyslnego koloru fragmentu
    // mozna tutaj uwzglednic tekstury i inne parametry

    //vec3 objectColor = vec3(0.2, 0.8, 0.2);
    vec4 objectColor = texture(tex0, vec2(inoutUV.x, -inoutUV.y));
    vec4 fogColor = vec4(.5, .5, .5, 1.);

    // Obliczenie wektora (swiatlo - wierzcholek)
    // czyli kierunku padania swiatla na wierzcholek
    vec3 resultDiffuse = vec3(0.2, 0.8, 0.2);
    float lightCoeff = 0;
    // brightness
    float dist = 0;
    vec3 lightDirection =vec3(0.2, 0.8, 0.2);
    for (int i=0; i<MAX_LIGHTS; i++)
    {

        // Obliczenie dla swiatla o numerze i
        // tak jak poprzednio (dla jednego swiatla, ale teraz
        // zamiast pozycji swiatla uzyj pozycji z tablicy

        // light
        lightDirection = normalize(Light_Positions[i] - vec3(ourPosition));
        dist = length(Light_Positions[i] - vec3(ourPosition));
        lightCoeff = max(dot(ourNormal, lightDirection), 0.0);
        resultDiffuse = lightCoeff * Light_Diffuse;
        pointLights += resultDiffuse  / dist;

        // fog
        // fog is more intence further away from lights
        visibility += exp(-pow((dist*density), gradient));
        visibility = clamp(visibility, 0.0, 1.0);

    }
    // Zastosowanie oswietlenia do fragmentu
    vec3 result = (Light_Ambient + pointLights) * objectColor.rgb;
    outColor = vec4(result, 1.0) + (fogColor * (1 - visibility));


}
