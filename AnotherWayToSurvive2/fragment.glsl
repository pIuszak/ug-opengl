#version 150

in vec4 ourPos;
in vec4 ourPosLight;
in vec3 ourNormal;

// Potrzebne do shadowMap i oswietlenia kierunkowego
uniform sampler2D shadowMap;
uniform vec3 Camera_Position;
uniform vec3 Light_Direction;

//FOG
// Parametry oswietlenia
uniform vec3 Light_Ambient;
uniform vec3 Light_Diffuse;
uniform vec3 Light_Position;
uniform vec3 PlayerPos;
float distance(int x1, int y1, int x2, int y2)
{
	// Calculating distance
	return sqrt(pow(x2 - x1, 2) +
	pow(y2 - y1, 2) * 1.0);
}

out float visibility;
const float density = 0.07;
const float gradient = 1.5;


in vec4 inoutPos;
out vec4 outColor;
out vec4 outFogColor;
in vec2 inoutUV;

struct LightParameters
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

uniform sampler2D tex0;

in VS_OUT
{
	vec4 coord;
//	vec4 color;
} fs_in;


// ---------------------------------------------------------------------------
float Calculate_Shadow(vec4 fragPosLight, vec3 Light_Direction)
{

	// perform perspective divide
	vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;

	if(projCoords.z > 1.0)
	return 0.0;

	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;

	// calculate bias according to the angle between normal vector and light vector
	float bias = max(0.004 * (1.0 - dot(ourNormal, Light_Direction)), 0.001);

	//bias = 0.0f;

	// Simple filtering
	// check whether current frag pos is in shadow
	// float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	// return shadow;

	// Smooth filtering
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(float x = -1; x <= 1; x+=1.0)
	{
		for(float y = -1; y <= 1; y+=1.0)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	return shadow;
}



// ---------------------------------------------------------------------------
vec3 Calculate_DirectionalLight(vec3 lightDiffuse, vec3 lightSpecular, vec3 LightDirection)
{
	// diffuse
	vec3  lightDirection = normalize(LightDirection);
	float diffuseCoeff = max(dot(ourNormal, -lightDirection), 0.0);
	vec3  resultDiffuse = diffuseCoeff * vec3(lightDiffuse);
	vec3  diffusePart = resultDiffuse;

	// specular
	vec3  viewDir = normalize(Camera_Position - vec3(inoutPos));
	vec3  reflectDir = reflect(lightDirection, ourNormal);
	float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3  specularPart = specularCoeff * vec3(lightSpecular);
	float shadowPart;
	// result
	return (diffusePart + specularPart);
}



// ---------------------------------------------------------------------------
void main()
{
	#define MAX_LIGHTS 3

	vec3 Light_Positions = Camera_Position;

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

		// Obliczenie dla swiatla o numerze i
		// tak jak poprzednio (dla jednego swiatla, ale teraz
		// zamiast pozycji swiatla uzyj pozycji z tablicy

		// light
		lightDirection = normalize(Light_Positions - vec3(inoutPos));
		dist = length(Light_Positions - vec3(inoutPos));
		lightCoeff = max(dot(ourNormal, lightDirection), 0.0);
		resultDiffuse = lightCoeff * Light_Diffuse;
		pointLights += resultDiffuse  / dist;

		// fog
		// fog is more intence further away from lights
		visibility += exp(-pow((dist*density), gradient));
		visibility = clamp(visibility, 0.0, 1.0);


	// Zastosowanie oswietlenia do fragmentu
	vec3 mid = (Light_Ambient + pointLights) * objectColor.rgb;
	outFogColor = vec4(mid, 1.0) + (fogColor * (1 - visibility));




	vec4 texColor = texture( tex0, inoutUV );
	float shadowPart = 0;
	vec3  lightPart = vec3(0.0, 0.0, 0.0);
	vec3  finalColor = vec3(0.0, 0.0, 0.0);
	//outColor = texColor;

	LightParameters myLight;

	// Parametry swiatla
	myLight.Ambient = vec3(0.2, 0.2, 0.2);
	myLight.Diffuse = vec3(0.8, 0.8, 0.8);
	myLight.Specular = vec3(0.5, 0.5, 0.5);

	// Kolor wyjsciowy
	//vec3  myColor = ;
	// Skladowe
	if(texColor.a > 0.1)
	{
		lightPart = Calculate_DirectionalLight(myLight.Diffuse, myLight.Specular, Light_Direction);
		shadowPart = Calculate_Shadow(ourPosLight,Light_Direction);
		finalColor = (myLight.Ambient + lightPart * (1 - shadowPart));
		outColor = vec4(finalColor , 1.0) * texColor + outFogColor;
	}
	else{
		discard;
	}




}


