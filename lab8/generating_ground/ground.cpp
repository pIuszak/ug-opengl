#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#include "texture_loader.hpp"
#include "shader_stuff.h"


enum {
	SCENE = 0,
	TREE,
	NUMBER_OF_BUFFERS,
};

enum {
	PROGRAM_SCENE = 0,
	PROGRAM_TREE = 1,
	NUMBER_OF_PROGRAMS,
};



// ---------------------------------------
GLuint program[NUMBER_OF_PROGRAMS];
GLuint vBuffer_pos[NUMBER_OF_BUFFERS];
GLuint vBuffer_uv[NUMBER_OF_BUFFERS];
GLuint vBuffer_normal[NUMBER_OF_BUFFERS];
GLuint vArray[NUMBER_OF_BUFFERS];


// ---------------------------------------
glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 Matrix_proj_mv;
glm::mat4x4 matModel(1.0);


// ---------------------------------------
std::vector<glm::vec3> OBJ_vertices[2];
std::vector<glm::vec2> OBJ_uvs[2];
std::vector<glm::vec3> OBJ_normals[2];


GLuint TextureID[NUMBER_OF_BUFFERS];


// ----------------------------------------------------------------
class CSceneObject
{

public:

    glm::vec3 Position;     // pozycja obiektu na scenie


    GLuint VAO;             // potok openGL
    int VBO_Size;
    GLuint Program;
    glm::mat4x4 matModel;


    CSceneObject()
    {
    }


    // ustawienie potoku
    void Set(GLuint _prog, GLuint _vao, int _size)
    {
        Program = _prog;
        VAO = _vao;
        VBO_Size = _size;
    }

    // rysowanie na scenie
    void Draw()
    {

        glUseProgram(Program);
        glUniformMatrix4fv( glGetUniformLocation( Program, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );

        glBindVertexArray( VAO );
        glDrawArrays( GL_TRIANGLES, 0, VBO_Size );
        glBindVertexArray( 0 );

    }

    // ustawienie pozycji na scenie
    void SetPosition(float x, float y, float z)
    {
        Position = glm::vec3(x, y, z);
        matModel = glm::translate(glm::mat4(1.0), Position);
    }

    // zmiana pozycji na scenie
    void MoveXZ(float _x, float _z)
    {
        Position += glm::vec3(_x, 0.0, _z);
        matModel = glm::translate(glm::mat4(1.0), Position);
    }

};

// -------------------------------
class CTriangle
{
public:

	// wspolrzedne punktow trojkata
	// uwaga! os pionowa w tym wypadku to z
	glm::vec3 p[3];


	// rownanie plaszczyzny Ax + By + Cz + D = 0
	float A,B,C,D;

	CTriangle() { }
	CTriangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
	{
		p[0] = p1;
		p[1] = p2;
		p[2] = p3;

		calculateEquation();
	}

	// obliczanie wspolczynnikow rownania
	void calculateEquation(void)
	{
		glm::vec3 v1, v2;

		v1 = p[1] - p[0];
		v2 = p[2] - p[0];

		A = v1.y * v2.z - v1.z * v2.y;	 // A
		B = v1.z * v2.x - v1.x * v2.z;   // B
		C = v1.x * v2.y - v1.y * v2.x;   // C


		D = - (A*p[0].x + B*p[0].y + C*p[0].z);	// D

		if (C == 0)
		{
			printf("Uwaga! Trojkat pionowy.\n");
		}

	}

	// czy punkt p1 jest po lewej stronie odcinka p2-p3
	inline float sign (glm::vec2 p1, glm::vec3 p2, glm::vec3 p3)
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}

	// czy punkt jest wewnatrz trojkata rzutowanego na plaszczyzne xy
	inline bool isInside (glm::vec2 point)
	{
		float s1, s2, s3;
		bool has_neg, has_pos;

		s1 = sign(point, p[0], p[1]);
		s2 = sign(point, p[1], p[2]);
		s3 = sign(point, p[2], p[0]);

		has_neg = (s1 < 0) || (s2 < 0) || (s3 < 0);
		has_pos = (s1 > 0) || (s2 > 0) || (s3 > 0);

		return !(has_neg && has_pos);
	}

	// obliczamy wysokosc punktu w trojkacie
	inline float altitude(glm::vec2 point)
	{
		if (C) return -(A*point.x + B*point.y + D)/C;
		else { return 99999; }
	}

};



// ---------------------------------------
class CGround
{
public:

	int Number_of_triangles = 0;
	std::vector<CTriangle> triangles;


	// stworzenie struktury z listy trojkatow
	void CreateFromOBJ(std::vector<glm::vec3> vert)
	{
		glm::vec3 p1,p2,p3;

		std::vector<glm::vec3>::iterator it = vert.begin();
		while (it != vert.end())
		{
			p1 = *it++;
			p2 = *it++;
			p3 = *it++;
			// uwaga ! zamiana wspolrzednych, tak aby z byl wysokoscia
			glm::vec3 np1 = glm::vec3 (p1.x,p1.z,p1.y);
			glm::vec3 np2 = glm::vec3 (p2.x,p2.z,p2.y);
			glm::vec3 np3 = glm::vec3 (p3.x,p3.z,p3.y);
			triangles.push_back(CTriangle(np1, np2, np3));

		}

		Number_of_triangles = triangles.size();
		printf("Created CGround of %d triangles.\n", Number_of_triangles);
	}

	// obliczenie wysokosci
	float getAltitute(glm::vec2 point)
	{

		// obliczenie listy trojkatow nad ktorymi jestesmy
		CTriangle *collTriangle = NULL;


		std::vector<CTriangle>::iterator it;
		for (it = triangles.begin(); it != triangles.end(); it++)
		{
			if ((*it).isInside(point))
			{
				collTriangle = &(*it);
				break;
			}
		}

		// jesli zostal znaleziony
		if (collTriangle)
		{
			return collTriangle->altitude(point);
		}
		else
		{
			printf("Brak ziemi pod nogami!\n");
			return 0.0;
		}
	}

};



CGround myGround;

float treesX [100];
float treesY [100];
bool randomBool()
{
    return 0 + (rand() % (1 - 0 + 1)) == 1;
}
void randTrees(){
    for (int i = 0; i < 100; ++i) {
        treesX[i] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2) - 1)*10;
        treesY[i] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2) - 1)*10;
    }
}

CSceneObject Trees [100];
// ---------------------------------------
void DisplayScene()
{


	// Macierz widoku
	matView = glm::mat4x4( 1.0 );
	matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );

    // Macierz modelu
    matModel =  glm::mat4x4( 1.0 );


	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// AKTYWUJEMY program
	glUseProgram( program[PROGRAM_SCENE] );


		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matProj" ), 1, GL_FALSE, glm::value_ptr(matProj) );
		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );
		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );


		// AKTYWUJEMY tekstury
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(program[PROGRAM_SCENE], "tex0"), 0);

    glUseProgram( program[PROGRAM_TREE] );


    glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_TREE], "matProj" ), 1, GL_FALSE, glm::value_ptr(matProj) );
    glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_TREE], "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );
    glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_TREE], "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );


    // AKTYWUJEMY tekstury
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(program[PROGRAM_TREE], "tex0"), 0);


		// Scena
		glBindVertexArray( vArray[SCENE] );
		glBindTexture(GL_TEXTURE_2D, TextureID[SCENE]);
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[SCENE].size() );


		glBindTexture(GL_TEXTURE_2D, TextureID[TREE]);
		glBindVertexArray( vArray[TREE] );
		glBindTexture(GL_TEXTURE_2D, TextureID[TREE]);
		//glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[TREE].size() );


    for (int i = 0; i < 100; ++i) {
        Trees[i].Draw();
    }


    // WYLACZAMY program
    glUseProgram(0);



	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}

// --------------------------------------------------------------
void Keyboard( unsigned char key, int x, int y )
{

    switch(key)
    {
		case 27:	// ESC key
			exit(0);
			break;

    }

    glutPostRedisplay();
}

// ---------------------------------------------------
void Initialize()
{
	_scene_translate_z = -5;
	_scene_translate_y = -2;

	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

    // ---------------------------------------
	// Ladowanie pliku OBJ
	if (!loadOBJ("scene-plane.obj", OBJ_vertices[SCENE], OBJ_uvs[SCENE], OBJ_normals[SCENE]))
	{
		printf("Not loaded!\n");
		exit(1);
	}


	// ---------------------------------------
	// Tworzenie tekstury
	int tex_width;
	int tex_height;
	unsigned char *tex_data;
	loadBMP_custom("chess.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID[SCENE]);
	glBindTexture(GL_TEXTURE_2D, TextureID[SCENE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);



	// Tworzenie potoku OpenGL
	program[PROGRAM_SCENE] = glCreateProgram();
	glAttachShader( program[PROGRAM_SCENE], LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( program[PROGRAM_SCENE], LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( program[PROGRAM_SCENE] );

    // Tworzenie potoku OpenGL
    program[PROGRAM_TREE] = glCreateProgram();
    glAttachShader( program[PROGRAM_TREE], LoadShader(GL_VERTEX_SHADER, "vertexTree.glsl"));
    glAttachShader( program[PROGRAM_TREE], LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
    LinkAndValidateProgram( program[PROGRAM_TREE] );


	// SCENA
	glGenVertexArrays( 1, &vArray[SCENE] );
	glBindVertexArray( vArray[SCENE] );

	glGenBuffers( 1, &vBuffer_pos[SCENE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[SCENE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[SCENE].size() * sizeof(glm::vec3), &(OBJ_vertices[SCENE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uv[SCENE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[SCENE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_uvs[SCENE].size() * sizeof(glm::vec2), &(OBJ_uvs[SCENE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );


    // Inne ustawienia openGL i sceny
	glEnable( GL_DEPTH_TEST );




    // SPHERE
    if (!loadOBJ("tree.obj", OBJ_vertices[TREE], OBJ_uvs[TREE], OBJ_normals[TREE]))
	{
		printf("Not loaded!\n");
		exit(1);
	}
	// Vertex arrays
	glGenVertexArrays( 1, &vArray[TREE] );
	glBindVertexArray( vArray[TREE] );

	glGenBuffers( 1, &vBuffer_pos[TREE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[TREE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[TREE].size() * sizeof(glm::vec3), &(OBJ_vertices[TREE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uv[TREE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[TREE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_uvs[TREE].size() * sizeof(glm::vec2), &(OBJ_uvs[TREE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );

    loadBMP_custom("grass.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID[TREE]);
	glBindTexture(GL_TEXTURE_2D, TextureID[TREE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Ziemia
    myGround.CreateFromOBJ(OBJ_vertices[SCENE]);
    // Inicjalizacja obiektow

    for (int i = 0; i < myGround.Number_of_triangles/2; ++i) {

        Trees[i].Set(program[SCENE], vArray[TREE], OBJ_vertices[TREE].size() );

        if (randomBool()){
            Trees[i].SetPosition(myGround.triangles[i].p[0].x,myGround.triangles[i].p[0].z, myGround.triangles[i].p[0].y);
        }



    }







}



// ---------------------------------------------------
int main( int argc, char *argv[] )
{
    randTrees();
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( 800, 600);
	glutCreateWindow( "OpenGL" );

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
	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutMouseWheelFunc( MouseWheel );
	glutKeyboardFunc( Keyboard );
	glutSpecialFunc( SpecialKeys );

	glutMainLoop();


	return 0;
}
