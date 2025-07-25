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

#include "cShaderManager/cShaderManager.h"
#include "cVAOManager/cVAOManager.h"
#include "cLightManager.h"
#include "cMeshObject.h"
#include "cLightHelper/cLightHelper.h"
#include "Camera.h"


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

glm::vec3 RGBify (int red, int green, int blue) {return glm::vec3(red / 255.0f, green / 255.0f, blue / 255.0f);}

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

    // Light debug

    sModelDrawInfo SmoothSphereMeshInfo;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Isoshphere_smooth_inverted_normals_xyz_n_rgba.ply",
                                           SmoothSphereMeshInfo, program, true, true, false, 1.0f)) {
        std::cout << "Isoshphere_smooth_inverted_normals_xyz_n_rgba.ply not loaded into VAO!" << std::endl;
    }

    // Floors
    sModelDrawInfo meshInfoFloor_1;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_01.ply",
                                            meshInfoFloor_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Dwarf_Floor_01.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoFloor_2;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_02.ply",
                                            meshInfoFloor_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Dwarf_Floor_02.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoFloor_3;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_03.ply",
                                            meshInfoFloor_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Dwarf_Floor_03.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoFloor_4;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_04.ply",
                                            meshInfoFloor_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Dwarf_Floor_04.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoFloor_5;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_05.ply",
                                            meshInfoFloor_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Dwarf_Floor_05.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoFloor_6;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_06.ply",
                                            meshInfoFloor_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Dwarf_Floor_06.ply not loaded into VAO!" << std::endl;
    }

    // Walls
    sModelDrawInfo meshInfoWall_1;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_01.ply",
                                            meshInfoWall_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Dwarf_Wall_01.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoWall_2;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_02.ply",
                                            meshInfoWall_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Dwarf_Wall_02.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoWall_3;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_03.ply",
                                            meshInfoWall_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Dwarf_Wall_03.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoWall_4;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_04.ply",
                                            meshInfoWall_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Dwarf_Wall_04.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoWall_5;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_05.ply",
                                            meshInfoWall_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Dwarf_Wall_05.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoWall_6;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_06.ply",
                                            meshInfoWall_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Dwarf_Wall_06.ply not loaded into VAO!" << std::endl;
    }

    // Gates
    sModelDrawInfo meshInfoGate_1;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_Archway_01.ply",
                                            meshInfoWall_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Dwarf_Wall_Archway_01.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoGate_2;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_Archway_02.ply",
                                            meshInfoWall_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Dwarf_Wall_Archway_02.ply not loaded into VAO!" << std::endl;
    }

    // Props
    sModelDrawInfo meshInfoTorch;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Torches/SM_Prop_Dwarf_Torch_01.ply",
                                            meshInfoWall_1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Prop_Dwarf_Torch_01.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoCompass;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Item_Compass_01.ply",
                                            meshInfoCompass, program, true, true, true, 1.0f)) {
        std::cout << "SM_Item_Compass_01.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoAnvil;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Prop_Anvil_01.ply",
                                            meshInfoCompass, program, true, true, true, 1.0f)) {
        std::cout << "SM_Prop_Anvil_01.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoBottle1;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Item_Bottle_01.ply",
                                            meshInfoBottle1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Item_Bottle_01.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoBottle2;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Item_Bottle_02.ply",
                                            meshInfoBottle2, program, true, true, true, 1.0f)) {
        std::cout << "SM_Item_Bottle_02.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoBottle3;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Item_Bottle_03.ply",
                                            meshInfoBottle3, program, true, true, true, 1.0f)) {
        std::cout << "SM_Item_Bottle_03.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoBottle4;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Item_Bottle_04.ply",
                                            meshInfoBottle4, program, true, true, true, 1.0f)) {
        std::cout << "SM_Item_Bottle_04.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoTankard;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Prop_Tankard_01.ply",
                                            meshInfoTankard, program, true, true, true, 1.0f)) {
        std::cout << "SM_Prop_Tankard_01.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoCup;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Item_Cup_01.ply",
                                            meshInfoCup, program, true, true, true, 1.0f)) {
        std::cout << "SM_Item_Cup_01.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoBag;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Item_Bag_02.ply",
                                            meshInfoBag, program, true, true, true, 1.0f)) {
        std::cout << "SM_Item_Bag_02.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoDead1;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Dead/SM_Prop_DeadBody_Dwarf_01.ply",
                                            meshInfoDead1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Prop_DeadBody_Dwarf_01.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoDead2;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Dead/SM_Prop_DeadBody_Nomad_01.ply",
                                            meshInfoDead2, program, true, true, true, 1.0f)) {
        std::cout << "SM_Prop_DeadBody_Nomad_01.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoDead3;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Dead/SM_Prop_DeadBody_Dwarf_04.ply",
                                            meshInfoDead3, program, true, true, true, 1.0f)) {
        std::cout << "SM_Prop_DeadBody_Dwarf_04.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoDead4;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Dead/SM_Prop_DeadBody_Nomad_02.ply",
                                            meshInfoDead4, program, true, true, true, 1.0f)) {
        std::cout << "SM_Prop_DeadBody_Nomad_02.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoCandelabra1;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Item_Candelabra_01.ply",
                                            meshInfoDead4, program, true, true, true, 1.0f)) {
        std::cout << "SM_Item_Candelabra_01.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoCandelabra2;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Item_Candelabra_02.ply",
                                            meshInfoDead4, program, true, true, true, 1.0f)) {
        std::cout << "SM_Item_Candelabra_02.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoBarrel1;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Prop_Barrel_01.ply",
                                            meshInfoBarrel1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Prop_Barrel_01.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoBarrel2;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Prop_Barrel_02.ply",
                                            meshInfoBarrel2, program, true, true, true, 1.0f)) {
        std::cout << "SM_Prop_Barrel_02.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoBarrel3;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Prop_Barrel_03.ply",
                                            meshInfoBarrel3, program, true, true, true, 1.0f)) {
        std::cout << "SM_Prop_Barrel_03.ply not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshInfoMagicRockLG;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Crystals/SM_Env_Crystals_Cluster_Large_01.ply",
                                            meshInfoMagicRockLG, program, true, true, true, 1.0f)) {
        std::cout << "SM_Prop_Barrel_03.ply not loaded into VAO!" << std::endl;
                                            }

    sModelDrawInfo meshInfoMagicRockSM;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Crystals/SM_Prop_Crystal_Rock_A_01.ply",
                                            meshInfoMagicRockSM, program, true, true, true, 1.0f)) {
        std::cout << "SM_Prop_Barrel_03.ply not loaded into VAO!" << std::endl;
                                            }

    // Plants
    sModelDrawInfo meshInfoTree1;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Plants/SM_Generic_Tree_01.ply",
                                            meshInfoTree1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Generic_Tree_01.ply not loaded into VAO!" << std::endl;
                                            }

    sModelDrawInfo meshInfoTree2;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Plants/SM_Env_Mangrove_Tree_01.ply",
                                            meshInfoTree1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Mangrove_Tree_01.ply not loaded into VAO!" << std::endl;
                                            }

    sModelDrawInfo meshInfoTree3;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Plants/SM_Env_PalmTree_01.ply",
                                            meshInfoTree3, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_PalmTree_01.ply not loaded into VAO!" << std::endl;
                                            }

    sModelDrawInfo meshInfoPlants1;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Plants/SM_Env_Flowers_01.ply",
                                            meshInfoPlants1, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Flowers_01.ply not loaded into VAO!" << std::endl;
                                            }

    sModelDrawInfo meshInfoPlants2;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Plants/SM_Env_GrassPatch_01.ply",
                                            meshInfoPlants2, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_GrassPatch_01.ply not loaded into VAO!" << std::endl;
                                            }

    sModelDrawInfo meshInfoPlants3;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Plants/SM_Env_Plant_01.ply",
                                            meshInfoPlants3, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Plant_01.ply not loaded into VAO!" << std::endl;
                                            }

    sModelDrawInfo meshInfoWell;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props/SM_Env_Camp_Well_01.ply",
                                            meshInfoPlants3, program, true, true, true, 1.0f)) {
        std::cout << "SM_Env_Camp_Well_01.ply not loaded into VAO!" << std::endl;
                                            }
}

void LoadModelsIntoScene() {

    // Floor
    int z = 500;
    int x = 0;
    for (int i = 1; i < 27; i++) {
        x = 0;
        for (int j = 1; j < 28; j++) {
            std::ostringstream ss;
            ss << "assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_0" << rand() % 6 + 1 << ".ply";
            cMeshObject* pFloor = new cMeshObject();
            pFloor->meshFileName = ss.str();
            pFloor->position.x = x;
            pFloor->position.z = z;
            pFloor->colourRGB = RGBify(48, 48, 45);
            pFloor->bOverrideVertexModelColour = true;

            ::g_pMeshesToDraw.push_back(pFloor);
            x += 500;
        }
        z += 500;
    }

    // North outer wall
    x = -500;
    for (int i = 1; i < 28; i++) {
        std::ostringstream ss;
        ss << "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_0" << rand() % 6 + 1 << ".ply";
        cMeshObject* pWall = new cMeshObject();
        pWall->meshFileName = ss.str();
        g_pMeshesToDraw.push_back(pWall);
        pWall->position.x = x;
        pWall->position.z = 0;
        pWall->orientation.y = glm::radians(-180.0f);
        pWall->colourRGB = RGBify(77, 75, 60);
        pWall->bOverrideVertexModelColour = true;
        x += 500;
    }

    // East outer wall
    z = 0;
    for (int i = 1; i < 27; i++) {
        std::ostringstream ss;

        if (i != 6) {
            ss << "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_0" << rand() % 6 + 1 << ".ply";
        } else {
            ss << "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_Archway_02.ply";
        }

        cMeshObject* pWall = new cMeshObject();
        pWall->meshFileName = ss.str();
        g_pMeshesToDraw.push_back(pWall);
        pWall->position.z = z;
        pWall->position.x = 13000;
        pWall->orientation.y = glm::radians(90.0f);
        pWall->colourRGB = RGBify(77, 75, 60);
        pWall->bOverrideVertexModelColour = true;
        z += 500;
    }

    // South outer wall
    x = 0;
    for (int i = 1; i < 28; i++) {
        std::ostringstream ss;

        if (i != 23) {
            ss << "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_0" << rand() % 6 + 1 << ".ply";
        } else {
            ss << "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_Archway_01.ply";
        }

        cMeshObject* pWall = new cMeshObject();
        pWall->meshFileName = ss.str();
        g_pMeshesToDraw.push_back(pWall);
        pWall->position.x = x;
        pWall->position.z = 13000;
        pWall->colourRGB = RGBify(77, 75, 60);
        pWall->bOverrideVertexModelColour = true;
        x += 500;
    }

    // West outer wall
    z = 500;
    for (int i = 1; i < 27; i++) {
        std::ostringstream ss;
        ss << "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_0" << rand() % 6 + 1 << ".ply";
        cMeshObject* pWall = new cMeshObject();
        pWall->meshFileName = ss.str();
        g_pMeshesToDraw.push_back(pWall);
        pWall->position.z = z;
        pWall->position.x = -500;
        pWall->orientation.y = glm::radians(-90.0f);
        pWall->colourRGB = RGBify(77, 75, 60);
        pWall->bOverrideVertexModelColour = true;
        z += 500;
    }

    std::ifstream file("assets/maze.txt");

    std::string line;
    std::getline(file, line);

    char letter;
    std::string currentLine;
    bool isFirstChar = true;

    z = 0;
    int count = 1;
    while (file.get(letter)) {
        if (letter == '\n') {
            for (int i = 0; i < 2; i++){
                x = 0;
                for (int j = 1; j < currentLine.length() - 1; j++) {
                    if (currentLine[j] == 'X') {
                        std::ostringstream ss;

                        ss << "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_0" << rand() % 6 + 1 << ".ply";
                        cMeshObject* pWall1 = new cMeshObject();
                        pWall1->meshFileName = ss.str();
                        g_pMeshesToDraw.push_back(pWall1);
                        pWall1->position.x = x;
                        pWall1->position.z = z;
                        pWall1->colourRGB = RGBify(168, 164, 133);
                        pWall1->bOverrideVertexModelColour = true;
                        ss.str("");

                        ss << "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_0" << rand() % 6 + 1 << ".ply";
                        cMeshObject* pWall2 = new cMeshObject();
                        pWall2->meshFileName = ss.str();
                        g_pMeshesToDraw.push_back(pWall2);
                        pWall2->position.x = x - 500;
                        pWall2->position.z = z + 500;
                        pWall2->orientation.y = glm::radians(180.0f);
                        pWall2->colourRGB = RGBify(168, 164, 133);
                        pWall2->bOverrideVertexModelColour = true;
                        ss.str("");

                        ss << "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_0" << rand() % 6 + 1 << ".ply";
                        cMeshObject* pWall3 = new cMeshObject();
                        pWall3->meshFileName = ss.str();
                        g_pMeshesToDraw.push_back(pWall3);
                        pWall3->position.x = x - 500;
                        pWall3->position.z = z;
                        pWall3->orientation.y = glm::radians(90.0f);
                        pWall3->colourRGB = RGBify(168, 164, 133);
                        pWall3->bOverrideVertexModelColour = true;
                        ss.str("");

                        ss << "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_0" << rand() % 6 + 1 << ".ply";
                        cMeshObject* pWall4 = new cMeshObject();
                        pWall4->meshFileName = ss.str();
                        g_pMeshesToDraw.push_back(pWall4);
                        pWall4->position.x = x;
                        pWall4->position.z = z + 500;
                        pWall4->orientation.y = glm::radians(-90.0f);
                        pWall4->colourRGB = RGBify(168, 164, 133);
                        pWall4->bOverrideVertexModelColour = true;
                        ss.str("");
                    }
                    x += 500;
                }
                z += 500;
            }
            currentLine.clear();
            count++;
            if (count > 13) {
                break;
            }
        } else {
            currentLine += letter;
        }
    }

    file.close();

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
