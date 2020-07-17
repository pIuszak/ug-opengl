#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "shader_stuff.h"


// ---------------------------------------
// Identyfikatory obiektow

GLuint program;
GLuint vBuffer_coord;
GLuint vBuffer_color;
GLuint vBuffer_indices;
GLuint vArray;


// ---------------------------------------
// Wspolrzedne wierzchokow
GLfloat triangles_coord[2*6] =
{
	-0.5f, -1.0f,
	 0.5f, -1.0f,
	 1.0f,  0.0f,
	 0.5f,  1.0f,
	-0.5f,  1.0f,
	-1.0f,  0.0f
};

// ---------------------------------------
// Kolory wierzcholkow
GLfloat triangles_color[3*6] =
{
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 0.f, 1.f,
	1.f, 1.f, 0.f,
	0.f, 1.f, 1.f,
	1.f, 0.f, 1.f
};

// ---------------------------------------
// Indeksy wierzcholkow
GLuint triangles_indices[3*4] =
{
	0, 1, 2,
	0, 2, 3,
	0, 3, 4,
	0, 4, 5
};



float Gray = 0.5;

float Table_of_Numbers[11] = { 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 };

struct SColor
{
	float r;
	float g;
	float b;
	float a;
};

struct SColor greenColor = {0.1, 1.0, 0.1, 1.0};


struct SColor Table_of_Colors[6] =
{
	1.f, 0.f, 0.f, 1.f,
	0.f, 1.f, 0.f, 1.f,
	0.f, 0.f, 1.f, 1.f,
	1.f, 1.f, 0.f, 1.f,
	0.f, 1.f, 1.f, 1.f,
	1.f, 0.f, 1.f, 1.f
};




//identyfikator bufora
GLuint uniformBuffer;


// ---------------------------------------
void DisplayScene()
{

	glClear( GL_COLOR_BUFFER_BIT );

	// Wlaczenie VA i programu
	glBindVertexArray( vArray );
	glUseProgram( program );


	// Przekazywanie pojedynczej wartosci
	glUniform1f(glGetUniformLocation(program, "Gray"), Gray);


	// Przekazywanie tablicy w calosci
	glUniform1fv(glGetUniformLocation(program, "Table_of_Numbers"), 11, Table_of_Numbers);

	// lub pojedynczo
	glUniform1f(glGetUniformLocation(program, "Table_of_Numbers[5]"), Table_of_Numbers[5]);



	// Przekazanie skladowych struktury
	glUniform1f(glGetUniformLocation(program, "greenColor.r"), greenColor.r);
	glUniform1f(glGetUniformLocation(program, "greenColor.g"), greenColor.g);
	glUniform1f(glGetUniformLocation(program, "greenColor.b"), greenColor.b);
	glUniform1f(glGetUniformLocation(program, "greenColor.a"), greenColor.a);


	// Przekazanie tablicy struktur za pomoca bloku zmiennych uniform
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
	// cale dane
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Table_of_Colors), Table_of_Colors, GL_DYNAMIC_DRAW);

	// lub czesciowe
	//glBufferSubData(GL_UNIFORM_BUFFER,  8, sizeof(Table_of_Colors)-8, Table_of_Colors);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	// Z tablica wierzcholkow
	glDrawElements( GL_TRIANGLES, 4 * 3, GL_UNSIGNED_INT, NULL );




	glUseProgram( 0 );
	glBindVertexArray( 0 );
	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
}


// ---------------------------------------
void Initialize()
{

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );


	// 1. Program i shadery
	program = glCreateProgram();

	glAttachShader( program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram( program );



	// 2. Vertex arrays

	#define POSITION 0
	#define COLOR 1

	glGenVertexArrays( 1, &vArray );
	glBindVertexArray( vArray );

	// BUFOR: Wspolrzedne wierzcholkow

	glGenBuffers( 1, &vBuffer_coord );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_coord );
	glBufferData( GL_ARRAY_BUFFER, sizeof( triangles_coord ), triangles_coord, GL_STATIC_DRAW );

	glVertexAttribPointer( POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( POSITION );

	// BUFOR: Kolor wierzcholkow

	glGenBuffers( 1, &vBuffer_color );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_color  );
	glBufferData( GL_ARRAY_BUFFER, sizeof( triangles_color ), triangles_color, GL_STATIC_DRAW );

	glVertexAttribPointer( COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( COLOR );

	// BUFOR: Indeksy wierzcholkow

	glGenBuffers( 1, &vBuffer_indices );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vBuffer_indices );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( triangles_indices ), triangles_indices, GL_STATIC_DRAW );


	glBindVertexArray( 0 );


	// UBO - Uniform Block Objects

	// 1. Tworzymy bufor danych
	//GLuint uniformBuffer;

	glGenBuffers(1, &uniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);

	// alokacja pamieci bez przekazania danych
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Table_of_Colors), NULL, GL_STATIC_DRAW);
	// skopiowanie danych
	//glBufferData(GL_UNIFORM_BUFFER, sizeof(Table_of_Colors), Table_of_Colors, GL_DYNAMIC_DRAW);

	// 2. Stworzenie UBO i punktu bindowania
	GLuint blockID;

	blockID = glGetUniformBlockIndex(program, "Table_of_Colors");

	// identyfikator punktu bindowania (obecnie sami ustalamy)
	GLuint bindingPoint = 1;
	glUniformBlockBinding(program, blockID, bindingPoint);

	// Polaczenie bufora i punktu bindowania
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, uniformBuffer);

}




// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( 500, 500 );
	glutCreateWindow( "Tablice wierzcholkow" );
	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );


	// GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if( GLEW_OK != err )
	{
		printf("GLEW Error\n");
		exit(1);
	}

	// OpenGL
	if( !GLEW_VERSION_3_2 )
	{
		printf("Brak OpenGL 3.2!\n");
		exit(1);
	}


	Initialize();


	glutMainLoop();

	// Cleaning
	glDeleteProgram( program );
	glDeleteBuffers( 1, &vBuffer_coord );
	glDeleteBuffers( 1, &vBuffer_color );
	glDeleteVertexArrays( 1, &vArray );

	return 0;
}
