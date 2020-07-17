#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/SOIL.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#include "shader_stuff.h"

// ---------------------------------------
// NOWE : OSWIETLENIE - parametry
// ---------------------------------------
typedef struct SLightParameters
{
	glm::vec4 Ambient;
	glm::vec4 Diffuse;
	glm::vec4 Specular;
	glm::vec4 Position;
	float Attenuation;

	glm::vec3 __offset;


} SLightParameters;


SLightParameters Light1 = {
	glm::vec4(0.3, 0.3, 0.3, 1.0),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	glm::vec4(0.0, 3.0, 0.0, 1.0),
	0.01f,
};


// ---------------------------------------
// NOWE : MATERIALY - parametry
// ---------------------------------------
typedef struct SMaterial
{

	glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    float Shininess;

} SMaterial;


SMaterial Material1 = {
	glm::vec3(0.8f, 0.8f, 0.2f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(0.7f, 0.7f, 0.7f),
	32.0f
};




// ---------------------------------------
GLuint program;
GLuint vBuffer_coord;
GLuint vBuffer_normal;
GLuint vBuffer_uv;
GLuint vArrayTrees;
GLuint vArrayFlowers;
GLuint TextureID;

// ---------------------------------------
// Macierze przeksztalcen i rzutowania
// ---------------------------------------
glm::mat4x4 matProjection;
glm::mat4x4 matView;
//glm::mat4x4 matModel;
//glm::mat3x3 matNormal;

glm::vec3 Camera_Position;


//matNormal = glm::transpose(glm::inverse(glm::mat3(matModel)));


// --------------------------------------
std::vector<glm::vec3> OBJ_vertices;
std::vector<glm::vec2> OBJ_uvs;
std::vector<glm::vec3> OBJ_normals;

std::vector<glm::vec3> OBJ2_vertices;
std::vector<glm::vec2> OBJ2_uvs;
std::vector<glm::vec3> OBJ2_normals;



float treesX [100];
float treesY [100];

void randTrees(){
    for (int i = 0; i < 100; ++i) {
        treesX[i] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2) - 1)*10;
        treesY[i] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2) - 1)*10;
    }
}

// ---------------------------------------
void DisplayScene()
{
	// Czyszczenie ramki
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // ------------------------------
	// 1. Geometria sceny
	// Ustawiamy macierz jednostkowa



    for (int i = 0; i < 100; ++i) {
        matView = glm::mat4x4( 1.0 );
        matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
        matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
        matView = glm::translate( matView, glm::vec3( _scene_translate_x+treesX[i], _scene_translate_y -2 + 0.001 * +treesX[i], _scene_translate_z+treesY[i] ) );




        glUniformMatrix4fv( glGetUniformLocation( program, "matProjection" ), 1, GL_FALSE, glm::value_ptr(matProjection) );
        glUniformMatrix4fv( glGetUniformLocation( program, "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );

        // Nowe! wazne dla Specular light
       // Camera_Position = ExtractCameraPos(matView);
        glUniform3fv( glGetUniformLocation( program, "Camera_Position" ), 1, &Camera_Position[0] );

        glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );
    }


    matView = glm::mat4x4( 1.0 );
    matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
    matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
    matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y -2, _scene_translate_z) );


    glUniformMatrix4fv( glGetUniformLocation( program, "matProjection" ), 1, GL_FALSE, glm::value_ptr(matProjection) );
    glUniformMatrix4fv( glGetUniformLocation( program, "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );

    // Nowe! wazne dla Specular light
    // Camera_Position = ExtractCameraPos(matView);
    glUniform3fv( glGetUniformLocation( program, "Camera_Position" ), 1, &Camera_Position[0] );

    glDrawArrays( GL_TRIANGLES, 0, OBJ2_vertices.size() );
    glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
    glViewport( 0, 0, width, height );
	matProjection = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}

// ---------------------------------------------------
void Initialize()
{

	_scene_translate_z = -10.0f;

// 1
//	if (!loadOBJ("scene.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
//	{
//		printf("Not loaded!\n");
//		exit(1);
//	}
    // Ladowanie pliku OBJ

    if (!loadOBJ("flower.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
    {
        printf("Not loaded!\n");
        exit(1);
    }

        if (!loadOBJ("laubbaum.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
    {
        printf("Not loaded!\n");
        exit(1);
    }

    if (!loadOBJ("scene.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
    {
        printf("Not loaded!\n");
        exit(1);
    }
    // 2
    //glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

    // ---------------------------------------
    // Tworzenie tekstury <<--- NOWE

    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    int width, height;
    unsigned char* image;


    // NOWE :
    // Parametr: SOIL_LOAD_RGBA

    image = SOIL_load_image("tex1-min.png", &width, &height, 0, SOIL_LOAD_RGBA);
    if (image == NULL)
    {
        printf("Blad odczytu pliku graficznego!\n");
        exit(1);
    }


    // NOWE: parametr RGBA zamiast RGB dwukrotnie !
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    _scene_translate_z = -5;
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

    // 1. Programowanie potoku
	program = glCreateProgram();

	glAttachShader( program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram( program );



    // 2. Vertex arrays
    glGenVertexArrays( 1, &vArrayFlowers );
    glBindVertexArray( vArrayFlowers );

	// Wspolrzedne wierzchokow
    glGenBuffers( 1, &vBuffer_coord );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_coord );
    glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );


	// Wektory normalne
    glGenBuffers( 1, &vBuffer_normal );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_normal );
    glBufferData( GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &OBJ_normals[0], GL_STATIC_DRAW );

	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );


    // Wspolrzedne textury UV
    glGenBuffers( 1, &vBuffer_uv );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv );
    glBufferData( GL_ARRAY_BUFFER, OBJ_uvs.size() * sizeof(glm::vec2), &OBJ_uvs[0], GL_STATIC_DRAW );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 2 );



	glBindVertexArray( 0 );
	glEnable( GL_DEPTH_TEST );
    glEnable( GL_BLEND );
    glBindVertexArray( vArrayFlowers );

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray( vArrayFlowers );
    glUseProgram( program );

	glUniform3fv( glGetUniformLocation(program, "myMaterial.Ambient"), 1, &Material1.Ambient[0]);
	glUniform3fv( glGetUniformLocation(program, "myMaterial.Diffuse"), 1, &Material1.Diffuse[0]);
	glUniform3fv( glGetUniformLocation(program, "myMaterial.Specular"), 1, &Material1.Specular[0]);
	glUniform1f( glGetUniformLocation(program, "myMaterial.Shininess"), Material1.Shininess);

	glUniform4fv( glGetUniformLocation(program, "myLight.Ambient"), 1, &Light1.Ambient[0]);
	glUniform4fv( glGetUniformLocation(program, "myLight.Diffuse"), 1, &Light1.Diffuse[0]);
	glUniform4fv( glGetUniformLocation(program, "myLight.Specular"), 1, &Light1.Specular[0]);
	glUniform4fv( glGetUniformLocation(program, "myLight.Position"), 1, &Light1.Position[0]);
	glUniform1f( glGetUniformLocation(program, "myLight.Attenuation"), Light1.Attenuation);
}


void Animation(int f)
{
	glutPostRedisplay();
	glutTimerFunc(10, Animation, 0);
}

// ---------------------------------------------------
int main( int argc, char *argv[] )
{
    randTrees();
    // GLUT
    glutInit( &argc, argv );
    // NOWE !! GLUT_RGBA
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    glutInitContextVersion( 4, 0 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    glutInitWindowSize( 500, 500 );
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

	glutTimerFunc(10, Animation, 0);

    glutMainLoop();

    // Cleaning();
    glDeleteProgram( program );

    glDeleteBuffers( 1, &vBuffer_coord );
    glDeleteBuffers( 1, &vBuffer_normal );
    glDeleteVertexArrays( 1, &vArrayTrees );

    return 0;
}
