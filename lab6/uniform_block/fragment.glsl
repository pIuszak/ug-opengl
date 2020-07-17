#version 330

in vec4 inoutColor;
out vec4 outColor;


// Pojedyncza zmienna uniform
uniform float Gray;

// Tablica zmiennych
uniform float Table_of_Numbers[11];


// Struktura
struct SColor
{
	float r;
	float g;
	float b;
	float a;
};

// Zmienna strukturalna typu uniform
uniform SColor greenColor;



struct StrColor
{
	vec4 color;
};

// Block zmiennych uniform
layout (std140) uniform Table_of_Colors
{
	StrColor Block_Color[6];

};


void main()
{
	vec4 newColor = inoutColor;

	newColor.r = Gray;
	newColor.g = Table_of_Numbers[gl_PrimitiveID];

	//newColor.r = Table_of_Numbers[5];
	//newColor.g = Table_of_Numbers[5];
	//newColor.b = Table_of_Numbers[5];

	//newColor = vec4(greenColor.r, greenColor.g, greenColor.b, greenColor.a);

	// blok zmiennych uniform
	newColor = Block_Color[gl_PrimitiveID].color;



    outColor = newColor;
}
