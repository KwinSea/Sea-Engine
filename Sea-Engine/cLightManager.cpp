#include "cLightManager.h"

#include <iostream>


cLight::cLight() {
    this->position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    this->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    this->specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    this->atten = glm::vec4(0.0f, 0.01f, 0.01f, 1.0f);
    // For spot lights
    this->direction = glm::vec4(0.0f, -0.5f, 0.0f, 1.0f);
    this->param1.x = 0.0f; // Point light
    // If it's a spot light, these will be looked at
    this->param1.y = 1.0f; // Inner angle
    this->param1.z = 45.0f; // outer angle

    this->param2.x = 0.0f; // Off

}


void cLightManager::GetUniformLocations(GLuint shaderProgram) {
    std::ostringstream ss;
    for (unsigned int index = 0; index != NUMBEROFLIGHTS; index++) {
        ss << "theLights[" << index << "].position";
        this->theLights[index].position_UL = glGetUniformLocation(shaderProgram, ss.str().c_str());
        ss.str("");

        ss << "theLights[" << index << "].diffuse";
        this->theLights[index].diffuse_UL = glGetUniformLocation(shaderProgram, ss.str().c_str());
        ss.str("");

        ss << "theLights[" << index << "].specular";
        this->theLights[index].specular_UL = glGetUniformLocation(shaderProgram, ss.str().c_str());
        ss.str("");

        ss << "theLights[" << index << "].atten";
        this->theLights[index].atten_UL = glGetUniformLocation(shaderProgram, ss.str().c_str());
        ss.str("");

        ss << "theLights[" << index << "].direction";
        this->theLights[index].direction_UL = glGetUniformLocation(shaderProgram, ss.str().c_str());
        ss.str("");

        ss << "theLights[" << index << "].param1";
        this->theLights[index].param1_UL = glGetUniformLocation(shaderProgram, ss.str().c_str());
        ss.str("");

        ss << "theLights[" << index << "].param2";
        this->theLights[index].param2_UL = glGetUniformLocation(shaderProgram, ss.str().c_str());
        ss.str("");
    }
}

void cLightManager::UpdateShaderUniforms(GLuint shaderProgram) {
    for (unsigned int index = 0; index != cLightManager::NUMBEROFLIGHTS; index++) {
        glUniform4f(this->theLights[index].position_UL, // Uniform location
                    this->theLights[index].position.x,
                    this->theLights[index].position.y,
                    this->theLights[index].position.z,
                    this->theLights[index].position.w);

        glUniform4f(this->theLights[index].diffuse_UL, // diffuse(colour)
                    this->theLights[index].diffuse.r,
                    this->theLights[index].diffuse.g,
                    this->theLights[index].diffuse.b,
                    this->theLights[index].diffuse.a);

        glUniform4f(this->theLights[index].specular_UL, // Specular
                    this->theLights[index].specular.r,
                    this->theLights[index].specular.g,
                    this->theLights[index].specular.b,
                    this->theLights[index].specular.w);

        glUniform4f(this->theLights[index].atten_UL, // attenuation
                    this->theLights[index].atten.r,
                    this->theLights[index].atten.g,
                    this->theLights[index].atten.b,
                    this->theLights[index].atten.a);

        glUniform4f(this->theLights[index].direction_UL, // Direction
                    this->theLights[index].direction.r,
                    this->theLights[index].direction.g,
                    this->theLights[index].direction.b,
                    this->theLights[index].direction.a);

        glUniform4f(this->theLights[index].param1_UL, // Light type and unique attributes
                    this->theLights[index].param1.x,
                    this->theLights[index].param1.y,
                    this->theLights[index].param1.z,
                    this->theLights[index].param1.w);

        glUniform4f(this->theLights[index].param2_UL, // Light state
                    this->theLights[index].param2.x,
                    this->theLights[index].param2.y,
                    this->theLights[index].param2.z,
                    this->theLights[index].param2.w);
    } //for ( unsigned int index...
}
