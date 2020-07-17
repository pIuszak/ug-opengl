//
// Created by paull on 20-Apr-20.
//

#include <glm/glm.hpp>
#include <GL/glew.h>
#ifndef LAB5EX2AND3_LIGHT_H
#define LAB5EX2AND3_LIGHT_H


class Light {
public:
    Light(GLfloat r, GLfloat g, GLfloat b, GLfloat aIntensity);
    Light();
    void UseLight(GLfloat ambientIntesityLocation, GLfloat ambientColourLocation);

private:
    glm::vec3 colour;
    GLfloat ambientIntensity;

};


#endif //LAB5EX2AND3_LIGHT_H
