#version 330 core
layout (location = 0) in vec4 inPosition;
layout (location = 2) in vec2 inUV;

uniform mat4 lightProj;
uniform mat4 lightView;
uniform mat4 matModel;

out vec2 inoutUV;

void main()
{
    gl_Position = lightProj * lightView * matModel * inPosition;

    inoutUV = inUV;
}
