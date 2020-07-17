// 1. Inicjalizacja GLUTa : parametr GLUT_RGBA

glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

// 2. Wlaczenie kanalu Alpha w maszynie OpenGLa

glEnable( GL_BLEND );
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// 3. Mozemy juz uzywac parametru a w wektorach koloru rgba

// np. w shaderze geometrii:
// outColor = vec4(1.0, 0.0, 0.0, 0.5);

// 4. Ladowanie tekstury z kanalem alpha (plik PNG 32bit) z warstwa przezroczystosci

// parametry RGBA

image = SOIL_load_image("flower.png", &width, &height, 0, SOIL_LOAD_RGBA);

glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	
// 5. Shader fragmentow

void main()
{
	vec4 texColor = texture( tex0, inoutUV );
	
	if(texColor.a < 0.1)
        discard;

	outColor = texColor;
}


