#version 150

in vec4 inoutPos;
in vec2 inoutUV;

out vec4 outColor;


// Sampler tekstury - zwroc uwage na typ
uniform sampler2D tex0;


void main()
{
	vec4 texColor = texture( tex0, inoutUV );
	if(texColor.a < 0.1)
        discard;

	outColor = texColor;
}
