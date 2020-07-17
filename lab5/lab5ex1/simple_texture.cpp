#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "texture_loader.hpp"
#include "shader_stuff.h"

// ---------------------------------------
GLuint program;
GLuint vBuffer_pos;
GLuint vBuffer_color;
GLuint vBuffer_uv;
GLuint vArray;




// ---------------------------------------
glm::mat4x4 Matrix_proj;	// projection matrix
glm::mat4x4 Matrix_mv;		// modelview matrix
glm::mat4x4 Matrix_proj_mv;	// projection*modelview matrix



// ---------------------------------------
GLfloat vertices_pos[] =
        {
                -1.0f, -1.0f, 0.0f,	// bottom left
                1.0f, -1.0f, 0.0f,	// bottom right
                0.0f,  1.0f, 0.0f, // upper
        };

GLfloat vertices_color[] =
        {
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f,
        };

GLfloat vertices_tex[] =
        {
                0.0f, 0.0f, // for bottom left vertex
                1.0f, 0.0f, // for bottom right vertex
                0.5f, 1.0f, // for upper vertex
        };

GLuint textures[2];

// ---------------------------------------
void DisplayScene()
{

    // 0. Czyszczenie ramki (jak zwykle)
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    // 1. Geometria sceny  (jak zwykle)
    Matrix_mv = glm::mat4x4( 1.0 );
    Matrix_mv = glm::translate( Matrix_mv, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
    Matrix_mv = glm::rotate( Matrix_mv, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
    Matrix_mv = glm::rotate( Matrix_mv, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
    Matrix_proj_mv = Matrix_proj * Matrix_mv;
    glUniformMatrix4fv( glGetUniformLocation( program, "Matrix_proj_mv" ), 1, GL_FALSE, glm::value_ptr(Matrix_proj_mv) );



    // 2. Wlaczanie/aktywowanie tekstur (NOWE!)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    // 3. Przekazanie identyfikatora samplera (w przypadku 1 tekstury opcjonalne)
    //glUniform1i(glGetUniformLocation(program, "tex0"), 0);
    glUniform1i(glGetUniformLocation(program, "tex0"), 0);
    glUniform1i(glGetUniformLocation(program, "tex1"), 1);

    // 4. Generowanie obiektow na ekranie (jak zwykle)
    glDrawArrays( GL_TRIANGLES, 0, 3*3 );



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

    // NOWE NOWE NOWE NOWE
    // -------------------------------------------------------
    // Ladowanie plikow BMP i tworzenie tekstur
    // 1. Wczytanie danych z pliku graficznego (BMP)
    int tex_width;
    int tex_height;
    unsigned char *tex_data;
    //unsigned char *tex_data2;

    // gotowa funkcja z pliku texture_loader.hpp
   // loadBMP_custom("bubbles.bmp", tex_width, tex_height, &tex_data);

    glGenTextures(2, textures);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glLoadIdentity();
    // 1st tex
    //glActiveTexture(GL_TEXTURE0);
    loadBMP_custom("bubbles.bmp", tex_width, tex_height, &tex_data);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
  // image = SOIL_load_image("sample.png", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
    //SOIL_free_image_data(image);
    //glUniform1i(glGetUniformLocation(program, "bubbles"), 0);
    glGenerateMipmap(GL_TEXTURE_2D);
    // 5. Ustawianie parametrow tekstury
    // Podstawowy level mipmap (default 0)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 2nd tex

    loadBMP_custom("uvtemplate.bmp", tex_width, tex_height, &tex_data);

    glBindTexture(GL_TEXTURE_2D, textures[1]);
  // image = SOIL_load_image("sample.png", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
    //SOIL_free_image_data(image);
    //glUniform1i(glGetUniformLocation(program, "tex1"),1);

    glGenerateMipmap(GL_TEXTURE_2D);
    // 5. Ustawianie parametrow tekstury
    // Podstawowy level mipmap (default 0)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);




    // 2. Generowanie identyfikatora i ustawianie jako aktualnej tekstury
    //glGenTextures(1, &textures[0]);
    //glBindTexture(GL_TEXTURE_2D, textures[0]);

    // 3. Przekazanie danych tekstury
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data2);
   // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);

    // 4. Automatyczne tworzenie mipmap
   // glGenerateMipmap(GL_TEXTURE_2D);



    // 5. Ustawianie parametrow tekstury

    // Podstawowy level mipmap (default 0)
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
//
//    // Parametry ustalajace zachowanie wyjsca poza wspolrzedne UV (0-1) Wraping
//
//    // GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//
//    //float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
//    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
//
//
//    // GL_LINEAR, GL_NEAREST
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    // GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST,
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);


    // -------------------------------------------------------
    // Reszta bez zmian



    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );


    // Programowanie potoku
    program = glCreateProgram();

    glAttachShader( program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
    glAttachShader( program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

    LinkAndValidateProgram( program );



    // Vertex arrays

    glGenVertexArrays( 1, &vArray );
    glBindVertexArray( vArray );

    // Wspolrzedne wierzchokow
    glGenBuffers( 1, &vBuffer_pos );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_pos), vertices_pos, GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 0 );

    // Kolor wierzcholkow
    glGenBuffers( 1, &vBuffer_color );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_color );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_color), vertices_color, GL_STATIC_DRAW );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 1 );

    // Wspolrzedne textury UV
    glGenBuffers( 1, &vBuffer_uv );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_tex), vertices_tex, GL_STATIC_DRAW );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 2 );






    glEnable( GL_DEPTH_TEST );

    glBindVertexArray( vArray );
    glUseProgram( program );

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
    glutDisplayFunc( DisplayScene );
    glutReshapeFunc( Reshape );
    glutMouseFunc( MouseButton );
    glutMotionFunc( MouseMotion );
    glutMouseWheelFunc( MouseWheel );
    glutKeyboardFunc( Keyboard );
    glutSpecialFunc( SpecialKeys );

    glutMainLoop();

    // Cleaning();
    glDeleteProgram( program );
    glDeleteBuffers( 1, &vBuffer_pos );
    glDeleteBuffers( 1, &vBuffer_uv );
    glDeleteBuffers( 1, &vBuffer_color );
    glDeleteVertexArrays( 1, &vArray );

    return 0;
}