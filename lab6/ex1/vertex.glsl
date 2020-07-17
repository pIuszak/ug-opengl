#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;
layout( location = 2 ) in vec2 inUV;

out vec4 ourPosition;
out vec3 ourNormal;
out vec2 inoutUV;

out vec4 inoutPos;


uniform mat4 matProjection;
uniform mat4 matView;
uniform mat4 Matrix_proj_mv;
void main()
{

	gl_Position = matProjection * matView * inPosition;

	// Sending coordinates and normals in Model/World space
	ourPosition = inPosition;

	ourNormal = inNormal;
	inoutUV = inUV;
//	gl_Position = Matrix_proj_mv * inPosition;
//
//	inoutPos = inPosition;
//	inoutUV = inUV;

}

