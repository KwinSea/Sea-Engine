#include "GlobalOpenGL.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>

#include "Camera.h"
#include "cMeshObject.h"
#include "cLightManager.h"
#include "SceneDirector.h"

extern Camera camera;
extern std::vector<cMeshObject*> g_pMeshesToDraw;
extern cLightManager* g_pLights;
extern unsigned int g_LightingType;
extern unsigned int screenWidth;
extern unsigned int screenHeight;

unsigned int g_selectedObjectIndex = 0;
unsigned int g_selectedLightIndex = 0;
bool lightDebug = false;
bool meshDebug = false;

bool isShiftDown(int mods) {
    if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT) {
        return true;
    }

    return false;
}

bool isCtlDown(int mods) {
    if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL) {
        return true;
    }

    return false;
}

bool isAltDown(int mods) {
    if ((mods & GLFW_MOD_ALT) == GLFW_MOD_ALT) {
        return true;
    }

    return false;
}

bool areAnyModifiersDown(int mods) {
    if (isShiftDown(mods) || isCtlDown(mods) || isAltDown(mods)) {
        return true;
    }

    return false;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    const float object_move_speed = 0.7f;

    if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL) {
        if (key == GLFW_KEY_A) {
            ::g_pLights->theLights[::g_selectedLightIndex].position.x += object_move_speed;
        }

        if (key == GLFW_KEY_D) {
            ::g_pLights->theLights[::g_selectedLightIndex].position.x -= object_move_speed;
        }

        if (key == GLFW_KEY_Q) {
            ::g_pLights->theLights[::g_selectedLightIndex].position.y += object_move_speed;
        }

        if (key == GLFW_KEY_E) {
            ::g_pLights->theLights[::g_selectedLightIndex].position.y -= object_move_speed;
        }

        if (key == GLFW_KEY_W) {
            ::g_pLights->theLights[::g_selectedLightIndex].position.z += object_move_speed;
        }

        if (key == GLFW_KEY_S) {
            ::g_pLights->theLights[::g_selectedLightIndex].position.z -= object_move_speed;
        }

        if (key == GLFW_KEY_1) {
            g_pLights->theLights[g_selectedLightIndex].atten.y *= 0.98;
        }

        if (key == GLFW_KEY_2) {
            g_pLights->theLights[g_selectedLightIndex].atten.y *= 1.02;
        }

        if (key == GLFW_KEY_3) {
            g_pLights->theLights[g_selectedLightIndex].atten.z *= 0.98;
        }

        if (key == GLFW_KEY_4) {
            g_pLights->theLights[g_selectedLightIndex].atten.z *= 1.02;
        }

        if (key == GLFW_KEY_P && action == GLFW_RELEASE) {
            if (g_selectedLightIndex >= g_pLights->NUMBEROFLIGHTS - 1) {
                g_selectedLightIndex = 0;
            } else {
                ++g_selectedLightIndex;
            }
        }

        if (key == GLFW_KEY_O && action == GLFW_RELEASE) {
            if (g_selectedLightIndex == 0) {
                g_selectedLightIndex = g_pLights->NUMBEROFLIGHTS - 1;
            } else {
                --g_selectedLightIndex;
            }
        }

        if (key == GLFW_KEY_T && action == GLFW_RELEASE) {
            if (lightDebug) {
                lightDebug = false;
            } else {
                lightDebug = true;
            }
        }
    }

    if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT) {
        if (key == GLFW_KEY_A) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x += object_move_speed;
        }

        if (key == GLFW_KEY_D) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x -= object_move_speed;
        }

        if (key == GLFW_KEY_Q) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.y += object_move_speed;
        }

        if (key == GLFW_KEY_E) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.y -= object_move_speed;
        }

        if (key == GLFW_KEY_W) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z += object_move_speed;
        }

        if (key == GLFW_KEY_S) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z -= object_move_speed;
        }

        if (key == GLFW_KEY_T && action == GLFW_RELEASE) {
            if (meshDebug) {
                meshDebug = false;
            } else {
                meshDebug = true;
            }
        }

        if (key == GLFW_KEY_M) {
            cMeshObject* pNewObject = new cMeshObject();

            // Mesh name
            pNewObject->meshFileName =  ::g_pMeshesToDraw[::g_selectedObjectIndex]->meshFileName;

            // Position
            pNewObject->position.x = ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x;
            pNewObject->position.y = ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.y;
            pNewObject->position.z = ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z;

            // Orientation
            pNewObject->orientation.x = ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.x;
            pNewObject->orientation.y = ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.y;
            pNewObject->orientation.z = ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.z;

            // Scale
            pNewObject->scale = ::g_pMeshesToDraw[::g_selectedObjectIndex]->scale;

            // Color
            pNewObject->colourRGB.r = ::g_pMeshesToDraw[::g_selectedObjectIndex]->colourRGB.r;
            pNewObject->colourRGB.g = ::g_pMeshesToDraw[::g_selectedObjectIndex]->colourRGB.g;
            pNewObject->colourRGB.b = ::g_pMeshesToDraw[::g_selectedObjectIndex]->colourRGB.b;

            // Specular highlight
            pNewObject->specularHighLightRGB.r = ::g_pMeshesToDraw[::g_selectedObjectIndex]->specularHighLightRGB.r;
            pNewObject->specularHighLightRGB.g = ::g_pMeshesToDraw[::g_selectedObjectIndex]->specularHighLightRGB.g;
            pNewObject->specularHighLightRGB.b = ::g_pMeshesToDraw[::g_selectedObjectIndex]->specularHighLightRGB.b;

            // Specular power
            pNewObject->specularPower = ::g_pMeshesToDraw[::g_selectedObjectIndex]->specularPower;

            // Attributes
            pNewObject->bOverrideVertexModelColour = ::g_pMeshesToDraw[::g_selectedObjectIndex]->bOverrideVertexModelColour; // Override color
            pNewObject->bIsVisible = ::g_pMeshesToDraw[::g_selectedObjectIndex]->bIsVisible; // is Visible
            pNewObject->bIsWireframe = ::g_pMeshesToDraw[::g_selectedObjectIndex]->bIsWireframe; // is Wireframe

            ::g_pMeshesToDraw.push_back(pNewObject);
        }

        if (key == GLFW_KEY_INSERT && action == GLFW_RELEASE) {
            std::cout << "Enter new object mesh file name: ";
            std::string input;
            std::getline(std::cin, input);

            cMeshObject* pNewObject = new cMeshObject();
            pNewObject->meshFileName = input;


            std::cout << "Enter new object position (x y z, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                std::istringstream ss(input);

                float inputX;
                float inputY;
                float inputZ;

                if (ss >> inputX >> inputY >> inputZ) {
                    pNewObject->position.x = inputX;
                    pNewObject->position.y = inputY;
                    pNewObject->position.z = inputZ;
                }
            }

            std::cout << "Enter new object orientation (x y z, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                std::istringstream ss(input);

                float inputX;
                float inputY;
                float inputZ;

                if (ss >> inputX >> inputY >> inputZ) {
                    pNewObject->orientation.x = inputX;
                    pNewObject->orientation.y = inputY;
                    pNewObject->orientation.z = inputZ;
                }
            }

            std::cout << "Enter new object scale (press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                pNewObject->scale = std::stof(input);
            }

            std::cout << "Enter new object colour (r g b, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                std::istringstream ss(input);

                float inputX;
                float inputY;
                float inputZ;

                if (ss >> inputX >> inputY >> inputZ) {
                    pNewObject->colourRGB.x = inputX;
                    pNewObject->colourRGB.y = inputY;
                    pNewObject->colourRGB.z = inputZ;
                }
            }

            std::cout << "Enter new object Specular HighLight (r g b, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                std::istringstream ss(input);

                float inputX;
                float inputY;
                float inputZ;

                if (ss >> inputX >> inputY >> inputZ) {
                    pNewObject->specularHighLightRGB.x = inputX;
                    pNewObject->specularHighLightRGB.y = inputY;
                    pNewObject->specularHighLightRGB.z = inputZ;
                }
            }

            std::cout << "Enter new object Specular Power (press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                pNewObject->specularPower = std::stof(input);
            }

            std::cout << "Is new object ColourOverride(1 = True, 0 = False, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                if (std::stoi(input) == 1) {
                    pNewObject->bOverrideVertexModelColour = true;
                } else if (std::stoi(input) == 0) {
                    pNewObject->bOverrideVertexModelColour = false;
                }
            }

            std::cout << "Is new object Wireframe (1 = True, 0 = False, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                if (std::stoi(input) == 1) {
                    pNewObject->bIsWireframe = true;
                } else if (std::stoi(input) == 0) {
                    pNewObject->bIsWireframe = false;
                }
            }

            std::cout << "Is new object Visible(1 = True, 0 = False, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                if (std::stoi(input) == 1) {
                    pNewObject->bIsVisible = true;
                } else if (std::stoi(input) == 0) {
                    pNewObject->bIsVisible = false;
                }
            }

            ::g_pMeshesToDraw.push_back(pNewObject);
            std::getline(std::cin, input); // Flush
        }

        if (key == GLFW_KEY_P && action == GLFW_RELEASE) {
            if (::g_selectedObjectIndex >= ::g_pMeshesToDraw.size() - 1) {
                ::g_selectedObjectIndex = 0;
            } else {
                ++::g_selectedObjectIndex;
            }
        }

        if (key == GLFW_KEY_O && action == GLFW_RELEASE) {
            if (::g_selectedObjectIndex == 0) {
                ::g_selectedObjectIndex = ::g_pMeshesToDraw.size() - 1;
            } else {
                --::g_selectedObjectIndex;
            }
        }

        if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE) {
            std::ofstream mySaveFile("my_scene.scene");

            // Save num of mesh in scene
            mySaveFile << ::g_pMeshesToDraw.size() << std::endl;

            // Save scene objects
            for (size_t index = 0; index < ::g_pMeshesToDraw.size(); index++) {

                // Mesh name
                mySaveFile << ::g_pMeshesToDraw[index]->meshFileName << std::endl;

                // Position
                mySaveFile << ::g_pMeshesToDraw[index]->position.x << " "
                    << g_pMeshesToDraw[index]->position.y << " "
                    << g_pMeshesToDraw[index]->position.z << std::endl;

                // Orientation
                mySaveFile << ::g_pMeshesToDraw[index]->orientation.x << " "
                    << g_pMeshesToDraw[index]->orientation.y << " "
                    << g_pMeshesToDraw[index]->orientation.z << std::endl;

                // Scale
                mySaveFile << ::g_pMeshesToDraw[index]->scale << std::endl;

                // Color
                mySaveFile << ::g_pMeshesToDraw[index]->colourRGB.r << " "
                   << g_pMeshesToDraw[index]->colourRGB.g << " "
                   << g_pMeshesToDraw[index]->colourRGB.b << std::endl;

                // Specular highlight
                mySaveFile << ::g_pMeshesToDraw[index]->specularHighLightRGB.r << " "
                    << g_pMeshesToDraw[index]->specularHighLightRGB.g << " "
                    << g_pMeshesToDraw[index]->specularHighLightRGB.b << std::endl;

                // Specular power
                mySaveFile << ::g_pMeshesToDraw[index]->specularPower << std::endl;

                // Attitudes
                mySaveFile << ::g_pMeshesToDraw[index]->bOverrideVertexModelColour << std::endl; // Override color
                mySaveFile << ::g_pMeshesToDraw[index]->bIsVisible << std::endl; // is Visible
                mySaveFile << ::g_pMeshesToDraw[index]->bIsWireframe << std::endl; // is Wireframe
            }

            // Save number of lights
            mySaveFile << ::g_pLights->NUMBEROFLIGHTS << std::endl;

            // Save lights
            for (int index = 0; index < ::g_pLights->NUMBEROFLIGHTS; index++) {

                // Position
                mySaveFile << ::g_pLights->theLights[index].position.x << " "
                    << g_pLights->theLights[index].position.y << " "
                    << g_pLights->theLights[index].position.z << " "
                    << g_pLights->theLights[index].position.w << std::endl;

                // Diffuse
                mySaveFile << ::g_pLights->theLights[index].diffuse.x << " "
                    << g_pLights->theLights[index].diffuse.y << " "
                    << g_pLights->theLights[index].diffuse.z << " "
                    << g_pLights->theLights[index].diffuse.w << std::endl;

                // Spucular
                mySaveFile << ::g_pLights->theLights[index].specular.x << " "
                    << g_pLights->theLights[index].specular.y << " "
                    << g_pLights->theLights[index].specular.z << " "
                    << g_pLights->theLights[index].specular.w << std::endl;

                // Attenuation
                mySaveFile << ::g_pLights->theLights[index].atten.x << " "
                    << g_pLights->theLights[index].atten.y << " "
                    << g_pLights->theLights[index].atten.z << " "
                    << g_pLights->theLights[index].atten.w << std::endl;

                // Direction
                mySaveFile << ::g_pLights->theLights[index].direction.x << " "
                    << g_pLights->theLights[index].direction.y << " "
                    << g_pLights->theLights[index].direction.z << " "
                    << g_pLights->theLights[index].direction.w << std::endl;

                // Light type
                mySaveFile << ::g_pLights->theLights[index].param1.x << " "
                    << g_pLights->theLights[index].param1.y << " "
                    << g_pLights->theLights[index].param1.z << " "
                    << g_pLights->theLights[index].param1.w << std::endl;

                // Light state
                mySaveFile << ::g_pLights->theLights[index].param2.x << " "
                    << g_pLights->theLights[index].param2.y << " "
                    << g_pLights->theLights[index].param2.z << " "
                    << g_pLights->theLights[index].param2.w << std::endl;
            }

            std::cout << "Scene Saved";
            mySaveFile.close();
        }

        if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
            std::ifstream mySaveFile("my_scene.scene");

            if (!mySaveFile.is_open()) {
                std::cout << "Did not open scene file!" << std::endl;
                return;
            }

            // Delets mesh in vector
            for (cMeshObject* ptr : ::g_pMeshesToDraw) {
                delete ptr;
            }
            ::g_pMeshesToDraw.clear();

            int meshesInScene = 0;
            mySaveFile >> meshesInScene;\

            // Load meshs
            for (int index = 0; index < meshesInScene; index++) {
                cMeshObject* pNewObject = new cMeshObject();

                mySaveFile >> pNewObject->meshFileName;
                mySaveFile >> pNewObject->position.x >> pNewObject->position.y >> pNewObject->position.z;
                mySaveFile >> pNewObject->orientation.x >> pNewObject->orientation.y >> pNewObject->orientation.z;
                mySaveFile >> pNewObject->scale;
                mySaveFile >> pNewObject->colourRGB.r >> pNewObject->colourRGB.g >> pNewObject->colourRGB.b;
                mySaveFile >> pNewObject->specularHighLightRGB.r >> pNewObject->specularHighLightRGB.g >> pNewObject->specularHighLightRGB.b;
                mySaveFile >> pNewObject->specularPower;
                mySaveFile >> pNewObject->bOverrideVertexModelColour;
                mySaveFile >> pNewObject->bIsVisible;
                mySaveFile >> pNewObject->bIsWireframe;


                ::g_pMeshesToDraw.push_back(pNewObject);
            }

            // Load number of lights
            int lightsInScene = 0;
            mySaveFile >> lightsInScene;
            for (int i = 0; i < lightsInScene && i < g_pLights->NUMBEROFLIGHTS; ++i) {\

                // Position
                mySaveFile >> g_pLights->theLights[i].position.x
                    >> g_pLights->theLights[i].position.y
                    >> g_pLights->theLights[i].position.z
                    >> g_pLights->theLights[i].position.w;

                // Diffuse
                mySaveFile >> g_pLights->theLights[i].diffuse.x
                    >> g_pLights->theLights[i].diffuse.y
                    >> g_pLights->theLights[i].diffuse.z
                    >> g_pLights->theLights[i].diffuse.w;

                // Specular
                mySaveFile >> g_pLights->theLights[i].specular.x
                    >> g_pLights->theLights[i].specular.y
                    >> g_pLights->theLights[i].specular.z
                    >> g_pLights->theLights[i].specular.w;

                // Attenuation
                mySaveFile >> g_pLights->theLights[i].atten.x
                    >> g_pLights->theLights[i].atten.y
                    >> g_pLights->theLights[i].atten.z
                    >> g_pLights->theLights[i].atten.w;

                // Direction
                mySaveFile >> g_pLights->theLights[i].direction.x
                    >> g_pLights->theLights[i].direction.y
                    >> g_pLights->theLights[i].direction.z
                    >> g_pLights->theLights[i].direction.w;

                // Light type
                mySaveFile >> g_pLights->theLights[i].param1.x
                    >> g_pLights->theLights[i].param1.y
                    >> g_pLights->theLights[i].param1.z
                    >> g_pLights->theLights[i].param1.w;

                // Light state
                mySaveFile >> g_pLights->theLights[i].param2.x
                    >> g_pLights->theLights[i].param2.y
                    >> g_pLights->theLights[i].param2.z
                    >> g_pLights->theLights[i].param2.w;
            }

            std::cout << "Scene Loaded";
            mySaveFile.close();
        }

        if (key == GLFW_KEY_BACKSPACE && action == GLFW_RELEASE) {
            // Delets mesh in vector
            for (cMeshObject* ptr : ::g_pMeshesToDraw) {
                delete ptr;
            }
            ::g_pMeshesToDraw.clear();

            std::cout << "Scene Cleared";
        }
    }

    if (!areAnyModifiersDown(mods)) {
        if (key == GLFW_KEY_A) {
            camera.Position += camera.speed * -glm::normalize(glm::cross(camera.Orientation, camera.Up));
        }

        if (key == GLFW_KEY_D) {
             camera.Position += camera.speed * glm::normalize(glm::cross(camera.Orientation, camera.Up));
        }

        if (key == GLFW_KEY_W) {
             camera.Position += camera.speed * camera.Orientation;
        }

        if (key == GLFW_KEY_S) {
             camera.Position += camera.speed * -camera.Orientation;
        }

        if (key == GLFW_KEY_Q) {
             camera.Position += camera.speed * -camera.Up;
        }

        if (key == GLFW_KEY_E) {
             camera.Position += camera.speed * camera.Up;
        }

        if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
            camera.speed += 0.05f;
        }

        if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
            camera.speed -= 0.05f;
        }

        if (key == GLFW_KEY_T && action == GLFW_RELEASE) {
            if (g_LightingType >= 2) {
                g_LightingType = 0;
            } else {
                ++g_LightingType;
            }
        }
    }
}
