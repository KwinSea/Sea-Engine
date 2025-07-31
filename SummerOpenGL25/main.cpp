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
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"


cShaderManager* g_pTheShaderManager = NULL;
cVAOManager* g_pMeshManager = NULL;
cLightManager* g_pLights = NULL;

cMeshObject* g_pSmoothSphere = NULL;
cMeshObject* g_pSelectedMeshIndicator = NULL;

extern int g_selectedLightIndex;
extern int g_selectedObjectIndex;

extern float object_move_speed;
extern float object_rotate_speed;
extern float light_move_speed;

extern bool lightDebug;
extern bool meshDebug;

unsigned int screenWidth = 1280;
unsigned int screenHeight = 720;

int g_LightingType = 0;
unsigned int g_NumVerticiesToDraw = 0;
unsigned int g_SizeOfVertexArrayInBytes = 0;

bool usingGui = false;

Camera camera (screenWidth, screenHeight, glm::vec3(3000.0f, 1500.0f, 3000.0f));

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
    g_pLights->theLights[0].diffuse = glm::vec4(RGBify(255, 187, 0.0), 1.0);

    g_pLights->theLights[0].atten.x = 0.0f; // constant
    g_pLights->theLights[0].atten.y = 0.0001f; // linear
    g_pLights->theLights[0].atten.z = 0.00001f; // quadratic

    // // Light 2
    // ::g_pLights->theLights[1].param2.x = 0.0f; // turn on
    // ::g_pLights->theLights[1].param1.x = 0.0f; // light type = point light
    // g_pLights->theLights[1].position = glm::vec4(-500.0f, 1000.0f, 13000.0f, 0.5f);
    // g_pLights->theLights[1].diffuse = glm::vec4(RGBify(255, 187, 0), 1.0f);
    //
    // g_pLights->theLights[1].atten.x = 0.0f; // constant
    // g_pLights->theLights[1].atten.y = 0.0001f; // linear
    // g_pLights->theLights[1].atten.z = 0.00001f; // quadratic
    //
    // // Light 3
    // ::g_pLights->theLights[2].param2.x = 0.0f; // turn on
    // ::g_pLights->theLights[2].param1.x = 0.0f; // light type = point light
    // g_pLights->theLights[2].position = glm::vec4(13000.0f, 1000.0f, 0.0f, 0.5f);
    // g_pLights->theLights[2].diffuse = glm::vec4(RGBify(255, 187, 0), 1.0f);
    //
    // g_pLights->theLights[2].atten.x = 0.0f; // constant
    // g_pLights->theLights[2].atten.y = 0.0001f; // linear
    // g_pLights->theLights[2].atten.z = 0.00001f; // quadratic
    //
    // // Light 4
    // ::g_pLights->theLights[3].param2.x = 0.0f; // turn on
    // ::g_pLights->theLights[3].param1.x = 0.0f; // light type = point light
    // g_pLights->theLights[3].position = glm::vec4(-500.0f, 1000.0f, -500.0f, 0.5f);
    // g_pLights->theLights[3].diffuse = glm::vec4(RGBify(255, 187, 0), 1.0f);
    //
    // g_pLights->theLights[3].atten.x = 0.0f; // constant
    // g_pLights->theLights[3].atten.y = 0.0001f; // linear
    // g_pLights->theLights[3].atten.z = 0.00001f; // quadratic
    //
    // // Camera Light
    // g_pLights->theLights[19].param2.x = 0.0f; // turn on
    // g_pLights->theLights[19].param1.x = 1.0f;
    // g_pLights->theLights[19].position = glm::vec4(camera.Position, 1.0f);
    // g_pLights->theLights[19].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    // g_pLights->theLights[19].param1.z = 10.0f;
    //
    // g_pLights->theLights[19].atten.x = 0.0f; // constant
    // g_pLights->theLights[19].atten.y = 0.001f; // linear
    // g_pLights->theLights[19].atten.z = 0.00001f; // quadratic

    // Initialize ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 420");
    ImGui::StyleColorsDark();

    ::g_pSelectedMeshIndicator = new cMeshObject();
    ::g_pSmoothSphere = new cMeshObject();

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!usingGui) {
            camera.InputHandler(window);
        }
        camera.Matrix(60.0f, 0.1f, 100.0f, *g_pTheShaderManager, "camMatrix");

        GLint eyeLocation_UL = glGetUniformLocation(program, "eyeLocation");

        glUniform3f(eyeLocation_UL, camera.Position.x, camera.Position.y, camera.Position.z);

        // Lighting type
        GLint lightingType_UL = glGetUniformLocation(program, "lightingType");
        glUniform1i(lightingType_UL, g_LightingType);

        ::g_pLights->UpdateShaderUniforms(program);

        std::vector<cMeshObject*> vecSolidThings;
        std::vector<cMeshObject*> vecTransparentThings;


        //       struct cThingDistance
        //       {
        //           float distToCamera;
        //           cMeshObject* pMeshObject;
        //       };
        //       std::vector<cThingDistance> vecTransparentThings;

        // Separate transparent from non-transparent
        for (unsigned int index = 0; index != ::g_pMeshesToDraw.size(); index++)
        {
            cMeshObject* pCurrentMesh = ::g_pMeshesToDraw[index];
            // Transparent?
            if (pCurrentMesh->opacityAlpha < 1.0f)
            {
                // It's transparent
                vecTransparentThings.push_back(pCurrentMesh);
            }
            else
            {
                // It's solid
                vecSolidThings.push_back(pCurrentMesh);
            }
        }

        // Sort transparent from "far from camera to near"
        for (unsigned int index = 0; index != vecTransparentThings.size(); index++)
        {
            cMeshObject* pCurrentMesh = vecTransparentThings[index];
            // Distance from object to camera
            float distToCamera = glm::distance(camera.Position, pCurrentMesh->position);
        }
        // Sort them
        // 1 pass of the bubble sort
        // Monkey sort
        // Beer at the camp fire sort

        // Memory = zero
        // CPU bound
        // 500-1000



        for (unsigned int index = 0; index != vecSolidThings.size(); index++)
        {
            cMeshObject* pCurrentMesh = vecSolidThings[index];
            DrawMesh(pCurrentMesh, program);
        }

        for (unsigned int index = 0; index != vecTransparentThings.size(); index++)
        {
            cMeshObject* pCurrentMesh = vecTransparentThings[index];
            DrawMesh(pCurrentMesh, program);
        }

        // Firefly Flicker
        if (rand() % 20 + 1 > 5) {
            g_pLights->theLights[9].param2.x = 1.0f; // turn on
        } else {
            g_pLights->theLights[9].param2.x = 0.0f; // turn off
        }

        if (rand() % 20 + 1 > 5) {
            g_pLights->theLights[10].param2.x = 1.0f; // turn on
        } else {
            g_pLights->theLights[10].param2.x = 0.0f; // turn off
        }

        if (rand() % 20 + 1 > 5) {
            g_pLights->theLights[11].param2.x = 1.0f; // turn on
        } else {
            g_pLights->theLights[11].param2.x = 0.0f; // turn off
        }

        if (rand() % 20 + 1 > 5) {
            g_pLights->theLights[12].param2.x = 1.0f; // turn on
        } else {
            g_pLights->theLights[12].param2.x = 0.0f; // turn off
        }

        if (rand() % 20 + 1 > 5) {
            g_pLights->theLights[13].param2.x = 1.0f; // turn on
        } else {
            g_pLights->theLights[13].param2.x = 0.0f; // turn off
        }

        if (rand() % 20 + 1 > 5) {
            g_pLights->theLights[14].param2.x = 1.0f; // turn on
        } else {
            g_pLights->theLights[14].param2.x = 0.0f; // turn off
        }

        if (rand() % 20 + 1 > 5) {
            g_pLights->theLights[15].param2.x = 1.0f; // turn on
        } else {
            g_pLights->theLights[15].param2.x = 0.0f; // turn off
        }

        if (rand() % 20 + 1 > 5) {
            g_pLights->theLights[16].param2.x = 1.0f; // turn on
        } else {
            g_pLights->theLights[16].param2.x = 0.0f; // turn off
        }

        if (rand() % 20 + 1 > 5) {
            g_pLights->theLights[17].param2.x = 1.0f; // turn on
        } else {
            g_pLights->theLights[17].param2.x = 0.0f; // turn off
        }

        if (rand() % 20 + 1 > 5) {
            g_pLights->theLights[18].param2.x = 1.0f; // turn on
        } else {
            g_pLights->theLights[18].param2.x = 0.0f; // turn off
        }

        if (rand() % 20 + 1 > 5) {
            g_pLights->theLights[19].param2.x = 1.0f; // turn on
        } else {
            g_pLights->theLights[19].param2.x = 0.0f; // turn off
        }

        g_pLights->theLights[9].atten.y = 0.01f + (rand() / (float)RAND_MAX) * 0.12f; // linear
        g_pLights->theLights[9].atten.z = 0.001f + (rand() / (float)RAND_MAX) * 0.012f; // quadratic

        g_pLights->theLights[10].atten.y = 0.01f + (rand() / (float)RAND_MAX) * 0.12f; // linear
        g_pLights->theLights[10].atten.z = 0.001f + (rand() / (float)RAND_MAX) * 0.012f; // quadratic

        g_pLights->theLights[11].atten.y = 0.01f + (rand() / (float)RAND_MAX) * 0.12f; // linear
        g_pLights->theLights[11].atten.z = 0.001f + (rand() / (float)RAND_MAX) * 0.012f; // quadratic

        g_pLights->theLights[12].atten.y = 0.1f + (rand() / (float)RAND_MAX) * 0.12f; // linear
        g_pLights->theLights[12].atten.z = 0.01f + (rand() / (float)RAND_MAX) * 0.012f; // quadratic

        g_pLights->theLights[13].atten.y = 0.01f + (rand() / (float)RAND_MAX) * 0.12f; // linear
        g_pLights->theLights[13].atten.z = 0.001f + (rand() / (float)RAND_MAX) * 0.012f; // quadratic

        g_pLights->theLights[14].atten.y = 0.01f + (rand() / (float)RAND_MAX) * 0.12f; // linear
        g_pLights->theLights[14].atten.z = 0.001f + (rand() / (float)RAND_MAX) * 0.012f; // quadratic

        g_pLights->theLights[15].atten.y = 0.01f + (rand() / (float)RAND_MAX) * 0.12f; // linear
        g_pLights->theLights[15].atten.z = 0.001f + (rand() / (float)RAND_MAX) * 0.012f; // quadratic

        g_pLights->theLights[16].atten.y = 0.01f + (rand() / (float)RAND_MAX) * 0.12f; // linear
        g_pLights->theLights[16].atten.z = 0.001f + (rand() / (float)RAND_MAX) * 0.012f; // quadratic

        g_pLights->theLights[17].atten.y = 0.01f + (rand() / (float)RAND_MAX) * 0.12f; // linear
        g_pLights->theLights[17].atten.z = 0.001f + (rand() / (float)RAND_MAX) * 0.012f; // quadratic

        g_pLights->theLights[18].atten.y = 0.01f + (rand() / (float)RAND_MAX) * 0.12f; // linear
        g_pLights->theLights[18].atten.z = 0.001f + (rand() / (float)RAND_MAX) * 0.012f; // quadratic

        g_pLights->theLights[19].atten.y = 0.01f + (rand() / (float)RAND_MAX) * 0.12f; // linear
        g_pLights->theLights[19].atten.z = 0.001f + (rand() / (float)RAND_MAX) * 0.012f; // quadratic

        ::g_pSmoothSphere->meshFileName = "assets/models/Isoshphere_smooth_inverted_normals_xyz_n_rgba.ply";
        ::g_pSmoothSphere->bIsWireframe = true;
        ::g_pSmoothSphere->bOverrideVertexModelColour = true;
        ::g_pSmoothSphere->bIsVisible = lightDebug;
        ::g_pSmoothSphere->position = glm::vec3(
            ::g_pLights->theLights[::g_selectedLightIndex].position.x,
            ::g_pLights->theLights[::g_selectedLightIndex].position.y,
            ::g_pLights->theLights[::g_selectedLightIndex].position.z);


        if (!g_pMeshesToDraw.empty() && g_selectedObjectIndex < g_pMeshesToDraw.size()) {
            // Selected Mesh Indicator
            ::g_pSelectedMeshIndicator->meshFileName = ::g_pMeshesToDraw[::g_selectedObjectIndex]->meshFileName;
            ::g_pSelectedMeshIndicator->bIsWireframe = true;
            ::g_pSelectedMeshIndicator->bOverrideVertexModelColour = true;
            ::g_pSelectedMeshIndicator->bIsVisible = meshDebug;
            ::g_pSelectedMeshIndicator->scale = ::g_pMeshesToDraw[::g_selectedObjectIndex]->scale * 1.01f;
            ::g_pSelectedMeshIndicator->colourRGB = glm::vec4(RGBify(255, 165, 0.0), 1.0);
            ::g_pSelectedMeshIndicator->specularHighLightRGB = glm::vec4(RGBify(255, 165, 0.0), 1.0);
            ::g_pSelectedMeshIndicator->specularPower = 99999.0f;
            ::g_pSelectedMeshIndicator->position = ::g_pMeshesToDraw[::g_selectedObjectIndex]->position;
            ::g_pSelectedMeshIndicator->orientation = ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation;

            DrawMesh(g_pSelectedMeshIndicator, program);
        }

        cLightHelper lightHelper;

        const float errorValueforLightLevelGuess = 0.01f;
        const float infiniteDistance = 10000.0f;

        // where the light located
        ::g_pSmoothSphere->scale = 0.1f;
        ::g_pSmoothSphere->colourRGB = glm::vec4(1.0f);
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
        ::g_pSmoothSphere->colourRGB = glm::vec4(1.0f, 0.0f, 0.0f, 1.0);
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
        ::g_pSmoothSphere->colourRGB = glm::vec4(0.0f, 1.0f, 0.0f, 01.0f);
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

        ImGui::SetNextWindowSize(ImVec2(550, height));
        ImGui::SetNextWindowPos(ImVec2(width - 550, 0));
        if (usingGui) {
            ImGui::SetNextWindowBgAlpha(1);

        } else {
            ImGui::SetNextWindowBgAlpha(0.75);
        }
        ImGui::Begin("Simple Editor", nullptr,ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize);
        usingGui = ImGui::IsWindowHovered() || ImGui::IsAnyItemActive() || ImGui::IsAnyItemFocused();
        {
            static int selectedMeshIndex = 0;
            std::vector<std::string> currentVAOMeshs;
            for (const std::pair<const std::string, sModelDrawInfo>& pair : g_pMeshManager->GetMapOfMesh()) {
                currentVAOMeshs.push_back(pair.first);
            }

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Load")) {LoadScene();}
                    if (ImGui::MenuItem("Save")) {SaveScene();}
                    ImGui::EndMenu();
                }

                ImGui::Spacing();

                if (ImGui::BeginMenu("Lighting Mode")) {
                    if (ImGui::MenuItem("Lit")) {g_LightingType = 0;}
                    if (ImGui::MenuItem("Semi-Lit")) {g_LightingType = 1;}
                    if (ImGui::MenuItem("Unlit")) {g_LightingType = 2;}
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            if (ImGui::BeginTabBar("SceneEditor")) {
                // lighting tab
                if (ImGui::BeginTabItem("Lighting")) {
                    ImGui::Text("Edit Light");

                    static glm::vec4* directionRadians;
                    directionRadians = &g_pLights->theLights[g_selectedLightIndex].direction;
                    glm::vec4 directionDegrees = glm::degrees(*directionRadians);

                    static float* lightTypeValue;
                    lightTypeValue = &g_pLights->theLights[g_selectedLightIndex].param1.x;
                    int lightType = *lightTypeValue;

                    static float* lightStateValue;
                    lightStateValue = &g_pLights->theLights[g_selectedLightIndex].param2.x;
                    int lightState = *lightStateValue;

                    ImGui::InputFloat3("Edit Position",  glm::value_ptr(g_pLights->theLights[g_selectedLightIndex].position));
                    if ( lightType >= 1 &&
                        ImGui::SliderFloat3("Edit Direction", glm::value_ptr(directionDegrees), -180.0f, 180.0f)) {

                        *directionRadians = glm::radians(directionDegrees);
                    }

                    ImGui::InputFloat4("Edit attenuation", glm::value_ptr(g_pLights->theLights[g_selectedLightIndex].atten),"%.7f");


                    ImGui::ColorEdit4("Edit Diffuse", glm::value_ptr(g_pLights->theLights[g_selectedLightIndex].diffuse));
                    ImGui::ColorEdit4("Edit Specular", glm::value_ptr(g_pLights->theLights[g_selectedLightIndex].specular));

                    if (ImGui::SliderInt("Edit Light Type", &lightType, 0, 2)) {
                        g_pLights->theLights[g_selectedLightIndex].param1.x = lightType;
                    }
                    if (ImGui::IsItemHovered()){
                        ImGui::SetTooltip("0 = Point Light\n1 = Spot Light\n2 = Directional Light");
                    }

                    if (lightType == 1) {
                        if (ImGui::SliderFloat("Edit Spot Inner Angle", &g_pLights->theLights[g_selectedLightIndex].param1.y, 0.0f, 180.0f) &&
                            g_pLights->theLights[g_selectedLightIndex].param1.z < g_pLights->theLights[g_selectedLightIndex].param1.y) {

                            g_pLights->theLights[g_selectedLightIndex].param1.z = g_pLights->theLights[g_selectedLightIndex].param1.y;
                        }
                        if ( ImGui::SliderFloat("Edit Spot Outer Angle", &g_pLights->theLights[g_selectedLightIndex].param1.z, 0.0f, 180.0f) &&
                            g_pLights->theLights[g_selectedLightIndex].param1.y > g_pLights->theLights[g_selectedLightIndex].param1.z) {

                            g_pLights->theLights[g_selectedLightIndex].param1.y = g_pLights->theLights[g_selectedLightIndex].param1.z;
                        }
                    }

                    if (ImGui::SliderInt("Edit Light State", &lightState, 0, 1)) {
                        g_pLights->theLights[g_selectedLightIndex].param2.x = lightState;
                    }
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("0 = Off\n1 = On");
                    }

                    ImGui::Separator();

                    ImGui::EndTabItem();
                } // End lighting tab

                // Objects tab
                if (ImGui::BeginTabItem("Objects")) {

                    // Add object
                    if (!currentVAOMeshs.empty()) {
                        std::vector<const char*> meshNames;
                        for (std::string& mesh : currentVAOMeshs) {
                            meshNames.push_back(mesh.c_str());
                        }

                        ImGui::Text("Add Object");

                        static float addPosition[3] = {0.0f, 0.0f, 0.0f};
                        static float addRot[3] = {0.0f, 0.0f, 0.0f};
                        static float addScale = 1.0f;
                        static float addColour[3] = {1.0f, 1.0f, 1.0f};
                        static float addOpacity = 1.0f;
                        static float addSpecularHighlight[3] = {1.0f, 1.0f, 1.0f};
                        static float addSpecularPower = 1.0f;
                        static bool addOverrideColor = false;
                        static bool addWireframe = false;
                        static bool addVisible = true;

                        ImGui::InputFloat3("Position", addPosition);
                        ImGui::SliderFloat3("Orientation", addRot, -180.0f, 180.0f);
                        ImGui::InputFloat("Scale", &addScale);
                        if (addOverrideColor) {
                            ImGui::ColorEdit3("Colour RGB", addColour);
                        }
                        ImGui::SliderFloat("Opacity", &addOpacity, 0.0, 1.0);
                        ImGui::ColorEdit3("Specular Highlight", addSpecularHighlight);
                        ImGui::InputFloat("Specular Power", &addSpecularPower);
                        ImGui::Checkbox("Override Color", &addOverrideColor);
                        ImGui::Checkbox("Wireframe", &addWireframe);
                        ImGui::Checkbox("Visible", &addVisible);

                        if (ImGui::Combo("Mesh List", &selectedMeshIndex, meshNames.data(), meshNames.size()));
                        if (selectedMeshIndex >= 0 && selectedMeshIndex < currentVAOMeshs.size()) {
                            if (ImGui::Button("Add")) {

                                cMeshObject* pNewObject = new cMeshObject();
                                pNewObject->meshFileName = currentVAOMeshs[selectedMeshIndex];
                                pNewObject->position = glm::vec3(addPosition[0], addPosition[1], addPosition[2]);
                                pNewObject->orientation = glm::radians(glm::vec3(addRot[0], addRot[1], addRot[2]));
                                pNewObject->scale = addScale;
                                pNewObject->colourRGB = glm::vec3(addColour[0], addColour[1], addColour[2]);
                                pNewObject->opacityAlpha = addOpacity;
                                pNewObject->specularHighLightRGB = glm::vec3(addSpecularHighlight[0], addSpecularHighlight[1], addSpecularHighlight[2]);
                                pNewObject->specularPower = addSpecularPower;
                                pNewObject->bOverrideVertexModelColour = addOverrideColor;
                                pNewObject->bIsWireframe = addWireframe;
                                pNewObject->bIsVisible = addVisible;

                                ::g_pMeshesToDraw.push_back(pNewObject);

                                // Rest form
                                addPosition[0] = addPosition[1] = addPosition[2] = 0.0f;
                                addRot[0] = addRot[1] = addRot[2] = 0.0f;
                                addScale = 1.0f;
                                addColour[0] = addColour[1] = addColour[2] = 1.0f;
                                addOpacity = 1.0f;
                                addSpecularHighlight[0] = addSpecularHighlight[1] = addSpecularHighlight[2] = 1.0f;
                                addSpecularPower = 1.0f;
                                addOverrideColor = false;
                                addWireframe = false;
                                addVisible = true;

                                g_selectedObjectIndex = g_pMeshesToDraw.size() - 1;
                            }
                        }
                    }

                    ImGui::Separator();
                    ImGui::Text("Edit Object");

                    if (!g_pMeshesToDraw.empty() && g_selectedObjectIndex < g_pMeshesToDraw.size()) {
                        static glm::vec3* orientationRadians;
                        orientationRadians = &g_pMeshesToDraw[g_selectedObjectIndex]->orientation;
                        glm::vec3 orientationDegrees = glm::degrees(*orientationRadians);

                        ImGui::InputFloat3("Edit Position", glm::value_ptr(g_pMeshesToDraw[g_selectedObjectIndex]->position));
                        if (ImGui::SliderFloat3("Edit Orientation", glm::value_ptr(orientationDegrees), -180.0f, 180.0f)) {
                            *orientationRadians = glm::radians(orientationDegrees);
                        }

                        ImGui::InputFloat("Edit Scale", &g_pMeshesToDraw[g_selectedObjectIndex]->scale);

                        if (g_pMeshesToDraw[g_selectedObjectIndex]->bOverrideVertexModelColour) {
                            ImGui::ColorEdit4("Edit Colour", glm::value_ptr(g_pMeshesToDraw[g_selectedObjectIndex]->colourRGB));
                        }

                        ImGui::SliderFloat("Edit Opacity", &g_pMeshesToDraw[g_selectedObjectIndex]->opacityAlpha,0.0f, 1.0f);
                        ImGui::ColorEdit3("Edit Specular Highlight", glm::value_ptr(g_pMeshesToDraw[g_selectedObjectIndex]->specularHighLightRGB));
                        ImGui::InputFloat("Edit Specular Power", &g_pMeshesToDraw[g_selectedObjectIndex]->specularPower);
                        ImGui::Checkbox("Edit Override Color", &g_pMeshesToDraw[g_selectedObjectIndex]->bOverrideVertexModelColour);
                        ImGui::Checkbox("Edit Wireframe", &g_pMeshesToDraw[g_selectedObjectIndex]->bIsWireframe);
                        ImGui::Checkbox("Edit Visible", &g_pMeshesToDraw[g_selectedObjectIndex]->bIsVisible);
                    }
                    ImGui::EndTabItem();
                } // End object tab
                ImGui::EndTabBar();
            } //End tab bar

            ImGuiIO& io = ImGui::GetIO();

            ImGui::Separator();

            if (ImGui::ArrowButton("PreviousObject", ImGuiDir_Left)) {
                if (g_selectedObjectIndex > 0) {
                    g_selectedObjectIndex--;
                } else {
                    g_selectedObjectIndex = g_pMeshesToDraw.size() - 1;
                }
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            ImGui::SliderInt("Current Object", &g_selectedObjectIndex,0, g_pMeshesToDraw.size() - 1);
            ImGui::SameLine();
            if (ImGui::ArrowButton("NextObject", ImGuiDir_Right)) {
                if (g_selectedObjectIndex < g_pMeshesToDraw.size() - 1) {
                    g_selectedObjectIndex++;
                } else {
                    g_selectedObjectIndex = 0;
                }
            }

            if (ImGui::ArrowButton("PreviousLight", ImGuiDir_Left)) {
                if (g_selectedLightIndex > 0) {
                    g_selectedLightIndex--;
                } else {
                    g_selectedLightIndex = g_pLights->NUMBEROFLIGHTS - 1;
                }
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            ImGui::SliderInt("Current Light", &g_selectedLightIndex, 0, g_pLights->NUMBEROFLIGHTS - 1);
            ImGui::SameLine();
            if (ImGui::ArrowButton("NextLight", ImGuiDir_Right)) {
                if (g_selectedLightIndex < g_pLights->NUMBEROFLIGHTS - 1) {
                    g_selectedLightIndex++;
                } else {
                    g_selectedLightIndex = 0;
                }
            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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

    // Copy over the transparency
    // uniform float alphaTransparency;
    GLint alphaTransparency_UL
        = glGetUniformLocation(program, "alphaTransparency");
    // Set it
    glUniform1f(alphaTransparency_UL, pCurrentMesh->opacityAlpha);

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
