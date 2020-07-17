#include <stdio.h>
#include <GL/freeglut.h>
#include <math.h>

float squareX = 0;
float squareY = 0;

int WindowSizeX = 800;
int WindowSizeY = 600;

float mouseX;
float mouseY;


// --------------------------------------------------------------
void DrawingFunc(void)
{

    glClear( GL_COLOR_BUFFER_BIT );

    glColor3f( 1, 0, 0 );

    glBegin(GL_TRIANGLES);
    glVertex2f( -1.0, -1.0 );
    glVertex2f(  1.0, -1.0 );
    glVertex2f(  0,    1.0 );
    glEnd();

    glFlush();
}


void Rectangle(float x, float y, float r, float g, float b) {

    glColor3f(r, g, b);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-x, -y);
    glVertex2f(x, -y);
    glVertex2f(x, y);
    glVertex2f(-x, y);
    glEnd();


}
void Rectangle(float x, float y, float size) {

    glBegin(GL_LINE_LOOP);
    glVertex2f(x - size, y -size);
    glVertex2f(x + size, y - size);
    glVertex2f(x + size, y + size);
    glVertex2f(x - size, y + size);
    glEnd();

}
void Reactangle(void){

    glClear( GL_COLOR_BUFFER_BIT );

    glColor3f(1,0,0);
    Rectangle(((mouseX / WindowSizeX) * 2) - 1, -(((mouseY / WindowSizeY) * 2) - 1), 0.1f);
    glFlush();
}

void DrawMovingSquare(void){
    glClear( GL_COLOR_BUFFER_BIT );
    Rectangle(squareX,squareY,0.1);

    glFlush();
}


void DrawRandomRectangles(void) {

    glPopMatrix();
    glClear( GL_COLOR_BUFFER_BIT );
    for (int i = 0; i < 100; ++i) {
        Rectangle(static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2) - 1,
                  static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2) - 1,
                  static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2) - 1,
                  static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2) - 1,
                  static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2) - 1);
    }
    glFlush();
}
void DrawTriangleFan(void){
    glClear( GL_COLOR_BUFFER_BIT );
    glBegin(GL_TRIANGLE_FAN);

    glVertex2f( -1.0f, -1.0f );
    glVertex2f( -1.0f, 0.0f );
    glVertex2f( 0.0f, 1.0f );
    glVertex2f( -1.0f, -1.0f );
    glVertex2f( 1.0f, 0.0f );
    glVertex2f( 0.0f, 1.0f );

    glEnd();
    glFlush();
}

void DrawTriangleStrip(void){

    //Vertices below are in Clockwise orientation
    //Default setting for glFrontFace is Counter-clockwise
    glFrontFace(GL_CW);
    glBegin(GL_TRIANGLE_STRIP);

    glVertex3f( 0.0f, 1.0f, 0.0f ); //vertex 1
    glVertex3f( 0.0f, 0.0f, 0.0f ); //vertex 2
    glVertex3f( 1.0f, 1.0f, 0.0f ); //vertex 3
    glVertex3f( 1.5f, 0.0f, 0.0f ); //vertex 4
    glEnd();
    glFlush();
}

void DrawCircle(void)
{
    glClear( GL_COLOR_BUFFER_BIT );
    glBegin(GL_LINE_LOOP);
    for(int ii = 0; ii < 100; ii++)
    {
        float theta = 2.0f * 3.1415926f * float(ii) / float(100);//get the current angle

        float x = 0.25 * cosf(theta);//calculate the x component
        float y = 0.25 * sinf(theta);//calculate the y component

        glVertex2f(x, y);//output vertex

    }
    glEnd();
    glFlush();
}

// --------------------------------------------------------------
void DrawShapes(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 0, 0);
    glBegin(GL_LINE);
    glVertex2f(-0.5, -0.5);
    glVertex2f(0.5, -0.5);
    Rectangle(0.5,0.5,0.5);
    glEnd();
    glFlush();
}
// -----------------------------------------------------
void KeyboardFunc( unsigned char key, int x, int y )
{
    printf("ASCII key=%d, mouse (%d,%d)\n", key, x, y);
    if ( key == ' ')
    {
        printf("Space bar...\n");
    }
    if ( key == 27 )
    {
        printf("ESC key!\n");
    }
    // ...

    glutPostRedisplay();
}

// -----------------------------------------------------
void SpecialFunc( int key, int x, int y )
{
    switch( key ) {

        case GLUT_KEY_RIGHT:
            squareX += 0.1f;
            break;

        case GLUT_KEY_LEFT:
            squareX -= 0.1f;
            break;

        case GLUT_KEY_UP:
            squareY += 0.1f;
            break;

        case GLUT_KEY_DOWN:
            squareY -= 0.1f;
            break;
    }
    glutPostRedisplay();
}
void MousePosition(void){

    glClear( GL_COLOR_BUFFER_BIT );

    glColor3f(1,1,1);

    glBegin(GL_POINTS);

    glVertex2f(((mouseX / WindowSizeX) * 2) - 1, -(((mouseY / WindowSizeY) * 2) - 1));

    glEnd();
    glFlush();
}

void GetInput(int x, int y){
    mouseX = x;
    mouseY = y;
}
// --------------------------------------------------------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(WindowSizeX, WindowSizeY);
    if (glutCreateWindow("Hello world!") < 1)
    {
        printf("glutCreateWindow() failed!\n");
        exit(1);
    }
    glutMotionFunc(GetInput);
    // Setting drawing function
    //glutDisplayFunc(DrawingFunc);

    //ex.1
    //glutDisplayFunc(DrawingFunc);

    //ex.2
    //glutDisplayFunc(DrawShapes);

    //ex.3
    //glutDisplayFunc(DrawRandomRectangles);

    //ex.4
    //glutDisplayFunc(DrawTriangleStrip);

    //ex.5
    //glutDisplayFunc(DrawTriangleFan);p

    //ex.6
    //glutDisplayFunc(DrawTriangleFan);

    //ex.7
    //glutDisplayFunc(DrawCircle);

    //ex.8
    //glutDisplayFunc(DrawMovingSquare);

    //ex.9
    glutDisplayFunc(Reactangle);



    glutMainLoop();

    return 0;
}



