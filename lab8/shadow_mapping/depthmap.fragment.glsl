#version 330 core
in vec2 inoutUV;
uniform sampler2D tex0;
void main()
{
    vec4 texColor = texture(tex0, inoutUV);

    if(texColor.a < 0.1){
        discard;
    }
    else{
        gl_FragDepth = gl_FragCoord.z;
    }



}  