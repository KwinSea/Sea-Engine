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
#include "DrawMesh.h"
#include "LoadModelsAndTextures.h"

// ImGui
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

// Textures
#include "ScriptHelper.h"
#include "cBasicTextureManager/cBasicTextureManager.h"


cShaderManager* g_pTheShaderManager = NULL;
cLightManager* g_pLights = NULL;

cScript scriptInstance;

// Textures
cBasicTextureManager* g_pTheTextures = NULL;

cMeshObject* g_pSmoothSphere = NULL;
cMeshObject* g_pSelectedMeshIndicator = NULL;

extern cVAOManager* pTheMeshManager;
extern std::vector<cScript> scriptObjects;

extern int g_selectedLightIndex;
extern int g_selectedObjectIndex;

extern float object_move_speed;
extern float object_rotate_speed;
extern float light_move_speed;

double deltaTime;

extern bool lightDebug;
extern bool meshDebug;

extern bool isObjectGridSnap;
extern bool isLightGridSnap;

extern float object_move_grid;
extern float light_move_grid;

unsigned int screenWidth = 1280;
unsigned int screenHeight = 720;

int g_LightingType = 0;
unsigned int g_NumVerticiesToDraw = 0;
unsigned int g_SizeOfVertexArrayInBytes = 0;

bool objectHovered = false;
int g_hoveredObjectIndex = 0;

bool usingGui = false;

Camera camera (screenWidth, screenHeight, glm::vec3(5000.0f, 2000.0f, 5000.0f));

// void LoadFilesIntoVAOManager(GLuint program);

std::vector<cMeshObject*> g_pMeshesToDraw;


static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void UpdateSceneScripts(std::vector<cMeshObject*>& meshObjects,  float deltaTime) {
    for (cMeshObject* meshObject : meshObjects){
        meshObject->UpdateScripts(deltaTime);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main(void) {
    GLFWwindow* window;

    GLuint program;
    GLint mvp_location;

    double oldTimeSinceStart = 0;

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

    ::g_pTheTextures = new cBasicTextureManager();
    LoadTexturesIntoTextureManager(::g_pTheTextures);

    LoadScripts();

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

    // scriptInstance.CreateScript("Test1");
    // scriptInstance.CreateScript("Test2");
    // scriptInstance.CreateScript("Test3");

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        double timeSinceStart = glfwGetTime();
        deltaTime = timeSinceStart - oldTimeSinceStart;
        oldTimeSinceStart = timeSinceStart;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!usingGui) {
            camera.InputHandler(window);
        }
        camera.Matrix(0.6f, 0.1f, 1000000.0f, *g_pTheShaderManager, "camMatrix");

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

            int prev = index - 1;
            while (prev >= 0 && glm::distance(camera.Position, vecTransparentThings[prev]->position) < distToCamera) {
                vecTransparentThings[prev + 1] = vecTransparentThings[prev];
                --prev;
            }
            vecTransparentThings[prev + 1] = pCurrentMesh;
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

        /*TODO MOVE TORCHES AROUND SCENE*/

        // Torch Flicker
        static float flickFrame = 0.0f;
        flickFrame++ * deltaTime;
        if (flickFrame  > g_getRandBetween(1.0f, 10.0f)) {
            g_pLights->theLights[11].atten.y = g_getRandBetween(0.001f, 0.0012f); // linear
            g_pLights->theLights[11].atten.z = g_getRandBetween(0.0001f, 0.00005f);// quadratic

            g_pLights->theLights[12].atten.y = g_getRandBetween(0.001f, 0.0012f); // linear
            g_pLights->theLights[12].atten.z = g_getRandBetween(0.0001f, 0.00005f); // quadratic

            g_pLights->theLights[13].atten.y = g_getRandBetween(0.001f, 0.0012f); // linear
            g_pLights->theLights[13].atten.z = g_getRandBetween(0.0001f, 0.00005f); // quadratic

            g_pLights->theLights[14].atten.y = g_getRandBetween(0.001f, 0.0012f); // linear
            g_pLights->theLights[14].atten.z = g_getRandBetween(0.0001f, 0.00005f); // quadratic

            g_pLights->theLights[15].atten.y = g_getRandBetween(0.001f, 0.0012f); // linear
            g_pLights->theLights[15].atten.z = g_getRandBetween(0.0001f, 0.00005f); // quadratic

            g_pLights->theLights[16].atten.y = g_getRandBetween(0.001f, 0.0012f); // linear
            g_pLights->theLights[16].atten.z = g_getRandBetween(0.0001f, 0.00005f); // quadratic

            g_pLights->theLights[17].atten.y = g_getRandBetween(0.001f, 0.0012f); // linear
            g_pLights->theLights[17].atten.z = g_getRandBetween(0.0001f, 0.00005f); // quadratic

            g_pLights->theLights[18].atten.y = g_getRandBetween(0.001f, 0.0012f); // linear
            g_pLights->theLights[18].atten.z = g_getRandBetween(0.0001f, 0.00005f); // quadratic
            flickFrame = 0.0f;
        }


        ::g_pSmoothSphere->meshFileName = "assets/models/Isoshphere_smooth_inverted_normals_xyz_n_rgba.ply";
        ::g_pSmoothSphere->bIsWireframe = true;
        ::g_pSmoothSphere->bOverrideVertexModelColour = true;
        ::g_pSmoothSphere->bDontUseTextures = true;
        ::g_pSmoothSphere->bDoNotLight = true;
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
            ::g_pSelectedMeshIndicator->bDontUseTextures = true;
            ::g_pSelectedMeshIndicator->bDoNotLight = true;
            ::g_pSelectedMeshIndicator->bIsVisible = meshDebug;
            ::g_pSelectedMeshIndicator->scale = ::g_pMeshesToDraw[::g_selectedObjectIndex]->scale * 1.01f;
            ::g_pSelectedMeshIndicator->colourRGB = glm::vec4(RGBify(255, 165, 0.0), 1.0);
            ::g_pSelectedMeshIndicator->position = ::g_pMeshesToDraw[::g_selectedObjectIndex]->position;
            ::g_pSelectedMeshIndicator->orientation = ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation;

            DrawMesh(g_pSelectedMeshIndicator, program);
        }

        if (!g_pMeshesToDraw.empty() && g_hoveredObjectIndex < g_pMeshesToDraw.size()) {
            // Selected Mesh Indicator
            ::g_pSelectedMeshIndicator->meshFileName = ::g_pMeshesToDraw[::g_hoveredObjectIndex]->meshFileName;
            ::g_pSelectedMeshIndicator->bIsWireframe = true;
            ::g_pSelectedMeshIndicator->bOverrideVertexModelColour = true;
            ::g_pSelectedMeshIndicator->bDontUseTextures = true;
            ::g_pSelectedMeshIndicator->bDoNotLight = true;
            ::g_pSelectedMeshIndicator->bIsVisible = objectHovered;
            ::g_pSelectedMeshIndicator->scale = ::g_pMeshesToDraw[::g_hoveredObjectIndex]->scale * 1.01f;
            ::g_pSelectedMeshIndicator->colourRGB = glm::vec4(RGBify(0, 255, 255), 1.0);
            ::g_pSelectedMeshIndicator->position = ::g_pMeshesToDraw[::g_hoveredObjectIndex]->position;
            ::g_pSelectedMeshIndicator->orientation = ::g_pMeshesToDraw[::g_hoveredObjectIndex]->orientation;

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

        cMeshObject* pSkyBox = g_pFindObjectByUniqueName("skybox_mesh");
        GLint bIsSkyboxObject_UL = glGetUniformLocation(program, "bIsSkyboxObject");
        glUniform1f(bIsSkyboxObject_UL, (GLfloat)GL_TRUE);  // Or 1.0f

        if (pSkyBox != NULL)
        {
            pSkyBox->bIsVisible = true;

            // Move this mesh to where the camera is
            pSkyBox->position = camera.Position;

            // The skybox textue likely won't change, so we are setting it once at the start
            //            GLuint skyBoxTexture_ID = ::g_pTheTextures->getTextureIDFromName("SunnyDay");
            GLuint skyBoxTexture_ID = ::g_pTheTextures->getTextureIDFromName("Space");

            // Chose a unique texture unit. Here I pick 20 just because...
            glActiveTexture(GL_TEXTURE20);
            // Note this ISN'T GL_TEXTURE_2D
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTexture_ID);

            // uniform samplerCube skyboxCubeTexture;
            GLint skyboxCubeTexture_UL = glGetUniformLocation(program, "skyboxCubeTexture");
            glUniform1i(skyboxCubeTexture_UL, 20);   // (Uniform ID, Texture Unit #)

            DrawMesh(pSkyBox, program);


            pSkyBox->bIsVisible = false;

        }//if (pSkyBox != NULL)

        glUniform1f(bIsSkyboxObject_UL, (GLfloat)GL_FALSE);

        std::stringstream ssWindowTitle;

        ssWindowTitle << "Session info [ Camera Speed: " << camera.speed << " | Object Speed: " << object_move_speed << " | Rotation Speed: " << object_rotate_speed << " | Light Speed: " << light_move_speed << " ]   [ Camera X:" << camera.Position.x << " | Camera Y:" << camera.Position.y << " | Camera Z:" << camera.Position.z << " ]"<< std::endl;

        UpdateSceneScripts(g_pMeshesToDraw, deltaTime);

        glfwSetWindowTitle(window, ssWindowTitle.str().c_str());

        ImGui::SetNextWindowSize(ImVec2(550, height));
        ImGui::SetNextWindowPos(ImVec2(width - 550, 0));
        if (usingGui) {
            ImGui::SetNextWindowBgAlpha(1);

        } else {
            ImGui::SetNextWindowBgAlpha(0.75);
        }
        ImGui::Begin("Simple Editor", nullptr,ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize);
        usingGui = ImGui::IsWindowHovered() || ImGui::IsAnyItemActive() || ImGui::IsAnyItemFocused() || ImGui::IsAnyItemHovered();
        {
            static int selectedMeshIndex = 0;
            std::vector<std::string> currentVAOMeshs;
            for (const std::pair<const std::string, sModelDrawInfo>& pair : pTheMeshManager->GetMapOfMesh()) {
                currentVAOMeshs.push_back(pair.first);
            }

            std::vector<std::string> loadedTextures = g_pTheTextures->GetLoadedTextures();
            std::vector<const char*> currentVAOTextures;
            for (const std::string& texture : loadedTextures) {
                currentVAOTextures.push_back(texture.c_str());
            }

            static bool openScripPopUp = false;
            static bool openObjectPopUp = false;

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Edit")) {
                    if (ImGui::MenuItem("New Script")) {openScripPopUp = true;}
                    if (ImGui::MenuItem("New Object")) {openObjectPopUp = true;}
                    ImGui::EndMenu();
                }

                ImGui::Spacing();

                if (ImGui::BeginMenu("Scene")) {
                    if (ImGui::MenuItem("Clear Scene")) {ClearScene();}
                    if (ImGui::MenuItem("Load Scene")) {LoadScene();}
                    if (ImGui::MenuItem("Save Scene")) {SaveScene();}
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

            if (openScripPopUp) {
                ImGui::OpenPopup("##CreatingScript");
                openScripPopUp = false;
            } else if (openObjectPopUp) {
                ImGui::OpenPopup("##CreatingObject");
                openObjectPopUp = false;
            }

            ImVec2 screenCenter = ImGui::GetMainViewport()->GetCenter();

            // New Scrip Popup
            ImGui::SetNextWindowPos(screenCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("##CreatingScript", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                static char newScriptName[128] = "";
                ImGui::InputText("Script Name", newScriptName, sizeof(newScriptName));
                if (ImGui::Button("Create Script")) {
                    scriptInstance.CreateScript(newScriptName);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            // New Object Popup
            ImGui::SetNextWindowPos(screenCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("##CreatingObject", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
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
                    static float addReflectionStrength = 1.0f;
                    static float addRefractionStrength = 1.0f;
                    static bool addOverrideColor = false;
                    static bool addWireframe = false;
                    static bool addVisible = true;
                    static bool addDoNotLight = false;
                    static bool addDontUseTextures = false;
                    static bool addUseMaskingTexture = false;
                    static bool addReflect = false;
                    static bool addRefract = false;

                    static int addSelectedTexIndex_01 = 0;
                    static int addSelectedTexIndex_02 = 0;
                    static int addSelectedTexIndex_03 = 0;
                    static int addSelectedTexIndex_04 = 0;

                    static float addTexIndexMix[4] = {1.0f, 0.0f, 0.0f, 0.0f};

                    ImGui::InputFloat3("Position", addPosition);
                    ImGui::SliderFloat3("Orientation", addRot, -180.0f, 180.0f);
                    ImGui::InputFloat("Scale", &addScale);
                    ImGui::NewLine();

                    if (addOverrideColor) {
                        ImGui::ColorEdit3("Colour RGB", addColour);
                    }
                    ImGui::SliderFloat("Opacity", &addOpacity, 0.0, 1.0);
                    ImGui::NewLine();

                    ImGui::ColorEdit3("Specular Highlight", addSpecularHighlight);
                    ImGui::InputFloat("Specular Power", &addSpecularPower);
                    ImGui::NewLine();

                    ImGui::Checkbox("Override Color", &addOverrideColor);
                    ImGui::Checkbox("Wireframe", &addWireframe);
                    ImGui::NewLine();

                    ImGui::Checkbox("Dont Light", &addDoNotLight);
                    ImGui::NewLine();

                    ImGui::Checkbox("Add Reflect", &addReflect);
                    if (addReflect) {
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(150);
                        ImGui::SliderFloat("Add Reflect Strength", &addReflectionStrength, 0.0f, 1.0f);
                    }

                    ImGui::Checkbox("Add Refract", &addRefract);
                    if (addRefract) {
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(150);
                        ImGui::SliderFloat("Add Refract Strength", &addRefractionStrength, 0.0f, 1.0f);
                    }
                    ImGui::Checkbox("Dont Use Textures", &addDontUseTextures);
                    ImGui::Checkbox("Use Masking Texture", &addUseMaskingTexture);

                    if (!addDontUseTextures) {
                        if (ImGui::BeginCombo("New Texture 1 List", currentVAOTextures[addSelectedTexIndex_01])) {
                            for (int i = 0; i < currentVAOTextures.size(); i++) {
                                bool isSelected = addSelectedTexIndex_01 == i;

                                if (ImGui::Selectable(currentVAOTextures[i], isSelected)) {
                                    addSelectedTexIndex_01 = i;
                                }

                                // Texture preview when hovering
                                if (ImGui::IsItemHovered()) {
                                    ImGui::BeginTooltip();
                                    ImGui::Image((void*)(size_t)g_pTheTextures->getTextureIDFromName(currentVAOTextures[i]),
                                                 ImVec2(100, 100));
                                    ImGui::EndTooltip();
                                }

                                if (isSelected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }

                        if (ImGui::BeginCombo("New Texture 2 List", currentVAOTextures[addSelectedTexIndex_02])) {
                            for (int i = 0; i < currentVAOTextures.size(); i++) {
                                bool isSelected = addSelectedTexIndex_02 == i;

                                if (ImGui::Selectable(currentVAOTextures[i], isSelected)) {
                                    addSelectedTexIndex_02 = i;
                                }

                                // Texture preview when hovering
                                if (ImGui::IsItemHovered()) {
                                    ImGui::BeginTooltip();
                                    ImGui::Image((void*)(size_t)g_pTheTextures->getTextureIDFromName(currentVAOTextures[i]),
                                                 ImVec2(100, 100));
                                    ImGui::EndTooltip();
                                }

                                if (isSelected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }

                        if (ImGui::BeginCombo("New Texture 3 List", currentVAOTextures[addSelectedTexIndex_03])) {
                            for (int i = 0; i < currentVAOTextures.size(); i++) {
                                bool isSelected = addSelectedTexIndex_03 == i;

                                if (ImGui::Selectable(currentVAOTextures[i], isSelected)) {
                                    addSelectedTexIndex_03 = i;
                                }

                                // Texture preview when hovering
                                if (ImGui::IsItemHovered()) {
                                    ImGui::BeginTooltip();
                                    ImGui::Image((void*)(size_t)g_pTheTextures->getTextureIDFromName(currentVAOTextures[i]),
                                                 ImVec2(100, 100));
                                    ImGui::EndTooltip();
                                }

                                if (isSelected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }

                        if (ImGui::BeginCombo("New Texture 4 List", currentVAOTextures[addSelectedTexIndex_04])) {
                            for (int i = 0; i < currentVAOTextures.size(); i++) {
                                bool isSelected = addSelectedTexIndex_04 == i;

                                if (ImGui::Selectable(currentVAOTextures[i], isSelected)) {
                                    addSelectedTexIndex_04 = i;
                                }

                                // Texture preview when hovering
                                if (ImGui::IsItemHovered()) {
                                    ImGui::BeginTooltip();
                                    ImGui::Image((void*)(size_t)g_pTheTextures->getTextureIDFromName(currentVAOTextures[i]),
                                                 ImVec2(100, 100));
                                    ImGui::EndTooltip();
                                }

                                if (isSelected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }

                        ImGui::SliderFloat4("New Texture Mix", addTexIndexMix, 0.0f, 1.0f);
                    }
                    ImGui::NewLine();

                    if (ImGui::Combo("New Mesh List", &selectedMeshIndex, meshNames.data(), meshNames.size()));
                    ImGui::Checkbox("Visible", &addVisible);
                    ImGui::NewLine();

                    if (selectedMeshIndex >= 0 && selectedMeshIndex < currentVAOMeshs.size()) {
                        if (ImGui::Button("Add")) {

                            cMeshObject* pNewObject = new cMeshObject();
                            pNewObject->meshFileName = currentVAOMeshs[selectedMeshIndex];

                            pNewObject->textureNames[0] = loadedTextures[addSelectedTexIndex_01];
                            pNewObject->textureNames[1] = loadedTextures[addSelectedTexIndex_02];
                            pNewObject->textureNames[2] = loadedTextures[addSelectedTexIndex_03];
                            pNewObject->textureNames[3] = loadedTextures[addSelectedTexIndex_04];

                            for (int i = 0; i < 4; i++) {
                                pNewObject->textureMixRatio[i] = addTexIndexMix[i];
                            }

                            pNewObject->position = glm::vec3(addPosition[0], addPosition[1], addPosition[2]);
                            pNewObject->orientation = glm::vec3(addRot[0], addRot[1], addRot[2]);
                            pNewObject->scale = addScale;

                            pNewObject->colourRGB = glm::vec3(addColour[0], addColour[1], addColour[2]);
                            pNewObject->opacityAlpha = addOpacity;

                            pNewObject->specularHighLightRGB = glm::vec3(addSpecularHighlight[0], addSpecularHighlight[1], addSpecularHighlight[2]);
                            pNewObject->specularPower = addSpecularPower;
                            pNewObject->reflectionStrength = addReflectionStrength;
                            pNewObject->refractionStrength = addRefractionStrength;

                            pNewObject->bOverrideVertexModelColour = addOverrideColor;
                            pNewObject->bIsWireframe = addWireframe;
                            pNewObject->bIsVisible = addVisible;

                            pNewObject->bDoNotLight = addDoNotLight;
                            pNewObject->bDontUseTextures = addDontUseTextures;
                            pNewObject->bUseMaskingTexture = addUseMaskingTexture;
                            pNewObject->bAddReflect = addReflect;
                            pNewObject->bAddRefract = addRefract;

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
                            addDoNotLight = false;
                            addDontUseTextures = false;

                            g_selectedObjectIndex = g_pMeshesToDraw.size() - 1;

                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::SameLine();
                        ImGui::Spacing();
                        ImGui::SameLine();

                        if (ImGui::Button("Cancel")) {
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }
                ImGui::EndPopup();
            }

            ImGui::Text("Scene Objects");
            bool anyObjectListHovered = false;
            ImGui::BeginChild("ObjectList", ImVec2(0, height / 2), true, ImGuiWindowFlags_HorizontalScrollbar);
            for (size_t i = 0; i < g_pMeshesToDraw.size(); ++i) {
                std::string label;

                if (g_pMeshesToDraw[i]->uniqueName != "N/A" && !g_pMeshesToDraw[i]->uniqueName.empty()) {
                    label = g_pMeshesToDraw[i]->uniqueName;
                } else if (!g_pMeshesToDraw[i]->meshFileName.empty()) {
                    label = g_pMeshesToDraw[i]->meshFileName + std::to_string(i);
                } else {
                    label = "N/A" + std::to_string(i);
                }

                if (ImGui::Selectable(label.c_str(), g_selectedObjectIndex == i)) {
                    g_selectedObjectIndex = (int)i;
                }

                if (ImGui::IsItemHovered() && meshDebug) {
                    g_hoveredObjectIndex = (int)i;
                    anyObjectListHovered = true;
                }
            }
            objectHovered = anyObjectListHovered;
            ImGui::EndChild();
            ImGui::NewLine();

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
                    ImGui::NewLine();

                    ImGui::Checkbox("Script", &g_pMeshesToDraw[g_selectedObjectIndex]->bAddReflect);
                    ImGui::EndTabItem();
                } // End lighting tab

                // Objects tab
                if (ImGui::BeginTabItem("Objects")) {
                    ImGui::NewLine();
                    ImGui::Text("Edit Object");

                    if (!g_pMeshesToDraw.empty() && g_selectedObjectIndex < g_pMeshesToDraw.size()) {

                        ImGui::InputFloat3("Edit Position", glm::value_ptr(g_pMeshesToDraw[g_selectedObjectIndex]->position));
                        ImGui::SliderFloat3("Edit Orientation", glm::value_ptr(g_pMeshesToDraw[g_selectedObjectIndex]->orientation), -180.0f, 180.0f);


                        ImGui::InputFloat("Edit Scale", &g_pMeshesToDraw[g_selectedObjectIndex]->scale);
                        ImGui::NewLine();

                        if (g_pMeshesToDraw[g_selectedObjectIndex]->bOverrideVertexModelColour) {
                            ImGui::ColorEdit4("Edit Colour", glm::value_ptr(g_pMeshesToDraw[g_selectedObjectIndex]->colourRGB));
                        }

                        ImGui::SliderFloat("Edit Opacity", &g_pMeshesToDraw[g_selectedObjectIndex]->opacityAlpha,0.0f, 1.0f);
                        ImGui::NewLine();

                        ImGui::ColorEdit3("Edit Specular Highlight", glm::value_ptr(g_pMeshesToDraw[g_selectedObjectIndex]->specularHighLightRGB));
                        ImGui::InputFloat("Edit Specular Power", &g_pMeshesToDraw[g_selectedObjectIndex]->specularPower);
                        ImGui::NewLine();

                        ImGui::Checkbox("Edit Override Color", &g_pMeshesToDraw[g_selectedObjectIndex]->bOverrideVertexModelColour);
                        ImGui::Checkbox("Edit Wireframe", &g_pMeshesToDraw[g_selectedObjectIndex]->bIsWireframe);
                        ImGui::NewLine();

                        ImGui::Checkbox("Light", &g_pMeshesToDraw[g_selectedObjectIndex]->bDoNotLight);
                        ImGui::NewLine();

                        ImGui::Checkbox("Reflect", &g_pMeshesToDraw[g_selectedObjectIndex]->bAddReflect);
                        if (g_pMeshesToDraw[g_selectedObjectIndex]->bAddReflect) {
                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(150);
                            ImGui::SliderFloat("Reflect Strength", &g_pMeshesToDraw[g_selectedObjectIndex]->reflectionStrength, 0.0f, 1.0f);
                        }

                        ImGui::Checkbox("Refract", &g_pMeshesToDraw[g_selectedObjectIndex]->bAddRefract);
                        if (g_pMeshesToDraw[g_selectedObjectIndex]->bAddRefract) {
                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(150);
                            ImGui::SliderFloat("Refract Strength", &g_pMeshesToDraw[g_selectedObjectIndex]->refractionStrength, 0.0f, 1.0f);
                        }
                        ImGui::NewLine();

                        ImGui::Checkbox("Edit Dont Use Textures", &g_pMeshesToDraw[g_selectedObjectIndex]->bDontUseTextures);
                        ImGui::Checkbox("Edit Use Masking Texture", &g_pMeshesToDraw[g_selectedObjectIndex]->bUseMaskingTexture);

                        // Find Textue index based off currentVAOTextures
                        static int textureIndex[4];
                        for (int i = 0; i < currentVAOTextures.size(); i++) {
                            for (int j = 0; j < 4; j++) {
                                if (g_pMeshesToDraw[g_selectedObjectIndex]->textureNames[j] == currentVAOTextures[i]) {
                                    textureIndex[j] = i;
                                    break;
                                }
                            }
                        }

                        if (!g_pMeshesToDraw[g_selectedObjectIndex]->bDontUseTextures) {
                            if (ImGui::BeginCombo("Texture 1", currentVAOTextures[textureIndex[0]])) {
                                for (int i = 0; i < currentVAOTextures.size(); i++) {
                                    bool isSelected = textureIndex[0] == i;

                                    if (ImGui::Selectable(currentVAOTextures[i], isSelected)) {
                                        textureIndex[0] = i;
                                        g_pMeshesToDraw[g_selectedObjectIndex]->textureNames[0] = currentVAOTextures[i];
                                    }

                                    // Texture preview when hovering
                                    if (ImGui::IsItemHovered()) {
                                        ImGui::BeginTooltip();
                                        ImGui::Image((void*)(size_t)g_pTheTextures->getTextureIDFromName(currentVAOTextures[i]),
                                                     ImVec2(100, 100));
                                        ImGui::EndTooltip();
                                    }

                                    if (isSelected) {
                                        ImGui::SetItemDefaultFocus();
                                    }
                                }
                                ImGui::EndCombo();
                            }

                            if (ImGui::BeginCombo("Texture 2", currentVAOTextures[textureIndex[1]])) {
                                for (int i = 0; i < currentVAOTextures.size(); i++) {
                                    bool isSelected = textureIndex[1] == i;

                                    if (ImGui::Selectable(currentVAOTextures[i], isSelected)) {
                                        textureIndex[1] = i;
                                        g_pMeshesToDraw[g_selectedObjectIndex]->textureNames[1] = currentVAOTextures[i];
                                    }

                                    // Texture preview when hovering
                                    if (ImGui::IsItemHovered()) {
                                        ImGui::BeginTooltip();
                                        ImGui::Image((void*)(size_t)g_pTheTextures->getTextureIDFromName(currentVAOTextures[i]),
                                                     ImVec2(100, 100));
                                        ImGui::EndTooltip();
                                    }

                                    if (isSelected) {
                                        ImGui::SetItemDefaultFocus();
                                    }
                                }
                                ImGui::EndCombo();
                            }

                            if (ImGui::BeginCombo("Texture 3", currentVAOTextures[textureIndex[2]])) {
                                for (int i = 0; i < currentVAOTextures.size(); i++) {
                                    bool isSelected = textureIndex[2] == i;

                                    if (ImGui::Selectable(currentVAOTextures[i], isSelected)) {
                                        textureIndex[2] = i;
                                        g_pMeshesToDraw[g_selectedObjectIndex]->textureNames[2] = currentVAOTextures[i];
                                    }

                                    // Texture preview when hovering
                                    if (ImGui::IsItemHovered()) {
                                        ImGui::BeginTooltip();
                                        ImGui::Image((void*)(size_t)g_pTheTextures->getTextureIDFromName(currentVAOTextures[i]),
                                                     ImVec2(100, 100));
                                        ImGui::EndTooltip();
                                    }

                                    if (isSelected) {
                                        ImGui::SetItemDefaultFocus();
                                    }
                                }
                                ImGui::EndCombo();
                            }

                            if (ImGui::BeginCombo("Texture 4", currentVAOTextures[textureIndex[3]])) {
                                for (int i = 0; i < currentVAOTextures.size(); i++) {
                                    bool isSelected = textureIndex[3] == i;

                                    if (ImGui::Selectable(currentVAOTextures[i], isSelected)) {
                                        textureIndex[3] = i;
                                        g_pMeshesToDraw[g_selectedObjectIndex]->textureNames[3] = currentVAOTextures[i];
                                    }

                                    // Texture preview when hovering
                                    if (ImGui::IsItemHovered()) {
                                        ImGui::BeginTooltip();
                                        ImGui::Image((void*)(size_t)g_pTheTextures->getTextureIDFromName(currentVAOTextures[i]),
                                                     ImVec2(100, 100));
                                        ImGui::EndTooltip();
                                    }

                                    if (isSelected) {
                                        ImGui::SetItemDefaultFocus();
                                    }
                                }
                                ImGui::EndCombo();
                            }

                            ImGui::SliderFloat4("Texture Mix", g_pMeshesToDraw[g_selectedObjectIndex]->textureMixRatio, 0.0f, 1.0f);
                        }
                    }
                    ImGui::EndTabItem();
                } // End object tab
                ImGui::EndTabBar();
            } //End tab bar

            ImGuiIO& io = ImGui::GetIO();

            ImGui::Separator();
            ImGui::NewLine();

            ImGui::Checkbox("Object Grid Snap", &isObjectGridSnap);
            if (isObjectGridSnap) {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(75);
                ImGui::InputFloat("##ObjectGridVal", &object_move_grid);
            } else {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(75);
                ImGui::InputFloat("Object Move Speed", &object_move_speed);
            }

            ImGui::Checkbox("Light Grid Snap", &isLightGridSnap);
            if (isLightGridSnap) {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(75);
                ImGui::InputFloat("##LightGridVal", &light_move_grid);
            } else {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(75);
                ImGui::InputFloat("Light Move Speed", &light_move_speed);
            }

            ImGui::NewLine();

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
            ImGui::NewLine();

            ImGui::SetNextItemWidth(75);
            ImGui::InputFloat("Camera Speed", &camera.speed);
            ImGui::SetNextItemWidth(75);
            ImGui::InputFloat("Camera Sensitivity", &camera.sensitivity);

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