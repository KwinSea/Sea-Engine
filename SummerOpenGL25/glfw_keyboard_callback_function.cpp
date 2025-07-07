#include "GlobalOpenGL.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include "cMeshObject.h"
#include "cLightManager.h"

extern glm::vec3 g_cameraEye;
extern std::vector<cMeshObject*> g_pMeshesToDraw;
extern cLightManager* g_pLights;
extern glm::vec3 cameraTarget;

unsigned int g_selectedObjectIndex = 0;
unsigned int g_selectedLightIndex = 0;
bool lightDebug = false;

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

    float camera_speed = 0.5f;
    float camera_move_speed = 0.5f;
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

    if ((mods & GLFW_MOD_ALT) == GLFW_MOD_ALT) {
        if (key == GLFW_KEY_W) {
            camera_speed += 0.5;
        }

        if (key == GLFW_KEY_S) {
            camera_speed -= 0.5;
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
                mySaveFile << ::g_pMeshesToDraw[index]->meshFileName << std::endl;
                mySaveFile << ::g_pMeshesToDraw[index]->position.x << " "
                           << ::g_pMeshesToDraw[index]->position.y << " "
                           << ::g_pMeshesToDraw[index]->position.z << std::endl;
                mySaveFile << ::g_pMeshesToDraw[index]->orientation.x << " "
                           << ::g_pMeshesToDraw[index]->orientation.y << " "
                           << ::g_pMeshesToDraw[index]->orientation.z << std::endl;
                mySaveFile << ::g_pMeshesToDraw[index]->scale << std::endl;
            }

            // Save number of lights
            mySaveFile << ::g_pLights->NUMBEROFLIGHTS << std::endl;

            // Save lights
            for (int index = 0; index < ::g_pLights->NUMBEROFLIGHTS; index++) {
                mySaveFile << ::g_pLights->theLights[index].position.x << " "
                           << ::g_pLights->theLights[index].position.y << " "
                           << ::g_pLights->theLights[index].position.z << " "
                           << ::g_pLights->theLights[index].position.w << std::endl;
                mySaveFile << ::g_pLights->theLights[index].diffuse.x << " "
                           << ::g_pLights->theLights[index].diffuse.y << " "
                           << ::g_pLights->theLights[index].diffuse.z << " "
                           << ::g_pLights->theLights[index].diffuse.w << std::endl;
                mySaveFile << ::g_pLights->theLights[index].specular.x << " "
                           << ::g_pLights->theLights[index].specular.y << " "
                           << ::g_pLights->theLights[index].specular.z << " "
                           << ::g_pLights->theLights[index].specular.w << std::endl;
                mySaveFile << ::g_pLights->theLights[index].atten.x << " "
                           << ::g_pLights->theLights[index].atten.y << " "
                           << ::g_pLights->theLights[index].atten.z << " "
                           << ::g_pLights->theLights[index].atten.w << std::endl;
                mySaveFile << ::g_pLights->theLights[index].direction.x << " "
                           << ::g_pLights->theLights[index].direction.y << " "
                           << ::g_pLights->theLights[index].direction.z << " "
                           << ::g_pLights->theLights[index].direction.w << std::endl;
                mySaveFile << ::g_pLights->theLights[index].param1.x << " "
                           << ::g_pLights->theLights[index].param1.y << " "
                           << ::g_pLights->theLights[index].param1.z << " "
                           << ::g_pLights->theLights[index].param1.w << std::endl;
                mySaveFile << ::g_pLights->theLights[index].param2.x << " "
                           << ::g_pLights->theLights[index].param2.y << " "
                           << ::g_pLights->theLights[index].param2.z << " "
                           << ::g_pLights->theLights[index].param2.w << std::endl;
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
            mySaveFile >> meshesInScene;

            for (int index = 0; index < meshesInScene; index++) {
                cMeshObject* pNewObject = new cMeshObject();

                mySaveFile >> pNewObject->meshFileName;
                mySaveFile >> pNewObject->position.x >> pNewObject->position.y >> pNewObject->position.z;
                mySaveFile >> pNewObject->orientation.x >> pNewObject->orientation.y >> pNewObject->orientation.z;
                mySaveFile >> pNewObject->scale;

                ::g_pMeshesToDraw.push_back(pNewObject);
            }

            // Load number of lights
            int lightsInScene = 0;
            mySaveFile >> lightsInScene;
            for (int i = 0; i < lightsInScene && i < g_pLights->NUMBEROFLIGHTS; ++i) {
                mySaveFile >> g_pLights->theLights[i].position.x
                           >> g_pLights->theLights[i].position.y
                           >> g_pLights->theLights[i].position.z
                           >> g_pLights->theLights[i].position.w;
                mySaveFile >> g_pLights->theLights[i].diffuse.x
                           >> g_pLights->theLights[i].diffuse.y
                           >> g_pLights->theLights[i].diffuse.z
                           >> g_pLights->theLights[i].diffuse.w;
                mySaveFile >> g_pLights->theLights[i].specular.x
                           >> g_pLights->theLights[i].specular.y
                           >> g_pLights->theLights[i].specular.z
                           >> g_pLights->theLights[i].specular.w;
                mySaveFile >> g_pLights->theLights[i].atten.x
                           >> g_pLights->theLights[i].atten.y
                           >> g_pLights->theLights[i].atten.z
                           >> g_pLights->theLights[i].atten.w;
                mySaveFile >> g_pLights->theLights[i].direction.x
                           >> g_pLights->theLights[i].direction.y
                           >> g_pLights->theLights[i].direction.z
                           >> g_pLights->theLights[i].direction.w;
                mySaveFile >> g_pLights->theLights[i].param1.x
                           >> g_pLights->theLights[i].param1.y
                           >> g_pLights->theLights[i].param1.z
                           >> g_pLights->theLights[i].param1.w;
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

    if ((mods & GLFW_MOD_ALT) == GLFW_MOD_ALT) {
        if (key == GLFW_KEY_A) {
            ::g_cameraEye.x += camera_speed;
        }

        if (key == GLFW_KEY_D) {
            ::g_cameraEye.x -= camera_speed;
        }

        if (key == GLFW_KEY_Q) {
            ::g_cameraEye.y += camera_speed;
        }

        if (key == GLFW_KEY_E) {
            ::g_cameraEye.y -= camera_speed;
        }
    }

    if (!areAnyModifiersDown(mods)) {
        if (key == GLFW_KEY_A) {
            ::cameraTarget.x += camera_move_speed;
            ::g_cameraEye.x += camera_speed;
        }

        if (key == GLFW_KEY_D) {
            ::cameraTarget.x -= camera_move_speed;
            ::g_cameraEye.x -= camera_speed;
        }

        if (key == GLFW_KEY_W) {
            ::cameraTarget.z += camera_move_speed;
            ::g_cameraEye.z += camera_speed;
        }

        if (key == GLFW_KEY_S) {
            ::cameraTarget.z -= camera_move_speed;
            ::g_cameraEye.z -= camera_speed;
        }

        if (key == GLFW_KEY_Q) {
            ::cameraTarget.y += camera_move_speed;
            ::g_cameraEye.y += camera_speed;
        }

        if (key == GLFW_KEY_E) {
            ::cameraTarget.y -= camera_move_speed;
            ::g_cameraEye.y -= camera_speed;
        }
    }
}
