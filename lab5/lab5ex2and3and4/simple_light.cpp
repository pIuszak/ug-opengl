#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "texture_loader.hpp"
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "texture_loader.hpp"
#include "shader_stuff.h"

enum
{
    LIGHT_OFF = 0,
    LIGHT_DISTANCE,
    LIGHT_GOURAUD,
    LIGHT_PHONG,
    LIGHT_NORMAL,
    LIGHT_NUMBER,

};

int MODE = LIGHT_DISTANCE;

#include "obj_loader.hpp"
#include "shader_stuff.h"

// ---------------------------------------
GLuint program[LIGHT_NUMBER];
GLuint vBuffer_coord;
GLuint vBuffer_normal;
GLuint vBuffer_uv;
GLuint vArray;



// ---------------------------------------
glm::mat4x4 Matrix_proj;
glm::mat4x4 Matrix_mv;
glm::mat4x4 Matrix_proj_mv;


// ---------------------------------------
// Na potrzeby pliku OBJ
std::vector<glm::vec3> OBJ_vertices;
std::vector<glm::vec2> OBJ_uvs;
std::vector<glm::vec3> OBJ_normals;


GLuint textures[1];

// ---------------------------------------
// NOWE : OSWIETLENIE - parametry
// ---------------------------------------

glm::vec3 Light_Ambient = glm::vec3(0.2, 0.2, 0.2);
glm::vec3 Light_Diffuse = glm::vec3(1.0, 1.0, 1.0);
glm::vec3 Light_Position = glm::vec3(0.0, 1.0, -3.0);


// ---------------------------------------
void DisplayScene()
{

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glBindVertexArray( vArray );
    glUseProgram( program[MODE] );

    // ------------------------------------------------------
    // NOWE: OSWIETLENIE - przekazanie parametrow do shaderow
    // ------------------------------------------------------
    glUniform3fv( glGetUniformLocation( program[MODE], "Light_Ambient" ), 1, &Light_Ambient[0] );
    glUniform3fv( glGetUniformLocation( program[MODE], "Light_Diffuse" ), 1, &Light_Diffuse[0] );
    glUniform3fv( glGetUniformLocation( program[MODE], "Light_Position" ), 1, &Light_Position[0] );

    // 2. Wlaczanie/aktywowanie tekstur (NOWE!)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);


    // 3. Przekazanie identyfikatora samplera (w przypadku 1 tekstury opcjonalne)
    //glUniform1i(glGetUniformLocation(program, "tex0"), 0);
    glUniform1i(glGetUniformLocation(program[LIGHT_DISTANCE], "tex0"), 0);



// 2. Przekazanie identyfikatora samplera tekstury
// w przypadku jednej tekstury jest to opcjonalne




    // Geometria sceny
    Matrix_mv = glm::mat4x4( 1.0 );
    Matrix_mv = glm::translate( Matrix_mv, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
    Matrix_mv = glm::rotate( Matrix_mv, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
    Matrix_mv = glm::rotate( Matrix_mv, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
    Matrix_proj_mv = Matrix_proj * Matrix_mv;
    glUniformMatrix4fv( glGetUniformLocation( program[MODE], "Matrix_proj_mv" ), 1, GL_FALSE, glm::value_ptr(Matrix_proj_mv) );


    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );

    glUseProgram( 0 );
    glBindVertexArray( 0 );

    glutSwapBuffers();

}


// ---------------------------------------
void Reshape( int width, int height )
{
    glViewport( 0, 0, width, height );
    Matrix_proj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}

// ---------------------------------------------------
void Initialize()
{

    if (!loadOBJ("scene.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
    {
        printf("Not loaded!\n");
        exit(1);
    }

    _scene_translate_z = -10;

    glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );

    // 1. Programowanie potoku
    program[LIGHT_OFF] = glCreateProgram();
    glAttachShader( program[LIGHT_OFF], LoadShader(GL_VERTEX_SHADER, "off_vertex.glsl"));
    glAttachShader( program[LIGHT_OFF], LoadShader(GL_FRAGMENT_SHADER, "off_fragment.glsl"));
    LinkAndValidateProgram( program[LIGHT_OFF] );


    program[LIGHT_GOURAUD] = glCreateProgram();
    glAttachShader( program[LIGHT_GOURAUD], LoadShader(GL_VERTEX_SHADER, "gouraud_vertex.glsl"));
    glAttachShader( program[LIGHT_GOURAUD], LoadShader(GL_FRAGMENT_SHADER, "gouraud_fragment.glsl"));
    LinkAndValidateProgram( program[LIGHT_GOURAUD] );

    program[LIGHT_PHONG] = glCreateProgram();
    glAttachShader( program[LIGHT_PHONG], LoadShader(GL_VERTEX_SHADER, "phong_vertex.glsl"));
    glAttachShader( program[LIGHT_PHONG], LoadShader(GL_FRAGMENT_SHADER, "phong_fragment.glsl"));
    LinkAndValidateProgram( program[LIGHT_PHONG] );

    program[LIGHT_NORMAL] = glCreateProgram();
    glAttachShader( program[LIGHT_NORMAL], LoadShader(GL_VERTEX_SHADER, "normal_vertex.glsl"));
    glAttachShader( program[LIGHT_NORMAL], LoadShader(GL_FRAGMENT_SHADER, "normal_fragment.glsl"));
    LinkAndValidateProgram( program[LIGHT_NORMAL] );

    program[LIGHT_DISTANCE] = glCreateProgram();
    glAttachShader( program[LIGHT_DISTANCE], LoadShader(GL_VERTEX_SHADER, "pluszak_vertex.glsl"));
    glAttachShader( program[LIGHT_DISTANCE], LoadShader(GL_FRAGMENT_SHADER, "pluszak_fragment.glsl"));
    LinkAndValidateProgram( program[LIGHT_DISTANCE] );


    // tex -----------
    glUniform1i(glGetUniformLocation(program[LIGHT_DISTANCE], "tex0"), 0);

    // 1. Wczytanie danych z pliku graficznego (BMP)
    int tex_width;
    int tex_height;
    unsigned char *tex_data;
    //unsigned char *tex_data2;

    // gotowa funkcja z pliku texture_loader.hpp
    // loadBMP_custom("bubbles.bmp", tex_width, tex_height, &tex_data);

    glGenTextures(1, textures);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glLoadIdentity();
    //glActiveTexture(GL_TEXTURE0);
    loadBMP_custom("g.bmp", tex_width, tex_height, &tex_data);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, );
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //--------------------

    // Vertex arrays

    glGenVertexArrays( 1, &vArray );
    glBindVertexArray( vArray );

    // Wspolrzedne wierzchokow
    glGenBuffers( 1, &vBuffer_coord );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_coord );
    glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 0 );

    // Wspolrzedne textury UV
    glGenBuffers( 1, &vBuffer_uv );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv );
    glBufferData( GL_ARRAY_BUFFER, OBJ_uvs.size() * sizeof(glm::vec2), &OBJ_uvs[0], GL_STATIC_DRAW );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 1 );

    // Wektory normalne
    glGenBuffers( 1, &vBuffer_normal );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_normal );
    glBufferData( GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &OBJ_normals[0], GL_STATIC_DRAW );
    glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 2 );




    glEnable( GL_DEPTH_TEST );

    glBindVertexArray( vArray );
    glUseProgram( program[LIGHT_DISTANCE] );

}


void fog()
{
    glClearColor(.5,.5,.5,1.);      //background color and alpha
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(45,640.0/480.0,1.0,500.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    //cube = loadObject("test.obj");
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START,1.);
    glFogf(GL_FOG_END,100.);
//      glFogi(GL_FOG_MODE, GL_EXP2);
//      glFogf(GL_FOG_DENSITY, .3);
    float color[] = {.5,.5,.5,1.};
    glFogfv(GL_FOG_COLOR, color);
    float col[] = {1.,1.,1.,1.};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, col);
}

// ---------------------------------------------------
int main( int argc, char *argv[] )
{
    // GLUT
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitContextVersion( 3, 2 );
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
    fog();
    glutDisplayFunc( DisplayScene );
    glutReshapeFunc( Reshape );
    glutMouseFunc( MouseButton );
    glutMotionFunc( MouseMotion );
    glutMouseWheelFunc( MouseWheel );
    glutKeyboardFunc( Keyboard );
    glutSpecialFunc( SpecialKeys );

    glutMainLoop();

    // Cleaning();
    glDeleteProgram( program[0] );
    glDeleteProgram( program[1] );
    glDeleteProgram( program[2] );
    glDeleteBuffers( 1, &vBuffer_coord );
    glDeleteBuffers( 1, &vBuffer_normal );
    glDeleteVertexArrays( 1, &vArray );

    return 0;
}