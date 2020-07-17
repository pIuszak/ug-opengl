#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#define GLM_SWIZZLE

#include <GL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#include "texture_loader.hpp"
#include "shader_stuff.h"
#include "text.h"

#define ZOMBIES 100
#define PI 3.14159

GLuint Window_width = 800;
GLuint Window_height = 600;

void UpdateZombie(float step);

class CGround;

enum {
    SCENE,
    ARMS,
    TREE,
    TORSO,
    KEY,
    GATE,
    LEG_R,
    LEG_L,
    WINDMILL,
    PROPELLER_MAIN,
    PROPELLER_SECONDARY,
    NUMBER_OF_BUFFERS
};

enum {
    PROGRAM_SCENE,
    NUMBER_OF_PROGRAMS
};

//angle of rotation
float xpos = 0, ypos = 0, zpos = 0, angle = 0.0;
int GLOBAL_argc;
char *GLOBAL_argv[10000];
float lastx, lasty;

int KEYS = 3;
// ---------------------------------------
// Swiatlo kierunkowe
glm::vec3 Light_Direction = glm::normalize(glm::vec3(0.2, -0.8f, 1.1f));
glm::vec3 Light_Position = glm::vec3(0.0f, 5.0f, -25.0f);
glm::vec3 Camera_Position;
glm::mat4 lightProj = glm::ortho(-30.0f, 30.0f, -30.0f, 40.0f, 1.0f, 40.5f);

glm::mat4 lightView = glm::lookAt(Light_Position,
                                  Light_Position + Light_Direction,
                                  glm::vec3(0.0f, 1.0f, 0.0f));


// ---------------------------------------
// Cienie
GLuint DepthMap_Program;
GLuint DepthMap_FrameBuffer;
GLuint DepthMap_Texture;
const unsigned int DepthMap_Width = 2024, DepthMap_Height = 2024;

// -------------------------------------------------------


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
std::vector<glm::vec3> OBJ_vertices[NUMBER_OF_BUFFERS];
std::vector<glm::vec2> OBJ_uvs[NUMBER_OF_BUFFERS];
std::vector<glm::vec3> OBJ_normals[NUMBER_OF_BUFFERS];


GLuint TextureID[NUMBER_OF_BUFFERS];

class CSceneObject;

// ----------------------------------------------------------------
class CSceneObject {

public:

    glm::vec3 Position;     // pozycja obiektu na scenie
    glm::vec3 Rotation;     // pozycja obiektu na scenie


    GLuint VAO;             // potok openGL
    int VBO_Size;
    GLuint Program;
    glm::mat4x4 matModel;


    CSceneObject() {
    }

    float radius = 2.0f;

    // ustawienie potoku
    void Set(GLuint _prog, GLuint _vao, int _size) {
        Program = _prog;
        VAO = _vao;
        VBO_Size = _size;
    }

    // rysowanie na scenie
    void Draw(GLuint _prog = 0) {

        if (_prog == 0) {
            glUseProgram(Program);
            glUniformMatrix4fv(glGetUniformLocation(Program, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
        } else {
            glUseProgram(_prog);
            glUniformMatrix4fv(glGetUniformLocation(_prog, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
        }


        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, VBO_Size);
        glBindVertexArray(0);

    }

    // ustawienie pozycji na scenie
    void SetPosition(float x, float y, float z) {
        Position = glm::vec3(x, y, z);
        matModel = glm::translate(glm::mat4(1.0), Position);

    }

    void SetRotation(float w, float x, float y, float z) {
        matModel = glm::rotate(matModel, w, glm::vec3(x, y, z));
    }

    // zmiana pozycji na scenie
    void MoveXZ(float _x, float _z) {
        Position += glm::vec3(_x, 0.0, _z);
        matModel = glm::translate(glm::mat4(1.0), Position);
    }

    bool isCollision(const CSceneObject &other) {
        float dist = glm::distance(this->Position, other.Position);
        if (dist < this->radius + other.radius) {

            return true;
        }
        return false;
    }

};


// -------------------------------
class CTriangle {
public:

    // wspolrzedne punktow trojkata
    // uwaga! os pionowa w tym wypadku to z
    glm::vec3 p[3];


    // rownanie plaszczyzny Ax + By + Cz + D = 0
    float A, B, C, D;

    CTriangle() {}

    CTriangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3) {
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;

        calculateEquation();
    }

    // obliczanie wspolczynnikow rownania
    void calculateEquation(void) {
        glm::vec3 v1, v2;

        v1 = p[1] - p[0];
        v2 = p[2] - p[0];

        A = v1.y * v2.z - v1.z * v2.y;     // A
        B = v1.z * v2.x - v1.x * v2.z;   // B
        C = v1.x * v2.y - v1.y * v2.x;   // C


        D = -(A * p[0].x + B * p[0].y + C * p[0].z);    // D

        if (C == 0) {
            //	printf("Uwaga! Trojkat pionowy.\n");
        }

    }

    // czy punkt p1 jest po lewej stronie odcinka p2-p3
    inline float sign(glm::vec2 p1, glm::vec3 p2, glm::vec3 p3) {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    }

    // czy punkt jest wewnatrz trojkata rzutowanego na plaszczyzne xy
    inline bool isInside(glm::vec2 point) {
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
    float altitude(glm::vec2 point) {
        if (C) return -(A * point.x + B * point.y + D) / C;
        else { return 99999; }
    }

};

// ---------------------------------------
class CGround {
public:

    int Number_of_triangles = 0;
    std::vector<CTriangle> triangles;


    // stworzenie struktury z listy trojkatow
    void CreateFromOBJ(std::vector<glm::vec3> vert) {
        glm::vec3 p1, p2, p3;

        std::vector<glm::vec3>::iterator it = vert.begin();
        while (it != vert.end()) {
            p1 = *it++;
            p2 = *it++;
            p3 = *it++;
            // uwaga ! zamiana wspolrzednych, tak aby z byl wysokoscia
            glm::vec3 np1 = glm::vec3(p1.x, p1.z, p1.y);
            glm::vec3 np2 = glm::vec3(p2.x, p2.z, p2.y);
            glm::vec3 np3 = glm::vec3(p3.x, p3.z, p3.y);
            triangles.push_back(CTriangle(np1, np2, np3));

        }

        Number_of_triangles = triangles.size();
        printf("Created CGround of %d triangles.\n", Number_of_triangles);
    }

//	float getAltitudeFromClosest(glm::vec2 point){
//     //   return glm::v
//	}



// Returns element closest to target in arr[]
    int findClosest(int arr[], int n, int target) {
        // Corner cases
        if (target <= arr[0])
            return arr[0];
        if (target >= arr[n - 1])
            return arr[n - 1];

        // Doing binary search
        int i = 0, j = n, mid = 0;
        while (i < j) {
            mid = (i + j) / 2;

            if (arr[mid] == target)
                return arr[mid];

            /* If target is less than array element,
                then search in left */
            if (target < arr[mid]) {

                // If target is greater than previous
                // to mid, return closest of two
                if (mid > 0 && target > arr[mid - 1])
                    return getClosest(arr[mid - 1],
                                      arr[mid], target);

                /* Repeat for left half */
                j = mid;
            }

                // If target is greater than mid
            else {
                if (mid < n - 1 && target < arr[mid + 1])
                    return getClosest(arr[mid],
                                      arr[mid + 1], target);
                // update i
                i = mid + 1;
            }
        }

        // Only single element left after search
        return arr[mid];
    }

// Method to compare which one is the more close.
// We find the closest by taking the difference
// between the target and both values. It assumes
// that val2 is greater than val1 and target lies
// between these two.
    int getClosest(int val1, int val2,
                   int target) {
        if (target - val1 >= val2 - target)
            return val2;
        else
            return val1;
    }


    // obliczenie wysokosci
    float getAltitute(glm::vec2 point) {

        // obliczenie listy trojkatow nad ktorymi jestesmy
        CTriangle *collTriangle = NULL;


        std::vector<CTriangle>::iterator it;
        for (it = triangles.begin(); it != triangles.end(); it++) {
            if ((*it).isInside(point)) {
                collTriangle = &(*it);
                break;
            }
        }

        // jesli zostal znaleziony
        if (collTriangle) {
            return collTriangle->altitude(point);
        } else {
            printf("Brak ziemi pod nogami!\n");
            return 0.0;
        }
    }

};

// -----------------------------------------
class CBone {

public:
    glm::vec3 Position;     // pozycja obiektu na scenie
    glm::vec3 Rotation;     // pozycja obiektu na scenie
    float radius = 1.0f;
    // potok opengl
    GLuint VAO;
    int VBO_Size;

    // macierz
    glm::mat4x4 matModel;

    // nast�pny obiekt typu CBone w strukturze
    CBone *next = NULL;

    // ustawienie potoku
    void Set(GLuint _vao, int _size) {
        matModel = glm::mat4(1.0);
        VAO = _vao;
        VBO_Size = _size;
    }

    // rysowanie na scenie
    void Draw(GLuint _prog, glm::mat4 _stackModel = glm::mat4(1.0)) {

        glm::mat4 localModel = _stackModel * matModel;

        glUniformMatrix4fv(glGetUniformLocation(_prog, "matModel"), 1, GL_FALSE, glm::value_ptr(localModel));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, VBO_Size);
        glBindVertexArray(0);

        if (next) {
            next->Draw(_prog, localModel);
        }

    }

    void Obroc(float angle, float x, float y, float z) {
        matModel = glm::rotate(matModel, angle, glm::vec3(x, y, z));
    }

    void Przesun(float x, float y, float z) {
        Position += glm::vec3(x, y, z);
        this->matModel = glm::translate(matModel, glm::vec3(x, y, z));
    }

    void SetRotation(float w, float x, float y, float z) {
        matModel = glm::rotate(matModel, w, glm::vec3(x, y, z));
    }

    bool isCollision(const CSceneObject &other) {
        float dist = glm::distance(this->Position, other.Position);
        if (dist < this->radius + other.radius) {

            return true;
        }
        return false;
    }

    // ustawienie pozycji na scenie
    void SetPosition(float x, float y, float z,CGround cg) {
        Position = glm::vec3(x, cg.getAltitute(glm::vec2(x,z))+1.8, z);
        matModel = glm::translate(glm::mat4(1.0), Position);

    }

//    void SetPosition(float x, float y, float z)
//    {
//
//        this->matModel = glm::translate(matModel, glm::vec3(-xpos, -new_y - 2, -zpos));
//    }


};


CBone Torso[ZOMBIES];
CBone RightArm[ZOMBIES];
CBone RightLeg[ZOMBIES];
CBone LeftArm[ZOMBIES];
CBone LeftLeg[ZOMBIES];
CBone Windmill;
CBone PropellerMain;
CBone PropellerSecondary;

void camera(void) {
//    glRotatef(xrot,1.0,0.0,0.0);  //rotate our camera on teh x-axis (left and right)
//    glRotatef(yrot,0.0,1.0,0.0);  //rotate our camera on the y-axis (up and down)
    glTranslated(-xpos, -ypos, -zpos); //translate the screen to the position of our camera
}

CGround myGround;

float treesX[500];
float treesY[500];

bool randomBool() {
    return 0 + (rand() % (1 - 0 + 1)) == 1;
}

void randTrees() {
    for (int i = 0; i < 100; ++i) {
        treesX[i] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2) - 1) * 10;
        treesY[i] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2) - 1) * 10;
    }
}

CSceneObject Trees[500];
CSceneObject Arms;
CSceneObject KEY1;
CSceneObject KEY2;
CSceneObject KEY3;
CSceneObject KEYEND;
CSceneObject Gate;
CSceneObject myCharacterCollider;
bool isCollision = false;

int CalculateFrameRate() {
    static float framesPerSecond = 0.0f;
    static int fps;
    static float lastTime = 0.0f;
    float currentTime = GetTickCount() * 0.001f;
    ++framesPerSecond;

    if (currentTime - lastTime > 1.0f) {
        lastTime = currentTime;
        fps = (int) framesPerSecond;
        framesPerSecond = 0;
    }
    return fps;

}

int frame = 0;

// ------------------------------------------------------------
// renderowanie sceny - same obiekty bez ustalonego framebuffera
void RenderScene(GLuint _prog = 0) {


    // Scena
    // ziemia
    glBindVertexArray(vArray[SCENE]);
    glBindTexture(GL_TEXTURE_2D, TextureID[SCENE]);
    glDrawArrays(GL_TRIANGLES, 0, OBJ_vertices[SCENE].size());


    glBindTexture(GL_TEXTURE_2D, TextureID[TREE]);
    glBindVertexArray(vArray[TREE]);
    glBindTexture(GL_TEXTURE_2D, TextureID[TREE]);
    //glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[TREE].size() );

    glBindTexture(GL_TEXTURE_2D, TextureID[TORSO]);
    glBindVertexArray(vArray[TORSO]);
    glBindTexture(GL_TEXTURE_2D, TextureID[TORSO]);

    glBindTexture(GL_TEXTURE_2D, TextureID[ARMS]);
    glBindVertexArray(vArray[ARMS]);
    glBindTexture(GL_TEXTURE_2D, TextureID[ARMS]);
    //glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[TREE].size() );

    for (int i = 0; i < 100; ++i) {
        Trees[i].Draw(_prog);
    }


    Arms.Draw();
    KEY1.Draw();
    KEY2.Draw();
    KEY3.Draw();
    Gate.Draw();
    KEYEND.Draw();
    Windmill.Draw(program[SCENE]);

}

// ------------------------------------------------------------
void DrawShadowMap() {
    // AKTYWUJEMY program
    glUseProgram(DepthMap_Program);

    glUniformMatrix4fv(glGetUniformLocation(DepthMap_Program, "lightProj"), 1, GL_FALSE, glm::value_ptr(lightProj));
    glUniformMatrix4fv(glGetUniformLocation(DepthMap_Program, "lightView"), 1, GL_FALSE, glm::value_ptr(lightView));
    glm::mat4 matModel = glm::mat4(1.0);
    glUniformMatrix4fv(glGetUniformLocation(DepthMap_Program, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));


//    glBindVertexArray(vArray[SCENE]);
//    glDrawArrays(GL_TRIANGLES, 0, OBJ_vertices[SCENE].size());
//    glBindVertexArray(0);

    RenderScene(DepthMap_Program);


    // WYLACZAMY program
    glUseProgram(0);

}

// ---------------------------------------
void DisplayScene() {

    // 1. Renderowanie z pozycji swiatla do textury DepthMap
    glViewport(0, 0, DepthMap_Width, DepthMap_Height);
    glBindFramebuffer(GL_FRAMEBUFFER, DepthMap_FrameBuffer);
    glClear(GL_DEPTH_BUFFER_BIT);

    DrawShadowMap();


    glViewport(0, 0, Window_width, Window_height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Macierz widoku
    matView = glm::mat4x4(1.0);
    matView = glm::rotate(matView, _scene_rotate_x, glm::vec3(1.0f, 0.0f, 0.0f));
    matView = glm::rotate(matView, _scene_rotate_y, glm::vec3(0.0f, 1.0f, 0.0f));

    matView = glm::translate(matView, glm::vec3(_scene_translate_x, 0, _scene_translate_z));

    if (isCollision) {
        xpos -= 0.01f;
        ypos -= 0.01f;
//        if (xpos >= 0){
//            xpos -= 0.01f;
//        }else{
//            xpos += 0.01f;
//        }
//        if (ypos >= 0){
//            ypos -= 0.01f;
//        }else{
//            ypos += 0.01f;
//        }

    }


    float y = myGround.getAltitute(glm::vec2(xpos, zpos));
    //printf("XD %f", y);
    matView = glm::translate(matView, glm::vec3(-xpos, -y - 2, -zpos));
    Arms.SetPosition(xpos, y + 2.0f, zpos + 5);
    Arms.SetRotation(-_scene_rotate_x, 1.0f, 0.0f, 0.0f);
    Arms.SetRotation(-_scene_rotate_y, 0.0f, 1.0f, 0.0f);
    myCharacterCollider.SetPosition(xpos, y + 2.0f, zpos + 5);


    Camera_Position = ExtractCameraPos(matView);
    glUniform3fv(glGetUniformLocation(program[PROGRAM_SCENE], "PlayerPos"), 1, glm::value_ptr(Camera_Position));
    // Macierz modelu
    matModel = glm::mat4x4(1.0);


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glUseProgram(program[PROGRAM_SCENE]);

    //TODO LIGHT AND SHADOWS

    glUniformMatrix4fv(glGetUniformLocation(program[PROGRAM_SCENE], "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
    glUniformMatrix4fv(glGetUniformLocation(program[PROGRAM_SCENE], "matView"), 1, GL_FALSE, glm::value_ptr(matView));
    glUniformMatrix4fv(glGetUniformLocation(program[PROGRAM_SCENE], "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));


    glUniformMatrix4fv(glGetUniformLocation(program[PROGRAM_SCENE], "lightProj"), 1, GL_FALSE,
                       glm::value_ptr(lightProj));
    glUniformMatrix4fv(glGetUniformLocation(program[PROGRAM_SCENE], "lightView"), 1, GL_FALSE,
                       glm::value_ptr(lightView));


    // Swiatlo kierunkowe
    glUniform3fv(glGetUniformLocation(program[PROGRAM_SCENE], "Light_Direction"), 1, glm::value_ptr(Light_Direction));
    glUniform3fv(glGetUniformLocation(program[PROGRAM_SCENE], "Camera_Position"), 1, glm::value_ptr(Camera_Position));


    // Shadow map textura na slot 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, DepthMap_Texture);
    glUniform1i(glGetUniformLocation(program[PROGRAM_SCENE], "shadowMap"), 1);



    // AKTYWUJEMY tekstury
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(program[PROGRAM_SCENE], "tex0"), 0);


    // Rendering SCENY
    RenderScene();


    Gate.radius = 5;
    for (int i = 0; i < 100; ++i) {
        if (Gate.isCollision(myCharacterCollider)) {
            isCollision = true;
            break;
        } else {
            isCollision = false;
        }


        if (Trees[i].isCollision(myCharacterCollider)) {
            isCollision = true;


            break;
        } else {
            isCollision = false;
        }
    }


    if (KEY1.isCollision(myCharacterCollider)) {
        KEYS--;
        KEY1.SetPosition(0.0, -100.0, 0.0);

    }
    if (KEY2.isCollision(myCharacterCollider)) {
        KEYS--;
        KEY2.SetPosition(0.0, -100.0, 0.0);

    }
    if (KEY3.isCollision(myCharacterCollider)) {
        KEYS--;
        KEY3.SetPosition(0.0, -100.0, 0.0);

    }
    if (KEYEND.isCollision(myCharacterCollider)) {
        KEYS--;
        KEYEND.SetPosition(0.0, -100.0, 0.0);
    }
    if (KEYS == 0) {
        Gate.SetPosition(0.0, -100.0, 0.0);
    }

    for (int j = 0; j < ZOMBIES; ++j) {
        Torso[j].Draw(program[SCENE]);
    }

    glUseProgram(0);
    glBindVertexArray(0);

    char txt[255];
    if (KEYS == -1) {
        sprintf(txt, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$ YOU ARE RICH $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    } else if (KEYS == 0) {
        sprintf(txt, "GO TO THE WINDMILL");
    } else {
        sprintf(txt, "FIND: %d more KEYS TO ENTER TO WINDMILL", KEYS);
    }

    frame++;
    DrawText8x16(3, 21, txt, glm::vec4(1.0, 1.0, 1.0, 1.0));


    char txtfps[255];
    sprintf(txtfps, "FPS: %d", CalculateFrameRate());
    frame++;
    DrawText8x16(3, 3, txtfps, glm::vec4(1.0, 1.0, 1.0, 1.0));

    DrawText8x16(300, 3, "W - TO GO FORWARD, MOUSE - TO LOOK", glm::vec4(1.0, 1.0, 1.0, 1.0));


    glutSwapBuffers();
}

// ---------------------------------------
void Reshape(int width, int height) {
    glViewport(0, 0, width, height);
    Window_width = width;
    Window_height = height;
    matProj = glm::perspectiveFov(glm::radians(60.0f), (float) width, (float) height, 0.1f, 100.f);
}

float leftLeg1[ZOMBIES];
float leftLeg2[ZOMBIES];
float rightLeg1[ZOMBIES];
float rightLeg2[ZOMBIES];
float zombiePosX1[ZOMBIES];
float zombiePosX2[ZOMBIES];
float zombiePosZ1[ZOMBIES];
float zombiePosZ2[ZOMBIES];
float rot[ZOMBIES];

void UpdateZombieAnimation(float step) {

    for (int i = 0; i < ZOMBIES; i++) {

        // Leg Animation
        if (leftLeg1[i] > -0.5) {
            LeftLeg[i].Obroc(-step, 1.0, 0.0, 0.0);
            RightLeg[i].Obroc(step * 2, 1.0, 0.0, 0.0);
            leftLeg1[i] -= step;
            continue;
        }
        if (rightLeg1[i] > -0.5) {
            LeftLeg[i].Obroc(step, 1.0, 0.0, 0.0);
            RightLeg[i].Obroc(-step * 2, 1.0, 0.0, 0.0);
            rightLeg1[i] -= step;
            continue;
        }
        if (leftLeg2[i] <= 0.5) {
            LeftLeg[i].Obroc(step, 1.0, 0.0, 0.0);
            RightLeg[i].Obroc(-step * 2, 1.0, 0.0, 0.0);
            leftLeg2[i] += step;
            continue;
        }
        if (rightLeg2[i] <= 0.5) {
            LeftLeg[i].Obroc(-step, 1.0, 0.0, 0.0);
            RightLeg[i].Obroc(step * 2, 1.0, 0.0, 0.0);
            rightLeg2[i] += step;
            continue;
        }
        leftLeg1[i] = 0;
        leftLeg2[i] = 0;
        rightLeg1[i] = 0;
        rightLeg2[i] = 0;
    }


}

void UpdateZombieMovement(float step) {

    for (int i = 0; i < ZOMBIES; ++i) {

        if (zombiePosX1[i] < 10) {
            zombiePosX1[i] += step;
            if (rot[i] < 1.5) {
                Torso[i].Obroc(step, 0.0, 1.0, 0.0);
                rot[i] += step;
            }
            // Torso.matModel = glm::translate(glm::mat4(1.0), glm::vec3(zombiePosX1, 5, zombiePosZ1));
            Torso[i].Przesun(0, 0, step);
            continue;
        }
        if (zombiePosZ1[i] < 10) {
            zombiePosZ1[i] += step;
            if (rot[i] < 3) {
                Torso[i].Obroc(step, 0.0, 1.0, 0.0);
                rot[i] += step;
            }
            // Torso.matModel = glm::translate(glm::mat4(1.0), glm::vec3(zombiePosX1, 5, zombiePosZ1));
            Torso[i].Przesun(0, 0, step);
            continue;
        }

        if (zombiePosX2[i] < 10) {
            zombiePosX2[i] += step;
            if (rot[i] < 4.5) {
                Torso[i].Obroc(step, 0.0, 1.0, 0.0);
                rot[i] += step;
            }
            // Torso.matModel = glm::translate(glm::mat4(1.0), glm::vec3(zombiePosX1, 5, zombiePosZ1));
            Torso[i].Przesun(0, 0, step);
            continue;
        }

        if (zombiePosZ2[i] < 10) {
            zombiePosZ2[i] += step;
            if (rot[i] < 6) {
                Torso[i].Obroc(step, 0.0, 1.0, 0.0);
                rot[i] += step;
            }
            // Torso.matModel = glm::translate(glm::mat4(1.0), glm::vec3(zombiePosX1, 5, zombiePosZ1));
            Torso[i].Przesun(0, 0, step);
            continue;
        }
        zombiePosX1[i] = 0;
        zombiePosX2[i] = 0;
        zombiePosZ1[i] = 0;
        zombiePosZ2[i] = 0;
        rot[i] = 0;
    }

    // Movement
}

void UpdateWindMill() {

    PropellerMain.Obroc(0.001, 0.0, 0.0, 1.0);

    PropellerSecondary.Obroc(-0.005, 0.0, 0.0, 1.0);
}

int main(int argc, char *argv[]);

void Reset();

// --------------------------------------------------------------
void Keyboard(unsigned char key, int x, int y) {


    switch (key) {
        case 27:    // ESC key
            exit(0);
            break;

        case 'w':

            float xrotrad, yrotrad;
            // yrotrad = (_scene_rotate_x / 180 * 3.141592654f);
            yrotrad = (_scene_rotate_y);
            //xrotrad = (_scene_rotate_y / 180 * 3.141592654f);
            xrotrad = (_scene_rotate_x);
            xpos += float(sin(yrotrad)) / 8.0f;
            zpos -= float(cos(yrotrad)) / 8.0f;
            ypos -= float(sin(xrotrad)) / 8.0f;
            // printf("XD %f",_scene_rotate_x);
            break;


        case 'x':
            int rip = main(GLOBAL_argc, GLOBAL_argv);
            break;


    }

    glutPostRedisplay();
}

// ---------------------------------------------------
void Initialize() {
    _scene_translate_z = -5;
    _scene_translate_y = -2;

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // ---------------------------------------
    // Ladowanie pliku OBJ
    if (!loadOBJ("scene-plane1.obj", OBJ_vertices[SCENE], OBJ_uvs[SCENE], OBJ_normals[SCENE])) {
        printf("Not loaded!\n");
        exit(1);
    }


    // ---------------------------------------
    // Tworzenie tekstury
    int tex_width;
    int tex_height;
    unsigned char *tex_data;
    loadBMP_custom("grass.bmp", tex_width, tex_height, &tex_data);

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
    glAttachShader(program[PROGRAM_SCENE], LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
    glAttachShader(program[PROGRAM_SCENE], LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
    LinkAndValidateProgram(program[PROGRAM_SCENE]);

    glGenVertexArrays(1, &vArray[SCENE]);
    glBindVertexArray(vArray[SCENE]);

    glGenBuffers(1, &vBuffer_pos[SCENE]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos[SCENE]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_vertices[SCENE].size() * sizeof(glm::vec3), &(OBJ_vertices[SCENE])[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vBuffer_normal[SCENE]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_normal[SCENE]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_normals[SCENE].size() * sizeof(glm::vec3), &(OBJ_normals[SCENE])[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // Wspolrzedne textury UV
    glGenBuffers(1, &vBuffer_uv[SCENE]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv[SCENE]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_uvs[SCENE].size() * sizeof(glm::vec2), &(OBJ_uvs[SCENE])[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glEnable(GL_DEPTH_TEST);


    // SPHERE
    if (!loadOBJ("key.obj", OBJ_vertices[KEY], OBJ_uvs[KEY], OBJ_normals[KEY])) {
        printf("Not loaded!\n");
        exit(1);
    }


    // Vertex arrays
    glGenVertexArrays(1, &vArray[KEY]);
    glBindVertexArray(vArray[KEY]);

    glGenBuffers(1, &vBuffer_pos[KEY]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos[KEY]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_vertices[KEY].size() * sizeof(glm::vec3), &(OBJ_vertices[KEY])[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vBuffer_uv[KEY]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv[KEY]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_uvs[KEY].size() * sizeof(glm::vec2), &(OBJ_uvs[KEY])[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // SPHERE
    if (!loadOBJ("gate.obj", OBJ_vertices[GATE], OBJ_uvs[GATE], OBJ_normals[GATE])) {
        printf("Not loaded!\n");
        exit(1);
    }


    // Vertex arrays
    glGenVertexArrays(1, &vArray[GATE]);
    glBindVertexArray(vArray[GATE]);

    glGenBuffers(1, &vBuffer_pos[GATE]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos[GATE]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_vertices[GATE].size() * sizeof(glm::vec3), &(OBJ_vertices[GATE])[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vBuffer_uv[GATE]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv[GATE]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_uvs[GATE].size() * sizeof(glm::vec2), &(OBJ_uvs[GATE])[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    //loadBMP_custom("grass.bmp", tex_width, tex_height, &tex_data);

    glGenTextures(1, &TextureID[TREE]);
    glBindTexture(GL_TEXTURE_2D, TextureID[TREE]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


    // SPHERE
    if (!loadOBJ("laubbaum.obj", OBJ_vertices[TREE], OBJ_uvs[TREE], OBJ_normals[TREE])) {
        printf("Not loaded!\n");
        exit(1);
    }


    // Vertex arrays
    glGenVertexArrays(1, &vArray[TREE]);
    glBindVertexArray(vArray[TREE]);

    glGenBuffers(1, &vBuffer_pos[TREE]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos[TREE]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_vertices[TREE].size() * sizeof(glm::vec3), &(OBJ_vertices[TREE])[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vBuffer_uv[TREE]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv[TREE]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_uvs[TREE].size() * sizeof(glm::vec2), &(OBJ_uvs[TREE])[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    //loadBMP_custom("grass.bmp", tex_width, tex_height, &tex_data);

    glGenTextures(1, &TextureID[TREE]);
    glBindTexture(GL_TEXTURE_2D, TextureID[TREE]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    InitDrawText();
    /// arms
    // SPHERE
    if (!loadOBJ("arms.obj", OBJ_vertices[ARMS], OBJ_uvs[ARMS], OBJ_normals[ARMS])) {
        printf("Not loaded!\n");
        exit(1);
    }


    // Vertex arrays
    glGenVertexArrays(1, &vArray[ARMS]);
    glBindVertexArray(vArray[ARMS]);

    glGenBuffers(1, &vBuffer_pos[ARMS]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos[ARMS]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_vertices[ARMS].size() * sizeof(glm::vec3), &(OBJ_vertices[ARMS])[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vBuffer_uv[ARMS]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv[ARMS]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_uvs[ARMS].size() * sizeof(glm::vec2), &(OBJ_uvs[ARMS])[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    loadBMP_custom("wood.bmp", tex_width, tex_height, &tex_data);

    glGenTextures(1, &TextureID[ARMS]);
    glBindTexture(GL_TEXTURE_2D, TextureID[ARMS]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    tex_data = SOIL_load_image("tex1-min.png", &tex_width, &tex_height, 0, SOIL_LOAD_RGBA);
    if (tex_data == NULL) {
        printf("Blad odczytu pliku graficznego!\n");
        exit(1);
    }


    // NOWE: parametr RGBA zamiast RGB dwukrotnie !
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
    SOIL_free_image_data(tex_data);

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


//    Arms.SetPosition(0.0f,0.0f,0.0f);
    // Ziemia
    myGround.CreateFromOBJ(OBJ_vertices[SCENE]);


    Arms.Set(program[SCENE], vArray[ARMS], OBJ_vertices[ARMS].size());
    //Arms.SetPosition(1.0, 10.0, 1.0);
    KEY1.Set(program[SCENE], vArray[KEY], OBJ_vertices[KEY].size());
    KEY2.Set(program[SCENE], vArray[KEY], OBJ_vertices[KEY].size());
    KEY3.Set(program[SCENE], vArray[KEY], OBJ_vertices[KEY].size());
    KEYEND.Set(program[SCENE], vArray[KEY], OBJ_vertices[KEY].size());
    Gate.Set(program[SCENE], vArray[GATE], OBJ_vertices[GATE].size());
    Reset();
    // Inicjalizacja obiektow

    for (int i = 0; i < 500; ++i) {

        Trees[i].Set(program[SCENE], vArray[TREE], OBJ_vertices[TREE].size());


        Trees[i].SetPosition(myGround.triangles[i].p[0].x, myGround.triangles[i].p[0].z,
                             myGround.triangles[i].p[0].y);


    }


    if (!loadOBJ("torso.obj", OBJ_vertices[TORSO], OBJ_uvs[TORSO], OBJ_normals[TORSO])) {
        printf("Not loaded!\n");
        exit(1);
    }
    // Vertex arrays
    glGenVertexArrays(1, &vArray[TORSO]);
    glBindVertexArray(vArray[TORSO]);

    glGenBuffers(1, &vBuffer_pos[TORSO]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos[TORSO]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_vertices[TORSO].size() * sizeof(glm::vec3), &(OBJ_vertices[TORSO])[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vBuffer_uv[TORSO]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv[TORSO]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_uvs[TORSO].size() * sizeof(glm::vec2), &(OBJ_uvs[TORSO])[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);


    loadBMP_custom("zombie.bmp", tex_width, tex_height, &tex_data);

    glGenTextures(1, &TextureID[TORSO]);
    glBindTexture(GL_TEXTURE_2D, TextureID[TORSO]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


    if (!loadOBJ("leftleg.obj", OBJ_vertices[LEG_L], OBJ_uvs[LEG_L], OBJ_normals[LEG_L])) {
        printf("Not loaded!\n");
        exit(1);
    }
    // Vertex arrays
    glGenVertexArrays(1, &vArray[LEG_L]);
    glBindVertexArray(vArray[LEG_L]);

    glGenBuffers(1, &vBuffer_pos[LEG_L]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos[LEG_L]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_vertices[LEG_L].size() * sizeof(glm::vec3), &(OBJ_vertices[LEG_L])[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vBuffer_uv[LEG_L]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv[LEG_L]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_uvs[LEG_L].size() * sizeof(glm::vec2), &(OBJ_uvs[LEG_L])[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);


    if (!loadOBJ("rightleg.obj", OBJ_vertices[LEG_R], OBJ_uvs[LEG_R], OBJ_normals[LEG_R])) {
        printf("Not loaded!\n");
        exit(1);
    }
    // Vertex arrays
    glGenVertexArrays(1, &vArray[LEG_R]);
    glBindVertexArray(vArray[LEG_R]);

    glGenBuffers(1, &vBuffer_pos[LEG_R]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos[LEG_R]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_vertices[LEG_R].size() * sizeof(glm::vec3), &(OBJ_vertices[LEG_R])[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vBuffer_uv[LEG_R]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv[LEG_R]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_uvs[LEG_R].size() * sizeof(glm::vec2), &(OBJ_uvs[LEG_R])[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);


    // 1. Stworzenie kosci
    for (int j = 0; j < ZOMBIES; ++j) {
        Torso[j].Set(vArray[TORSO], OBJ_vertices[TORSO].size());
        LeftLeg[j].Set(vArray[LEG_L], OBJ_vertices[LEG_L].size());
        RightLeg[j].Set(vArray[LEG_R], OBJ_vertices[LEG_R].size());
        Torso[j].next = &RightArm[j];
        RightArm[j].next = &LeftArm[j];

        LeftArm[j].next = &LeftLeg[j];
        LeftLeg[j].next = &RightLeg[j];

        Torso[j].SetPosition(treesX[j]*2-5, 5, treesY[j]*2-20,myGround);
        RightArm[j].matModel = glm::translate(glm::mat4(1.0), glm::vec3(-0.3, -0.28, -0.1));
        LeftArm[j].matModel = glm::translate(glm::mat4(1.0), glm::vec3(0.6, 0.0, 0.0));
        LeftLeg[j].matModel = glm::translate(glm::mat4(1.0), glm::vec3(-0.3, -0.5, 0.2));
        RightLeg[j].matModel = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));
    }



    // 2. Ustalenie hierarchii

    //TODO change this hierarchy somehow




    //RightArm.Obroc(-0.5, 0.0, 1.0, 0.0);
    //PropellerSecondary.Obroc(0.6, 0.0, 0.0, 1.0);



    if (!loadOBJ("windmillbase.obj", OBJ_vertices[WINDMILL], OBJ_uvs[WINDMILL], OBJ_normals[WINDMILL])) {
        printf("Not loaded!\n");
        exit(1);
    }
    // Vertex arrays
    glGenVertexArrays(1, &vArray[WINDMILL]);
    glBindVertexArray(vArray[WINDMILL]);

    glGenBuffers(1, &vBuffer_pos[WINDMILL]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos[WINDMILL]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_vertices[WINDMILL].size() * sizeof(glm::vec3), &(OBJ_vertices[WINDMILL])[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vBuffer_uv[WINDMILL]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv[WINDMILL]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_uvs[WINDMILL].size() * sizeof(glm::vec2), &(OBJ_uvs[WINDMILL])[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);


    if (!loadOBJ("propeller.obj", OBJ_vertices[PROPELLER_MAIN], OBJ_uvs[PROPELLER_MAIN], OBJ_normals[PROPELLER_MAIN])) {
        printf("Not loaded!\n");
        exit(1);
    }
    // Vertex arrays
    glGenVertexArrays(1, &vArray[PROPELLER_MAIN]);
    glBindVertexArray(vArray[PROPELLER_MAIN]);

    glGenBuffers(1, &vBuffer_pos[PROPELLER_MAIN]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos[PROPELLER_MAIN]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_vertices[PROPELLER_MAIN].size() * sizeof(glm::vec3),
                 &(OBJ_vertices[PROPELLER_MAIN])[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vBuffer_uv[PROPELLER_MAIN]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv[PROPELLER_MAIN]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_uvs[PROPELLER_MAIN].size() * sizeof(glm::vec2), &(OBJ_uvs[PROPELLER_MAIN])[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);


    if (!loadOBJ("propeller.obj", OBJ_vertices[PROPELLER_SECONDARY], OBJ_uvs[PROPELLER_SECONDARY],
                 OBJ_normals[PROPELLER_SECONDARY])) {
        printf("Not loaded!\n");
        exit(1);
    }
    // Vertex arrays
    glGenVertexArrays(1, &vArray[PROPELLER_SECONDARY]);
    glBindVertexArray(vArray[PROPELLER_SECONDARY]);

    glGenBuffers(1, &vBuffer_pos[PROPELLER_SECONDARY]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos[PROPELLER_SECONDARY]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_vertices[PROPELLER_SECONDARY].size() * sizeof(glm::vec3),
                 &(OBJ_vertices[PROPELLER_SECONDARY])[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vBuffer_uv[PROPELLER_SECONDARY]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv[PROPELLER_SECONDARY]);
    glBufferData(GL_ARRAY_BUFFER, OBJ_uvs[PROPELLER_SECONDARY].size() * sizeof(glm::vec2),
                 &(OBJ_uvs[PROPELLER_SECONDARY])[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);


    // 1. Stworzenie kosci
    Windmill.Set(vArray[WINDMILL], OBJ_vertices[WINDMILL].size());
    PropellerMain.Set(vArray[PROPELLER_MAIN], OBJ_vertices[PROPELLER_MAIN].size());
    PropellerSecondary.Set(vArray[PROPELLER_SECONDARY], OBJ_vertices[PROPELLER_SECONDARY].size());

    // 2. Ustalenie hierarchii
    Windmill.next = &PropellerMain;
    PropellerMain.next = &PropellerSecondary;


    Windmill.matModel = glm::translate(glm::mat4(1.0), glm::vec3(0, 7.4, -35));
    // 3. Ustawlenie rozmieszczenia obiektow wzgledem bazowego
    PropellerMain.matModel = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 15.0, 1.5 * 3.0));
    PropellerSecondary.matModel = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.5 * 3.0));
    PropellerSecondary.Obroc(0.6, 0.0, 0.0, 1.0);

    // ---------- SHADOW MAPPING------------

    //Shadow mapping
    printf("1");
    // 1. Texture
    glGenTextures(1, &DepthMap_Texture);
    glBindTexture(GL_TEXTURE_2D, DepthMap_Texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DepthMap_Width, DepthMap_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    //float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float borderColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // 2. Frame buffer object
    glGenFramebuffers(1, &DepthMap_FrameBuffer);

    // 3. Attaching texture to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, DepthMap_FrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthMap_Texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 4. OpenGL program for shadow map
    // Tworzenie potoku OpenGL
    DepthMap_Program = glCreateProgram();
    glAttachShader(DepthMap_Program, LoadShader(GL_VERTEX_SHADER, "depthmap.vertex.glsl"));
    glAttachShader(DepthMap_Program, LoadShader(GL_FRAGMENT_SHADER, "depthmap.fragment.glsl"));
    LinkAndValidateProgram(DepthMap_Program);

    // Inne ustawienia openGL i sceny
    glEnable(GL_DEPTH_TEST);
    // NOWE
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(vArray[SCENE]);
    glUseProgram(program[SCENE]);


}

void Reset() {
    KEY1.SetPosition(30, 5, 30);
    KEY2.SetPosition(-30.0, 2.5, -30);
    KEY3.SetPosition(30, 2.5, -30);
    KEYEND.SetPosition(0, 2.5, -35);
    Gate.SetPosition(0, 2.5, -30);
    KEYS = 3;
}

//s
void Animation(int frame) {
    glutTimerFunc(5, Animation, 0);
    UpdateWindMill();
    UpdateZombieMovement(0.005);
    UpdateZombieAnimation(0.005);
    glutPostRedisplay();
}

// ---------------------------------------------------
int main(int argc, char *argv[]) {
    randTrees();
    GLOBAL_argc = argc;
    *GLOBAL_argv = *argv;
    // GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL");

    // GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        printf("GLEW Error\n");
        exit(1);
    }

    // OpenGL
    if (!GLEW_VERSION_3_2) {
        printf("Brak OpenGL 3.2!\n");
        exit(1);
    }


    Initialize();
    glutDisplayFunc(DisplayScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
    glutMouseWheelFunc(MouseWheel);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(SpecialKeys);
    glutTimerFunc(5, Animation, 0);
    glutMainLoop();


    return 0;
}
