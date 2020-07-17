// ---------------------------------------------------
// Instanced Rendering w OpenGL
// ---------------------------------------------------

// Zalozenia:
//
// Mamy jeden obiekt, ktory chcemy skopiowac wiele razy
// i kazdy z nich umiescic gdzies indziej na scenie
//
// Polozenie/orientacje kazdego z tych obiektow bedziemy przechowywac
// w macierzy modelu mat4x4.

const int Number_of_Birds = 1000;

// 1. Stworzenie tablicy tablicy z macierzami modelu dla obiektow

glm::mat4x4 Table_of_Birds_matModel[Number_of_Birds];


// 2. Inicjalizacja jakimis losowymi danymi

int RES = 1000;
int SIZE = 20;

for (int i=0; i<Number_of_Birds; ++i)
{
    float x = (rand()%RES)/(RES/SIZE) - SIZE/2.0;
    float z = (rand()%RES)/(RES/SIZE) - SIZE/2.0;
    float scale = (0.2 + (rand()%80)/100.0);
    float y = (rand()%RES)/(RES/SIZE) - SIZE/2.0;

    float angle = (rand()%314)/100.0;

    Table_of_Birds_matModel[i] = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z) );
    Table_of_Birds_matModel[i] = glm::rotate(Table_of_Birds_matModel[i], angle, glm::vec3(0.0f, 1.0f, 0.0f));
    Table_of_Birds_matModel[i] = glm::scale(Table_of_Birds_matModel[i], glm::vec3(scale, scale, scale));

}


// 3. Utworzenie bufora VBO dla tych macierzy, ktore bedziemy przekazywali
// podczas renderowania instancjonowanego


glGenBuffers(1, &vInstances);
glBindBuffer(GL_ARRAY_BUFFER, vInstances);
glBufferData(GL_ARRAY_BUFFER, Number_of_Birds * sizeof(glm::mat4), &Table_of_Birds_matModel[0], GL_STATIC_DRAW);


glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
glEnableVertexAttribArray(3);
glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
glEnableVertexAttribArray(4);
glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
glEnableVertexAttribArray(5);
glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
glEnableVertexAttribArray(6);

glVertexAttribDivisor(3, 1);
glVertexAttribDivisor(4, 1);
glVertexAttribDivisor(5, 1);
glVertexAttribDivisor(6, 1);


// 4. Shader wierzcholkow:

#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec2 inUV;
layout( location = 2 ) in vec3 inNormal;
layout( location = 3 ) in mat4 matModelInst;

uniform mat4 matProj;
uniform mat4 matView;

out vec4 inoutPos;
out vec2 inoutUV;


void main()
{
	gl_Position = matProj * matView * matModelInst * inPosition;

	inoutPos = inPosition;
	inoutUV = inUV;
}


// 5. Generowanie obiektow na ekranie za pomoca instancji
glDrawArraysInstanced( GL_TRIANGLES, 0, OBJ_vertices.size(), Number_of_Birds);
