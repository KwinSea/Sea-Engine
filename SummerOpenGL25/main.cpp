//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
#include "GlobalOpenGL.h"
//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <filesystem>

#include "cShaderManager/cShaderManager.h"
#include "cVAOManager/cVAOManager.h"
#include "cLightManager.h"
#include "cMeshObject.h"
#include "cLightHelper/cLightHelper.h"
#include "Camera.h"
#include "Functions.h"


cShaderManager* g_pTheShaderManager = NULL;
cVAOManager* g_pMeshManager = NULL;
cLightManager* g_pLights = NULL;

cMeshObject* g_pSmoothSphere = NULL;
cMeshObject* g_pSelectedMeshIndicator = NULL;

extern unsigned int g_selectedLightIndex;
extern unsigned int g_selectedObjectIndex;

extern float object_move_speed;
extern float object_rotate_speed;
extern float light_move_speed;

extern bool lightDebug;
extern bool meshDebug;

unsigned int screenWidth = 1280;
unsigned int screenHeight = 720;

unsigned int g_LightingType = 0;
unsigned int g_NumVerticiesToDraw = 0;
unsigned int g_SizeOfVertexArrayInBytes = 0;

Camera camera (screenWidth, screenHeight, glm::vec3(2000.0f, 2000.0f, 2000.0f));

void LoadFilesIntoVAOManager(GLuint program);

std::vector<cMeshObject*> g_pMeshesToDraw;
void LoadModelsIntoScene();

void DrawMesh(cMeshObject* pCurrentMesh, GLint program);

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);



int main(void) {
    GLFWwindow* window;


    GLuint program;
    GLint mvp_location;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(screenWidth, screenHeight, "Simple example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);


    ::g_pTheShaderManager = new cShaderManager();

    ::g_pTheShaderManager->setBasePath("assets/shaders/");

    cShaderManager::cShader vert_shader;
    vert_shader.fileName = "vertex_shader.glsl";

    cShaderManager::cShader frag_shader;
    frag_shader.fileName = "fragment_shader.glsl";


    if (::g_pTheShaderManager->createProgramFromFile("shader1", vert_shader, frag_shader)) {
        std::cout << "Shaders succesfully created!" << std::endl;
    } else {
        std::cout << ::g_pTheShaderManager->getLastError() << std::endl;
    }

    program = ::g_pTheShaderManager->getIDFromFriendlyName("shader1");

    mvp_location = glGetUniformLocation(program, "MVP");

    LoadFilesIntoVAOManager(program);

    LoadModelsIntoScene();

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    ::g_pLights = new cLightManager();
    ::g_pLights->GetUniformLocations(program);

    // Light 1
    ::g_pLights->theLights[0].param2.x = 0.0f; // turn on
    ::g_pLights->theLights[0].param1.x = 0.0f; // light type = point light
    g_pLights->theLights[0].position = glm::vec4(13000.0f, 1000.0f, 13000.0f, 0.5f);
    g_pLights->theLights[0].diffuse = glm::vec4(RGBify(255, 187, 0), 1.0f);

    g_pLights->theLights[0].atten.x = 0.0f; // constant
    g_pLights->theLights[0].atten.y = 0.0001f; // linear
    g_pLights->theLights[0].atten.z = 0.00001f; // quadratic

    // Light 2
    ::g_pLights->theLights[1].param2.x = 0.0f; // turn on
    ::g_pLights->theLights[1].param1.x = 0.0f; // light type = point light
    g_pLights->theLights[1].position = glm::vec4(-500.0f, 1000.0f, 13000.0f, 0.5f);
    g_pLights->theLights[1].diffuse = glm::vec4(RGBify(255, 187, 0), 1.0f);

    g_pLights->theLights[1].atten.x = 0.0f; // constant
    g_pLights->theLights[1].atten.y = 0.0001f; // linear
    g_pLights->theLights[1].atten.z = 0.00001f; // quadratic

    // Light 3
    ::g_pLights->theLights[2].param2.x = 0.0f; // turn on
    ::g_pLights->theLights[2].param1.x = 0.0f; // light type = point light
    g_pLights->theLights[2].position = glm::vec4(13000.0f, 1000.0f, 0.0f, 0.5f);
    g_pLights->theLights[2].diffuse = glm::vec4(RGBify(255, 187, 0), 1.0f);

    g_pLights->theLights[2].atten.x = 0.0f; // constant
    g_pLights->theLights[2].atten.y = 0.0001f; // linear
    g_pLights->theLights[2].atten.z = 0.00001f; // quadratic

    // Light 4
    ::g_pLights->theLights[3].param2.x = 0.0f; // turn on
    ::g_pLights->theLights[3].param1.x = 0.0f; // light type = point light
    g_pLights->theLights[3].position = glm::vec4(-500.0f, 1000.0f, -500.0f, 0.5f);
    g_pLights->theLights[3].diffuse = glm::vec4(RGBify(255, 187, 0), 1.0f);

    g_pLights->theLights[3].atten.x = 0.0f; // constant
    g_pLights->theLights[3].atten.y = 0.0001f; // linear
    g_pLights->theLights[3].atten.z = 0.00001f; // quadratic

    // Camera Light
    g_pLights->theLights[19].param2.x = 0.0f; // turn on
    g_pLights->theLights[19].param1.x = 1.0f;
    g_pLights->theLights[19].position = glm::vec4(camera.Position, 1.0f);
    g_pLights->theLights[19].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    g_pLights->theLights[19].param1.z = 10.0f;

    g_pLights->theLights[19].atten.x = 0.0f; // constant
    g_pLights->theLights[19].atten.y = 0.001f; // linear
    g_pLights->theLights[19].atten.z = 0.00001f; // quadratic

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        camera.InputHandler(window);
        camera.Matrix(60.0f, 0.1f, 100.0f, *g_pTheShaderManager, "camMatrix");

        GLint eyeLocation_UL = glGetUniformLocation(program, "eyeLocation");

        glUniform3f(eyeLocation_UL, camera.Position.x, camera.Position.y, camera.Position.z);

        // Lighting type
        GLint lightingType_UL = glGetUniformLocation(program, "lightingType");
        glUniform1i(lightingType_UL, g_LightingType);

        ::g_pLights->UpdateShaderUniforms(program);

        for (unsigned int index = 0; index != ::g_pMeshesToDraw.size(); index++) {
            cMeshObject* pCurrentMesh = ::g_pMeshesToDraw[index];
            DrawMesh(pCurrentMesh, program);
        }

        // Torch Flicker
        g_pLights->theLights[11].atten.y = 0.001f + (rand() / (float)RAND_MAX) * 0.0012f; // linear
        g_pLights->theLights[11].atten.z = 0.0001f + (rand() / (float)RAND_MAX) * 0.00012f; // quadratic

        g_pLights->theLights[12].atten.y = 0.001f + (rand() / (float)RAND_MAX) * 0.0012f; // linear
        g_pLights->theLights[12].atten.z = 0.0001f + (rand() / (float)RAND_MAX) * 0.00012f; // quadratic

        g_pLights->theLights[13].atten.y = 0.001f + (rand() / (float)RAND_MAX) * 0.0012f; // linear
        g_pLights->theLights[13].atten.z = 0.0001f + (rand() / (float)RAND_MAX) * 0.00012f; // quadratic

        g_pLights->theLights[14].atten.y = 0.001f + (rand() / (float)RAND_MAX) * 0.0012f; // linear
        g_pLights->theLights[14].atten.z = 0.0001f + (rand() / (float)RAND_MAX) * 0.00012f; // quadratic

        g_pLights->theLights[15].atten.y = 0.001f + (rand() / (float)RAND_MAX) * 0.0012f; // linear
        g_pLights->theLights[15].atten.z = 0.0001f + (rand() / (float)RAND_MAX) * 0.00012f; // quadratic

        g_pLights->theLights[16].atten.y = 0.001f + (rand() / (float)RAND_MAX) * 0.0012f; // linear
        g_pLights->theLights[16].atten.z = 0.0001f + (rand() / (float)RAND_MAX) * 0.00012f; // quadratic

        g_pLights->theLights[17].atten.y = 0.001f + (rand() / (float)RAND_MAX) * 0.0012f; // linear
        g_pLights->theLights[17].atten.z = 0.0001f + (rand() / (float)RAND_MAX) * 0.00012f; // quadratic

        g_pLights->theLights[18].atten.y = 0.001f + (rand() / (float)RAND_MAX) * 0.0012f; // linear
        g_pLights->theLights[18].atten.z = 0.0001f + (rand() / (float)RAND_MAX) * 0.00012f; // quadratic

        ::g_pSmoothSphere = new cMeshObject();
        ::g_pSmoothSphere->meshFileName = "assets/models/Isoshphere_smooth_inverted_normals_xyz_n_rgba.ply";
        ::g_pSmoothSphere->bIsWireframe = true;
        ::g_pSmoothSphere->bOverrideVertexModelColour = true;
        ::g_pSmoothSphere->bIsVisible = lightDebug;
        ::g_pSmoothSphere->position = glm::vec3(
            ::g_pLights->theLights[::g_selectedLightIndex].position.x,
            ::g_pLights->theLights[::g_selectedLightIndex].position.y,
            ::g_pLights->theLights[::g_selectedLightIndex].position.z);


        if (!g_pMeshesToDraw.empty()) {
            // Selected Mesh Indicator
            ::g_pSelectedMeshIndicator = new cMeshObject();
            ::g_pSelectedMeshIndicator->meshFileName = ::g_pMeshesToDraw[::g_selectedObjectIndex]->meshFileName;
            ::g_pSelectedMeshIndicator->bIsWireframe = true;
            ::g_pSelectedMeshIndicator->bOverrideVertexModelColour = true;
            ::g_pSelectedMeshIndicator->bIsVisible = meshDebug;
            ::g_pSelectedMeshIndicator->scale = ::g_pMeshesToDraw[::g_selectedObjectIndex]->scale * 1.01f;
            ::g_pSelectedMeshIndicator->colourRGB = glm::vec3(1.0f);
            ::g_pSelectedMeshIndicator->specularHighLightRGB = glm::vec3(1.0f);
            ::g_pSelectedMeshIndicator->specularPower = 0.0f;
            ::g_pSelectedMeshIndicator->position = glm::vec3(
                ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x,
                ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.y,
                ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z);
            ::g_pSelectedMeshIndicator->orientation = glm::vec3(
                ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.x,
                ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.y,
                ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.z);

            DrawMesh(g_pSelectedMeshIndicator, program);
        }

        cLightHelper lightHelper;

        const float errorValueforLightLevelGuess = 0.01f;
        const float infiniteDistance = 10000.0f;

        // where the light located
        ::g_pSmoothSphere->scale = 0.1f;
        ::g_pSmoothSphere->colourRGB = glm::vec3(1.0f);
        DrawMesh(g_pSmoothSphere, program);

        float distanceAt75Percent = lightHelper.calcApproxDistFromAtten(0.75f,
                                                                        errorValueforLightLevelGuess, infiniteDistance,
                                                                        ::g_pLights->theLights[::g_selectedLightIndex].
                                                                        atten.x,
                                                                        ::g_pLights->theLights[::g_selectedLightIndex].
                                                                        atten.y,
                                                                        ::g_pLights->theLights[::g_selectedLightIndex].
                                                                        atten.z);

        ::g_pSmoothSphere->scale = distanceAt75Percent;
        ::g_pSmoothSphere->colourRGB = glm::vec3(1.0f, 0.0f, 0.0f);
        DrawMesh(g_pSmoothSphere, program);

        float distanceAt50Percent = lightHelper.calcApproxDistFromAtten(0.5f,
                                                                        errorValueforLightLevelGuess, infiniteDistance,
                                                                        ::g_pLights->theLights[::g_selectedLightIndex].
                                                                        atten.x,
                                                                        ::g_pLights->theLights[::g_selectedLightIndex].
                                                                        atten.y,
                                                                        ::g_pLights->theLights[::g_selectedLightIndex].
                                                                        atten.z);

        ::g_pSmoothSphere->scale = distanceAt50Percent;
        ::g_pSmoothSphere->colourRGB = glm::vec3(0.0f, 1.0f, 0.0f);
        DrawMesh(g_pSmoothSphere, program);

        float distanceAt25Percent = lightHelper.calcApproxDistFromAtten(0.25f,
                                                                        errorValueforLightLevelGuess, infiniteDistance,
                                                                        ::g_pLights->theLights[::g_selectedLightIndex].
                                                                        atten.x,
                                                                        ::g_pLights->theLights[::g_selectedLightIndex].
                                                                        atten.y,
                                                                        ::g_pLights->theLights[::g_selectedLightIndex].
                                                                        atten.z);

        ::g_pSmoothSphere->scale = distanceAt25Percent;
        ::g_pSmoothSphere->colourRGB = glm::vec3(0.0f, 0.0f, 1.0f);
        DrawMesh(g_pSmoothSphere, program);


        std::stringstream ssWindowTitle;

        ssWindowTitle << "Session info [ Camera Speed: " << camera.speed << " | Object Speed: " << object_move_speed << " | Rotation Speed: " << object_rotate_speed << " | Light Speed: " << light_move_speed << " ]   [ Camera X:" << camera.Position.x << " | Camera Y:" << camera.Position.y << " | Camera Z:" << camera.Position.z << " ]"<< std::endl;

        glfwSetWindowTitle(window, ssWindowTitle.str().c_str());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void LoadFilesIntoVAOManager(GLuint program) {
    ::g_pMeshManager = new cVAOManager();

    std::string AssetsFolder = "assets/";
    std::vector<std::string> modelFiles;

    std::cout << "Searching for models in assets folder...\n";
    std::vector<sModelDrawInfo> meshObjects;

    try {
        for (const std::filesystem::directory_entry& file : std::filesystem::recursive_directory_iterator(AssetsFolder)) {
            if (file.is_regular_file() && file.path().extension() == ".ply") {
                sModelDrawInfo meshObject;
                std::string meshFile = file.path().generic_string();
                if (!::g_pMeshManager->LoadModelIntoVAO(meshFile, meshObject, program, 1.0f)) {
                    std::cout << "Model at: " << meshFile << " not loaded into VAO!" << std::endl;
                } else {
                    meshObjects.push_back(meshObject);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& ex) {
        std::cerr << ex.what() << '\n';
    }
    std::cout << "Finished searching\n" << meshObjects.size() << " Mesh found";
}

void LoadModelsIntoScene() {

    LoadMaze("assets/maze.txt");

    cMeshObject* pCompass = new cMeshObject();
    pCompass->meshFileName = "assets/models/Dungeon_models/Props/SM_Item_Compass_01.ply";
    g_pMeshesToDraw.push_back(pCompass);
}

void DrawMesh(cMeshObject* pCurrentMesh, GLint program) {
    if (!pCurrentMesh->bIsVisible) {
        return;
    }

    glm::mat4 matModel;
    GLint Model_location = glGetUniformLocation(program, "mModel");
    GLint useOverrideColor_location = glGetUniformLocation(program, "bUseOverrideColor");
    GLint overrideColor_location = glGetUniformLocation(program, "colorOverride");

    if (pCurrentMesh->bOverrideVertexModelColour) {
        glUniform3f(overrideColor_location, pCurrentMesh->colourRGB.r,
                    pCurrentMesh->colourRGB.g, pCurrentMesh->colourRGB.b);

        glUniform1f(useOverrideColor_location, GL_TRUE); // 1.0f
    } else {
        glUniform1f(useOverrideColor_location, GL_FALSE);
    }

    // ste specular value
    GLint vertSpecular_UL = glGetUniformLocation(program, "vertSpecular");

    glUniform4f(vertSpecular_UL,
                pCurrentMesh->specularHighLightRGB.r,
                pCurrentMesh->specularHighLightRGB.g,
                pCurrentMesh->specularHighLightRGB.b,
                pCurrentMesh->specularPower);

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

    if (pCurrentMesh->bIsWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }


    //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    glUniformMatrix4fv(Model_location, 1, GL_FALSE, glm::value_ptr(matModel));

    GLint mModelIt_location = glGetUniformLocation(program, "mModel_InverseTranpose");

    // gets rid of any translation (movement) and scaling. leaves only roation
    glm::mat4 matModelIt = glm::inverse(glm::transpose(matModel));
    glUniformMatrix4fv(mModelIt_location, 1, GL_FALSE, glm::value_ptr(matModelIt));

    //glDrawArrays(GL_TRIANGLES, 0, g_NumVerticiesToDraw);
    sModelDrawInfo modelToDraw;

    if (::g_pMeshManager->FindDrawInfoByModelName(pCurrentMesh->meshFileName,
                                                  modelToDraw)) {
        glBindVertexArray(modelToDraw.VAO_ID);
        glDrawElements(GL_TRIANGLES, modelToDraw.numberOfIndices,
                       GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
    }
}
