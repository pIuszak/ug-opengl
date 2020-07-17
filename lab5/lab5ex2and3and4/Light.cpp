//
// Created by paull on 20-Apr-20.
//

#include "Light.h"

Light::Light() {
    colour = glm::vec3(1.0f, 1.0f, 1.0f);
    ambientIntensity = 1.0f;
}

Light::Light(GLfloat r, GLfloat g, GLfloat b, GLfloat aIntensity) {
    colour = glm::vec3(r, g, b);
    ambientIntensity = aIntensity;
}

void Light::UseLight(GLfloat ambientIntesityLocation, GLfloat ambientColourLocation) {
    glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
    glUniform1f(ambientIntesityLocation, ambientIntensity);
}