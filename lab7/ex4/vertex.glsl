#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec2 inUV;
layout( location = 2 ) in vec3 inNormal;
layout( location = 3 ) in mat4 localMatrix;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;
uniform int Frame;

out vec4 inoutPos;
out vec2 inoutUV;


void main()
{

    vec4 newPos = inPosition;
//    newPos.x += gl_InstanceID/10;
//    newPos.y += gl_InstanceID%10;
        newPos.z += Frame*0.01;

    gl_Position = matProj * matView * localMatrix * newPos;

    inoutPos = inPosition;
    inoutUV = inUV;
}
