#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include "GlobalOpenGL.h"

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include "cShaderManager/cShaderManager.h"
#include "cVAOManager/cVAOManager.h"
#include "cMeshObject.h"


cShaderManager* g_pTheShaderManager = NULL;
cVAOManager* g_pMeshManager = NULL;

std::vector<cMeshObject*> g_pMeshesToDraw;

unsigned int g_NumVerticiesToDraw = 0;
unsigned int g_SizeOfVertexArrayInBytes = 0;
glm::vec3 g_cameraEye = glm::vec3(0.0, 0.0, -30.0f);



static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main(void)
{
    GLFWwindow* window;


    GLuint program;
    GLint mvp_location;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    

    ::g_pTheShaderManager = new cShaderManager();

    ::g_pTheShaderManager->setBasePath("assets/shaders/");

    cShaderManager::cShader vert_shader;
    vert_shader.fileName = "vertex_shader.glsl";

    cShaderManager::cShader frag_shader;
    frag_shader.fileName = "fragment_shader.glsl";


    if (::g_pTheShaderManager->createProgramFromFile("shader1", vert_shader, frag_shader))
    {
        std::cout << "Shaders succesfully created!" << std::endl;
    }
    else
    {
        std::cout << ::g_pTheShaderManager->getLastError() << std::endl;
    }

    program = ::g_pTheShaderManager->getIDFromFriendlyName("shader1");




    mvp_location = glGetUniformLocation(program, "MVP");

   
    ::g_pMeshManager = new cVAOManager();
    sModelDrawInfo meshInfoCow;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/cow.ply",
        meshInfoCow, program, false, true))
    {
        std::cout << "Cow not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshTeapot;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Utah_Teapot.ply",
        meshTeapot, program, false, true))
    {
        std::cout << "Teapot NOT loaded into VAO!" << std::endl;
    }

    sModelDrawInfo dolphinMeshInfo;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/dolphin.ply",
        dolphinMeshInfo, program, false, true))
    {
        std::cout << "Teapot NOT loaded into VAO!" << std::endl;
    }

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Warehouse_xyz_rgba.ply",
        dolphinMeshInfo, program, false, true)) {
        std::cout << "Warehouse NOT loaded into VAO!" << std::endl;
    }

    cMeshObject* pWarehouse = new cMeshObject();
    pWarehouse->meshFileName = "assets/models/Warehouse_xyz_rgba.ply";
    pWarehouse->position.y = -20.0f;
    pWarehouse->orientation.y = 90.0f;
    ::g_pMeshesToDraw.push_back(pWarehouse);

    cMeshObject* pCow = new cMeshObject();
    pCow->bIsWireframe = true;
    pCow->bOverrideVertexModelColour = true;
    pCow->colourRGB = glm::vec3(0.0f, 1.0f, 0.0f);
    pCow->position.x = -10.f;
    pCow->orientation.z = 90.0f;
    pCow->meshFileName = "assets/models/cow.ply";

    cMeshObject* pCow2 = new cMeshObject();
    pCow2->bIsWireframe = false;
    pCow2->bOverrideVertexModelColour = true;
    pCow2->colourRGB = glm::vec3(1.0f, 0.0f, 0.0f);
    pCow2->position.x = 10.f;
    pCow2->scale = 0.5f;
    pCow2->meshFileName = "assets/models/cow.ply";

    ::g_pMeshesToDraw.push_back(pCow);
    ::g_pMeshesToDraw.push_back(pCow2);

    cMeshObject* pTeapot = new cMeshObject();
    pTeapot->bIsVisible = false;
    pTeapot->bIsWireframe = true;
    pTeapot->scale = 0.2f;
    pTeapot->orientation.x = 90.0f;
    pTeapot->meshFileName = "assets/models/Utah_Teapot.ply";


    ::g_pMeshesToDraw.push_back(pTeapot);

    cMeshObject* pDolphin = new cMeshObject();
    pDolphin->meshFileName = "assets/models/dolphin.ply";
    pDolphin->scale = 0.02f;
    pDolphin->position.y = 10.0f;
    pDolphin->orientation.z = 45.0f;

    ::g_pMeshesToDraw.push_back(pDolphin);

    cMeshObject* pDolphin2 = new cMeshObject();
    pDolphin2->meshFileName = "assets/models/dolphin.ply";
    pDolphin2->scale = 0.02f;
    pDolphin2->position.y = -10.0f;
    pDolphin2->orientation.z = -45.0f;

    ::g_pMeshesToDraw.push_back(pDolphin2);

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);



    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        //       mat4x4 m, p, mvp;
        glm::mat4 matModel, matProj, matView;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        GLint proj_location = glGetUniformLocation(program, "mProj");
        GLint view_location = glGetUniformLocation(program, "mView");
        GLint Model_location = glGetUniformLocation(program, "mModel");

        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        matProj = glm::perspective(0.6f,
            ratio,
            0.1f,
            1000.0f);

        matView = glm::mat4(1.0f);

        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        matView = glm::lookAt(g_cameraEye,
            cameraTarget,
            upVector);

        glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(matProj));
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(matView));

        for (unsigned int index = 0; index != ::g_pMeshesToDraw.size(); index++)
        {
            cMeshObject* pCurrentMesh = ::g_pMeshesToDraw[index];
            if (!pCurrentMesh->bIsVisible)
            {
                continue;
            }

            GLint useOverrideColor_location = glGetUniformLocation(program, "bUseOverrideColor");
            GLint overrideColor_location = glGetUniformLocation(program, "colorOverride");

            if (pCurrentMesh->bOverrideVertexModelColour)
            {
                glUniform3f(overrideColor_location, pCurrentMesh->colourRGB.r,
                    pCurrentMesh->colourRGB.g, pCurrentMesh->colourRGB.b);

                glUniform1f(useOverrideColor_location, GL_TRUE); // 1.0f

            }
            else
            {
                glUniform1f(useOverrideColor_location, GL_FALSE);
            }

            //         mat4x4_identity(m);
            matModel = glm::mat4(1.0f);

            glm::mat4 translation = glm::translate(glm::mat4(1.0f), pCurrentMesh->position);

            //mat4x4_rotate_Z(m, m, (float) glfwGetTime());
            glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
                pCurrentMesh->orientation.x,
                glm::vec3(1.0f, 0.0f, 0.0f));

            glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
                pCurrentMesh->orientation.y,
                glm::vec3(0.0f, 1.0f, 0.0f));

            glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
                pCurrentMesh->orientation.z,
                glm::vec3(0.0f, 0.0f, 1.0f));

            float uniformScale = pCurrentMesh->scale;
            glm::mat4 scaleXYZ = glm::scale(glm::mat4(1.0f), 
                glm::vec3(uniformScale, uniformScale, uniformScale));

            matModel = matModel * translation * rotateX * rotateY * rotateZ * scaleXYZ;

            //m = m * rotateZ;

            //mat4x4_mul(mvp, p, m);
            //mvp = matProj * matView * matModel;

            if (pCurrentMesh->bIsWireframe)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            

            //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
            glUniformMatrix4fv(Model_location, 1, GL_FALSE, glm::value_ptr(matModel));

            //glDrawArrays(GL_TRIANGLES, 0, g_NumVerticiesToDraw);
            sModelDrawInfo modelToDraw;

            if (::g_pMeshManager->FindDrawInfoByModelName(pCurrentMesh->meshFileName,
                modelToDraw))
            {
                glBindVertexArray(modelToDraw.VAO_ID);
                glDrawElements(GL_TRIANGLES, modelToDraw.numberOfIndices,
                    GL_UNSIGNED_INT, (void*)0);
                glBindVertexArray(0);
            }
        }
        std::stringstream ssWindowTitle;

        ssWindowTitle << "Camera (XYZ)" << ::g_cameraEye.x << ","
            << ::g_cameraEye.y << ", " << ::g_cameraEye.z << std::endl;

        glfwSetWindowTitle(window, ssWindowTitle.str().c_str());

       /* if (::g_pMeshManager->FindDrawInfoByModelName("assets/models/cow.ply",
            modelToDraw))
        {
            glBindVertexArray(modelToDraw.VAO_ID);
            glDrawElements(GL_TRIANGLES, modelToDraw.numberOfIndices, 
                GL_UNSIGNED_INT, (void*)0);
            glBindVertexArray(0);
        }

        if (::g_pMeshManager->FindDrawInfoByModelName("assets/models/Utah_Teapot.ply",
            modelToDraw))
        {
            glBindVertexArray(modelToDraw.VAO_ID);
            glDrawElements(GL_TRIANGLES, modelToDraw.numberOfIndices,
                GL_UNSIGNED_INT, (void*)0);
            glBindVertexArray(0);
        }

        if (::g_pMeshManager->FindDrawInfoByModelName("assets/models/dolphin.ply",
            modelToDraw))
        {
            glBindVertexArray(modelToDraw.VAO_ID);
            glDrawElements(GL_TRIANGLES, modelToDraw.numberOfIndices,
                GL_UNSIGNED_INT, (void*)0);
            glBindVertexArray(0);
        }*/



        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
