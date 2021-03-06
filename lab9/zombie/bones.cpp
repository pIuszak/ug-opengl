#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "obj_loader.hpp"
#include "texture_loader.hpp"
#include "shader_stuff.h"



glm::mat4x4 matProj;
glm::mat4x4 matView;




enum {
	ZIEMIA = 0,
    TORSO = 1,
    LEG_R = 2,
    LEG_L = 3,
    NUMBER_OF_OBJECTS = 4
};

int Frame = 0;



// ---------------------------------------

GLuint program;
GLuint vBuffer_pos[NUMBER_OF_OBJECTS];
GLuint vBuffer_uv[NUMBER_OF_OBJECTS];
GLuint vArray[NUMBER_OF_OBJECTS];
GLuint TextureID;

std::vector<glm::vec3> OBJ_vertices[NUMBER_OF_OBJECTS];
std::vector<glm::vec2> OBJ_uvs[NUMBER_OF_OBJECTS];
std::vector<glm::vec3> OBJ_normals[NUMBER_OF_OBJECTS];





// -----------------------------------------
class CBone
{

public:


	// potok opengl
	GLuint VAO;
    int VBO_Size;

    // macierz
    glm::mat4x4 matModel;

    // nast�pny obiekt typu CBone w strukturze
    CBone *next = NULL;

    // ustawienie potoku
    void Set(GLuint _vao, int _size)
    {
		matModel = glm::mat4(1.0);
        VAO = _vao;
        VBO_Size = _size;
    }

    // rysowanie na scenie
    void Draw(GLuint _prog, glm::mat4 _stackModel = glm::mat4(1.0))
    {

        glm::mat4 localModel = _stackModel * matModel;

        glUniformMatrix4fv( glGetUniformLocation( _prog, "matModel" ), 1, GL_FALSE, glm::value_ptr(localModel) );

        glBindVertexArray( VAO );
        glDrawArrays( GL_TRIANGLES, 0, VBO_Size );
        glBindVertexArray( 0 );

        if(next)
        {
            next->Draw(_prog, localModel);
        }

    }

    void Obroc(float angle, float x, float y, float z)
    {
        this->matModel = glm::rotate(matModel, angle, glm::vec3(x, y, z));
    }

	void Przesun(float x, float y, float z)
	{

	    this->matModel = glm::translate(matModel, glm::vec3(x, y, z));
	}


};



CBone Torso;
CBone RightArm;
CBone RightLeg;
CBone LeftArm;
CBone LeftLeg;





// ---------------------------------------
void DisplayScene()
{

	// 0. Czyszczenie ramki
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    glUseProgram( program );

    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);
    glUniform1i(glGetUniformLocation(program, "tex0"), 0);


	// 1. Geometria sceny
	matView = glm::translate(glm::mat4x4( 1.0 ), glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );

	glUniformMatrix4fv( glGetUniformLocation( program, "matProj" ), 1, GL_FALSE, glm::value_ptr(matProj) );
	glUniformMatrix4fv( glGetUniformLocation( program, "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );


	// Ziemia
	glm::mat4 matModel = glm::mat4(1.0);
	glUniformMatrix4fv( glGetUniformLocation( program, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );
	glBindVertexArray( vArray[ZIEMIA] );
	glDrawArrays( GL_TRIANGLES, 0,  OBJ_vertices[ZIEMIA].size() );
	glBindVertexArray( 0 );




	// DZWIG
	Torso.Draw(program);


    glUseProgram( 0 );
	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}
float leftLeg1 = 0;
float leftLeg2 = 0;
float rightLeg1;
float rightLeg2;
// --------------------------------------------------------------
void Keyboard( unsigned char key, int x, int y )
{
    float move_vec = 0.2;

    switch(key)
    {
		case 27:	// ESC key
			exit(0);
			break;

		case 'w':

		    if(leftLeg1 > -0.5){
                LeftLeg.Obroc(-0.1, 1.0, 0.0, 0.0);
                RightLeg.Obroc(0.2, 1.0, 0.0, 0.0);
                leftLeg1-= 0.1;
                printf("1");
                break;
		    }

            if(rightLeg1 > -0.5){
                LeftLeg.Obroc(0.1, 1.0, 0.0, 0.0);
                RightLeg.Obroc(-0.2, 1.0, 0.0, 0.0);
                rightLeg1-= 0.1;
                printf("2");
                break;
            }

            if(leftLeg2 <= 0.5){
                LeftLeg.Obroc(0.1, 1.0, 0.0, 0.0);
                RightLeg.Obroc(-0.2, 1.0, 0.0, 0.0);
                leftLeg2+= 0.1;
                printf("3");
                break;
            }
            if(rightLeg2 <= 0.5){
                LeftLeg.Obroc(-0.1, 1.0, 0.0, 0.0);
                RightLeg.Obroc(0.2, 1.0, 0.0, 0.0);
                rightLeg2+= 0.1;
                printf("4");
                break;
            }

            leftLeg2= 0;
            leftLeg1= 0;
            rightLeg2= 0;
            rightLeg1= 0;

            Torso.Przesun(0,0,0.5);

		break;

//		case 's':
//            RightArm.Obroc(-0.1, 0.0, 0.0, 1.0);
//		break;
//
//		case 'a':
//			Torso.Obroc(0.1, 0.0, 1.0, 0.0);
//		break;
//
//		case 'd':
//			Torso.Obroc(-0.1, 0.0, 1.0, 0.0);
//		break;
//
//		case 'q':
//            LeftArm.Obroc(0.5, 0.0, 0.0, 1.0);
//			break;
//
//		case 'e':
//            LeftArm.Obroc(-0.5, 0.0, 0.0, 1.0);


    }

    glutPostRedisplay();
}

// ---------------------------------------------------
void Initialize()
{
	// Odsuwanie widoku (na potrzeby przykladu)
	_scene_translate_z = -8;
	_scene_translate_y = -3;
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
	glEnable( GL_DEPTH_TEST );

	// tekstury
	int tex_width;
	int tex_height;
	unsigned char *tex_data;

	loadBMP_custom("zombie.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);






	// Programowanie potoku
	program = glCreateProgram();

	glAttachShader( program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram( program );


	if (!loadOBJ("obj/ground.obj", OBJ_vertices[ZIEMIA], OBJ_uvs[ZIEMIA], OBJ_normals[ZIEMIA]))
	{
		printf("Not loaded!\n");
		exit(1);
	}
	// Vertex arrays
	glGenVertexArrays( 1, &vArray[ZIEMIA] );
	glBindVertexArray( vArray[ZIEMIA] );

	glGenBuffers( 1, &vBuffer_pos[ZIEMIA] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[ZIEMIA] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[ZIEMIA].size() * sizeof(glm::vec3), &(OBJ_vertices[ZIEMIA])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uv[ZIEMIA] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[ZIEMIA] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_uvs[ZIEMIA].size() * sizeof(glm::vec2), &(OBJ_uvs[ZIEMIA])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );


	//Ziemia.Set(program, vArray[ZIEMIA], OBJ_vertices[ZIEMIA].size() );


	if (!loadOBJ("torso.obj", OBJ_vertices[TORSO], OBJ_uvs[TORSO], OBJ_normals[TORSO]))
	{
		printf("Not loaded!\n");
		exit(1);
	}
	// Vertex arrays
	glGenVertexArrays( 1, &vArray[TORSO] );
	glBindVertexArray( vArray[TORSO] );

	glGenBuffers( 1, &vBuffer_pos[TORSO] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[TORSO] );
	glBufferData(GL_ARRAY_BUFFER, OBJ_vertices[TORSO].size() * sizeof(glm::vec3), &(OBJ_vertices[TORSO])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uv[TORSO] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[TORSO] );
	glBufferData(GL_ARRAY_BUFFER, OBJ_uvs[TORSO].size() * sizeof(glm::vec2), &(OBJ_uvs[TORSO])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );



    if (!loadOBJ("leftleg.obj", OBJ_vertices[LEG_L], OBJ_uvs[LEG_L], OBJ_normals[LEG_L]))
    {
        printf("Not loaded!\n");
        exit(1);
    }
    // Vertex arrays
    glGenVertexArrays( 1, &vArray[LEG_L] );
    glBindVertexArray( vArray[LEG_L] );

    glGenBuffers( 1, &vBuffer_pos[LEG_L] );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[LEG_L] );
    glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[LEG_L].size() * sizeof(glm::vec3), &(OBJ_vertices[LEG_L])[0], GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 0 );

    glGenBuffers( 1, &vBuffer_uv[LEG_L] );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[LEG_L] );
    glBufferData( GL_ARRAY_BUFFER, OBJ_uvs[LEG_L].size() * sizeof(glm::vec2), &(OBJ_uvs[LEG_L])[0], GL_STATIC_DRAW );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );


    if (!loadOBJ("rightleg.obj", OBJ_vertices[LEG_R], OBJ_uvs[LEG_R], OBJ_normals[LEG_R]))
    {
        printf("Not loaded!\n");
        exit(1);
    }
    // Vertex arrays
    glGenVertexArrays( 1, &vArray[LEG_R] );
    glBindVertexArray( vArray[LEG_R] );

    glGenBuffers( 1, &vBuffer_pos[LEG_R] );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[LEG_R] );
    glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[LEG_R].size() * sizeof(glm::vec3), &(OBJ_vertices[LEG_R])[0], GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 0 );

    glGenBuffers( 1, &vBuffer_uv[LEG_R] );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[LEG_R] );
    glBufferData( GL_ARRAY_BUFFER, OBJ_uvs[LEG_R].size() * sizeof(glm::vec2), &(OBJ_uvs[LEG_R])[0], GL_STATIC_DRAW );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );


	// 1. Stworzenie kosci
	Torso.Set(vArray[TORSO], OBJ_vertices[TORSO].size() );
	LeftLeg.Set(vArray[LEG_L], OBJ_vertices[LEG_L].size() );
	RightLeg.Set(vArray[LEG_R], OBJ_vertices[LEG_R].size() );

	// 2. Ustalenie hierarchii

	//TODO change this hierarchy somehow

	Torso.next = &RightArm;
    RightArm.next = &LeftArm;

    LeftArm.next = &LeftLeg;
    LeftLeg.next = &RightLeg;





	// 3. Ustawlenie rozmieszczenia obiektow wzgledem bazowego
	Torso.matModel = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 1.8, 0.0));
	RightArm.matModel = glm::translate(glm::mat4(1.0), glm::vec3(-0.3, -0.28, -0.1));
	LeftArm.matModel = glm::translate(glm::mat4(1.0), glm::vec3(0.6, 0.0, 0.0));
    LeftLeg.matModel = glm::translate(glm::mat4(1.0), glm::vec3(-0.3, -0.5, 0.2));
    RightLeg.matModel = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));

    //RightArm.Obroc(-0.5, 0.0, 1.0, 0.0);
    //PropellerSecondary.Obroc(0.6, 0.0, 0.0, 1.0);
}




// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( 800, 600 );
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
