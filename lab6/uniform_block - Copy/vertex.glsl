// Tutaj nie dzieje sie nic nowego
#version 330

#define POSITION 0
#define COLOR 1

layout( location = POSITION ) in vec4 inPosition;
layout( location = COLOR ) in vec4 inColor;


out vec4 inoutColor;

void main()
{


	gl_Position = inPosition;

	inoutColor = inColor;

}
