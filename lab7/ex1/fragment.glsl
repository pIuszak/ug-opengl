#version 150

//in vec4 inoutColor;
//in vec4 inoutCoord;

in VS_OUT
{
	vec4 coord;
//	vec4 color;
} fs_in;

out vec4 outColor;



void main()
{

	// szalone kolory
    float odl = sqrt(fs_in.coord.x*fs_in.coord.x + fs_in.coord.y*fs_in.coord.y + fs_in.coord.z*fs_in.coord.z)/5.0;
    outColor = vec4(sin(odl*13)/2 + 0.5,sin(fs_in.coord.y)/2 + 0.5, sin(gl_PrimitiveID/100.0)/5 + 0.5,1.0);


	outColor = vec4(1.0, fs_in.coord.y, 0.0, 1.0);
}
