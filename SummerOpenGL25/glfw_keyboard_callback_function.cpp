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
#include "Functions.h"
#include "cVAOManager/cVAOManager.h"

extern Camera camera;
extern std::vector<cMeshObject*> g_pMeshesToDraw;
extern cLightManager* g_pLights;

extern double deltaTime;

extern cVAOManager* pTheMeshManager;

extern bool usingGui;
extern int g_LightingType;
extern unsigned int screenWidth;
extern unsigned int screenHeight;

int g_selectedObjectIndex = 0;
int g_selectedLightIndex = 0;

float object_move_speed = 50.f;
float light_move_speed = 100.f;

float object_move_grid = 50.f;
float light_move_grid = 100.f;

float object_rotate_speed = glm::radians(90.0f);

bool lightDebug = false;
bool meshDebug = false;

bool isObjectGridSnap = false;
bool isLightGridSnap = false;

static std::pair<glm::vec3, glm::vec3> cameraAngles[4] = {
    std::make_pair(glm::vec3(0.0), glm::vec3(0.0)),
    std::make_pair(glm::vec3(0.0), glm::vec3(0.0)),
    std::make_pair(glm::vec3(0.0), glm::vec3(0.0)),
    std::make_pair(glm::vec3(0.0), glm::vec3(0.0))
};

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

    if (usingGui) {
        return;
    }

    if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL) {
        if (isLightGridSnap) {
             if (key == GLFW_KEY_E && action != GLFW_RELEASE) {
                g_pLights->theLights[::g_selectedLightIndex].position.y = light_move_grid * static_cast<float>(static_cast<int>(g_pLights->theLights[::g_selectedLightIndex].position.y / light_move_grid)) + light_move_grid;
            }
            if (key == GLFW_KEY_Q && action != GLFW_RELEASE) {
                g_pLights->theLights[::g_selectedLightIndex].position.y = light_move_grid * static_cast<float>(static_cast<int>(g_pLights->theLights[::g_selectedLightIndex].position.y / light_move_grid)) - light_move_grid;
            }
            if (key == GLFW_KEY_W && action != GLFW_RELEASE) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    g_pLights->theLights[::g_selectedLightIndex].position.x = light_move_grid * static_cast<float>(static_cast<int>(g_pLights->theLights[::g_selectedLightIndex].position.x / light_move_grid)) + (camera.Orientation.x > 0 ? light_move_grid : -light_move_grid);
                } else {
                    g_pLights->theLights[::g_selectedLightIndex].position.z = light_move_grid * static_cast<float>(static_cast<int>(g_pLights->theLights[::g_selectedLightIndex].position.z / light_move_grid)) + (camera.Orientation.z > 0 ? light_move_grid : -light_move_grid);
                }
            }
            if (key == GLFW_KEY_S && action != GLFW_RELEASE) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    g_pLights->theLights[::g_selectedLightIndex].position.x = light_move_grid * static_cast<float>(static_cast<int>(g_pLights->theLights[::g_selectedLightIndex].position.x / light_move_grid)) - (camera.Orientation.x > 0 ? light_move_grid : -light_move_grid);
                } else {
                    g_pLights->theLights[::g_selectedLightIndex].position.z = light_move_grid * static_cast<float>(static_cast<int>(g_pLights->theLights[::g_selectedLightIndex].position.z / light_move_grid)) - (camera.Orientation.z > 0 ? light_move_grid : -light_move_grid);
                }
            }
            if (key == GLFW_KEY_A && action != GLFW_RELEASE) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    g_pLights->theLights[::g_selectedLightIndex].position.z = light_move_grid * static_cast<float>(static_cast<int>(g_pLights->theLights[::g_selectedLightIndex].position.z / light_move_grid)) - (camera.Orientation.x > 0 ? light_move_grid : -light_move_grid);
                } else {
                    g_pLights->theLights[::g_selectedLightIndex].position.x = light_move_grid * static_cast<float>(static_cast<int>(g_pLights->theLights[::g_selectedLightIndex].position.x / light_move_grid)) + (camera.Orientation.z > 0 ? light_move_grid : -light_move_grid);
                }
            }
            if (key == GLFW_KEY_D && action != GLFW_RELEASE) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    g_pLights->theLights[::g_selectedLightIndex].position.z = light_move_grid * static_cast<float>(static_cast<int>(g_pLights->theLights[::g_selectedLightIndex].position.z / light_move_grid)) + (camera.Orientation.x > 0 ? light_move_grid : -light_move_grid);
                } else {
                    g_pLights->theLights[::g_selectedLightIndex].position.x = light_move_grid * static_cast<float>(static_cast<int>(g_pLights->theLights[::g_selectedLightIndex].position.x / light_move_grid)) - (camera.Orientation.z > 0 ? light_move_grid : -light_move_grid);
                }
            }
        } else {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    g_pLights->theLights[::g_selectedLightIndex].position.x += (camera.Orientation.x > 0 ? 1 : -1) * light_move_speed * deltaTime;
                } else {
                    g_pLights->theLights[::g_selectedLightIndex].position.z += (camera.Orientation.z > 0 ? 1 : -1) * light_move_speed * deltaTime;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    g_pLights->theLights[::g_selectedLightIndex].position.x -= (camera.Orientation.x > 0 ? 1 : -1) * light_move_speed * deltaTime;
                } else {
                    g_pLights->theLights[::g_selectedLightIndex].position.z -= (camera.Orientation.z > 0 ? 1 : -1) * light_move_speed * deltaTime;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    g_pLights->theLights[::g_selectedLightIndex].position.z -= light_move_speed * deltaTime * (camera.Orientation.x > 0 ? 1 : -1);
                } else {
                    g_pLights->theLights[::g_selectedLightIndex].position.x += light_move_speed * deltaTime * (camera.Orientation.z > 0 ? 1 : -1);
                }
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    g_pLights->theLights[::g_selectedLightIndex].position.z += light_move_speed * deltaTime * (camera.Orientation.x > 0 ? 1 : -1);
                } else {
                    g_pLights->theLights[::g_selectedLightIndex].position.x -= light_move_speed * deltaTime * (camera.Orientation.z > 0 ? 1 : -1);
                }
            }
        }

        if (key == GLFW_KEY_1 && action != GLFW_RELEASE) {
            g_pLights->theLights[g_selectedLightIndex].atten.y *= 0.98;
        }

        if (key == GLFW_KEY_2 && action != GLFW_RELEASE) {
            g_pLights->theLights[g_selectedLightIndex].atten.y *= 1.02;
        }

        if (key == GLFW_KEY_3 && action != GLFW_RELEASE) {
            g_pLights->theLights[g_selectedLightIndex].atten.z *= 0.98;
        }

        if (key == GLFW_KEY_4 && action != GLFW_RELEASE) {
            g_pLights->theLights[g_selectedLightIndex].atten.z *= 1.02;
        }

        if (key == GLFW_KEY_PAGE_UP && action != GLFW_RELEASE) {
            light_move_speed += 5.0f;
        }

        if (key == GLFW_KEY_PAGE_DOWN && action != GLFW_RELEASE) {
            light_move_speed -= 5.0f;
        }

        if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) {
            if (g_selectedLightIndex >= g_pLights->NUMBEROFLIGHTS - 1) {
                g_selectedLightIndex = 0;
            } else {
                ++g_selectedLightIndex;
            }
        }

        if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS) {
            if (g_selectedLightIndex == 0) {
                g_selectedLightIndex = g_pLights->NUMBEROFLIGHTS - 1;
            } else {
                --g_selectedLightIndex;
            }
        }

        if (key == GLFW_KEY_T && action == GLFW_PRESS) {
            if (lightDebug) {
                lightDebug = false;
            } else {
                lightDebug = true;
            }
        }

        if (key == GLFW_KEY_I && action == GLFW_RELEASE) {
            std::string input;
            std::cout << "Editing Selected Light\nIs light on (1 = True, 0 = False press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                 g_pLights->theLights[g_selectedLightIndex].param2.x = std::stof(input);
            }

            std::cout << "Enter light type (0 = point light, 1 = spot light, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                 g_pLights->theLights[g_selectedLightIndex].param1.x = std::stof(input);
                 if (g_pLights->theLights[g_selectedLightIndex].param1.x == 1.0f) {
                     std::cout << "Enter light inner angle (press Enter to skip): ";
                     std::getline(std::cin, input);
                     if (!input.empty()) {
                         g_pLights->theLights[g_selectedLightIndex].param1.y = std::stof(input);
                     }

                     std::cout << "Enter light outer angle (press Enter to skip): ";
                     std::getline(std::cin, input);
                     if (!input.empty()) {
                         g_pLights->theLights[g_selectedLightIndex].param1.z = std::stof(input);
                     }

                     std::cout << "Enter light new direction (x y z, press Enter to skip): ";
                     std::getline(std::cin, input);
                     if (!input.empty()) {
                         std::istringstream ss(input);

                         float inputX;
                         float inputY;
                         float inputZ;

                         if (ss >> inputX >> inputY >> inputZ) {
                             g_pLights->theLights[g_selectedLightIndex].direction.x = inputX;
                             g_pLights->theLights[g_selectedLightIndex].direction.y = inputY;
                             g_pLights->theLights[g_selectedLightIndex].direction.z = inputZ;
                         }
                     }
                 }
            }

            std::cout << "Enter light new position (x y z, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                std::istringstream ss(input);

                float inputX;
                float inputY;
                float inputZ;

                if (ss >> inputX >> inputY >> inputZ) {
                    g_pLights->theLights[g_selectedLightIndex].position.x = inputX;
                    g_pLights->theLights[g_selectedLightIndex].position.y = inputY;
                    g_pLights->theLights[g_selectedLightIndex].position.z = inputZ;
                }
            }

            std::cout << "Enter light new diffuse (x y z, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                std::istringstream ss(input);

                float inputX;
                float inputY;
                float inputZ;

                if (ss >> inputX >> inputY >> inputZ) {
                    g_pLights->theLights[g_selectedLightIndex].diffuse = glm::vec4(RGBify(inputX, inputY, inputZ), 1.0f);
                }
            }

            std::cout << "Enter light new specular (x y z, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                std::istringstream ss(input);

                float inputX;
                float inputY;
                float inputZ;

                if (ss >> inputX >> inputY >> inputZ) {
                    g_pLights->theLights[g_selectedLightIndex].specular = glm::vec4(RGBify(inputX, inputY, inputZ), 1.0f);
                }
            }

            std::cout << "Enter light new attention (constant linear quadratic, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                std::istringstream ss(input);

                float inputX;
                float inputY;
                float inputZ;

                if (ss >> inputX >> inputY >> inputZ) {
                    g_pLights->theLights[g_selectedLightIndex].atten.x = inputX;
                    g_pLights->theLights[g_selectedLightIndex].atten.y = inputY;
                    g_pLights->theLights[g_selectedLightIndex].atten.z = inputZ;
                }
            }
            std::cout << "Light Edited\n";
        }
    }

    if ((mods & GLFW_MOD_ALT) == GLFW_MOD_ALT) {
        if (key == GLFW_KEY_A  && action != GLFW_RELEASE) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.y += object_rotate_speed;
        }

        if (key == GLFW_KEY_D  && action != GLFW_RELEASE) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.y -= object_rotate_speed;
        }

        if (key == GLFW_KEY_Q  && action != GLFW_RELEASE) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.x += object_rotate_speed;
        }

        if (key == GLFW_KEY_E  && action != GLFW_RELEASE) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.x -= object_rotate_speed;
        }

        if (key == GLFW_KEY_W  && action != GLFW_RELEASE) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.z += object_rotate_speed;
        }

        if (key == GLFW_KEY_S  && action != GLFW_RELEASE) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.z -= object_rotate_speed;
        }

        if (key == GLFW_KEY_PAGE_UP && action != GLFW_RELEASE) {
            object_rotate_speed += glm::radians(5.0f);
        }

        if (key == GLFW_KEY_PAGE_DOWN && action != GLFW_RELEASE) {
            object_rotate_speed -= glm::radians(5.0f);
        }
    }

    if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT) {
        if (isObjectGridSnap) {
            if (key == GLFW_KEY_E && action != GLFW_RELEASE) {
                ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.y = object_move_grid * static_cast<float>(static_cast<int>(g_pMeshesToDraw[::g_selectedObjectIndex]->position.y / object_move_grid)) + object_move_grid;
            }
            if (key == GLFW_KEY_Q && action != GLFW_RELEASE) {
                ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.y = object_move_grid * static_cast<float>(static_cast<int>(g_pMeshesToDraw[::g_selectedObjectIndex]->position.y / object_move_grid)) - object_move_grid;
            }
            if (key == GLFW_KEY_W && action != GLFW_RELEASE) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x = object_move_grid * static_cast<float>(static_cast<int>(g_pMeshesToDraw[::g_selectedObjectIndex]->position.x / object_move_grid)) + (camera.Orientation.x > 0 ? object_move_grid : -object_move_grid);
                } else {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z = object_move_grid * static_cast<float>(static_cast<int>(g_pMeshesToDraw[::g_selectedObjectIndex]->position.z / object_move_grid)) + (camera.Orientation.z > 0 ? object_move_grid : -object_move_grid);
                }
            }
            if (key == GLFW_KEY_S && action != GLFW_RELEASE) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x = object_move_grid * static_cast<float>(static_cast<int>(g_pMeshesToDraw[::g_selectedObjectIndex]->position.x / object_move_grid)) - (camera.Orientation.x > 0 ? object_move_grid : -object_move_grid);
                } else {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z = object_move_grid * static_cast<float>(static_cast<int>(g_pMeshesToDraw[::g_selectedObjectIndex]->position.z / object_move_grid)) - (camera.Orientation.z > 0 ? object_move_grid : -object_move_grid);
                }
            }
            if (key == GLFW_KEY_A && action != GLFW_RELEASE) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z = object_move_grid * static_cast<float>(static_cast<int>(g_pMeshesToDraw[::g_selectedObjectIndex]->position.z / object_move_grid)) - (camera.Orientation.x > 0 ? object_move_grid : -object_move_grid);
                } else {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x = object_move_grid * static_cast<float>(static_cast<int>(g_pMeshesToDraw[::g_selectedObjectIndex]->position.x / object_move_grid)) + (camera.Orientation.z > 0 ? object_move_grid : -object_move_grid);
                }
            }
            if (key == GLFW_KEY_D && action != GLFW_RELEASE) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z = object_move_grid * static_cast<float>(static_cast<int>(g_pMeshesToDraw[::g_selectedObjectIndex]->position.z / object_move_grid)) + (camera.Orientation.x > 0 ? object_move_grid : -object_move_grid);
                } else {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x = object_move_grid * static_cast<float>(static_cast<int>(g_pMeshesToDraw[::g_selectedObjectIndex]->position.x / object_move_grid)) - (camera.Orientation.z > 0 ? object_move_grid : -object_move_grid);
                }
            }
        } else {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x += (camera.Orientation.x > 0 ? 1 : -1) * object_move_speed * deltaTime;
                } else {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z += (camera.Orientation.z > 0 ? 1 : -1) * object_move_speed * deltaTime;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x -= (camera.Orientation.x > 0 ? 1 : -1) * object_move_speed * deltaTime;
                } else {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z -= (camera.Orientation.z > 0 ? 1 : -1) * object_move_speed * deltaTime;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z -= object_move_speed * deltaTime * (camera.Orientation.x > 0 ? 1 : -1);
                } else {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x += object_move_speed * deltaTime * (camera.Orientation.z > 0 ? 1 : -1);
                }
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                if (std::abs(camera.Orientation.x) > std::abs(camera.Orientation.z)) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z += object_move_speed * deltaTime * (camera.Orientation.x > 0 ? 1 : -1);
                } else {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x -= object_move_speed * deltaTime * (camera.Orientation.z > 0 ? 1 : -1);
                }
            }
        }


        if (key == GLFW_KEY_T && action == GLFW_PRESS) {
            if (meshDebug) {
                meshDebug = false;
            } else {
                meshDebug = true;
            }
        }

        if (key == GLFW_KEY_H && action == GLFW_PRESS && !g_pMeshesToDraw.empty() && g_pMeshesToDraw[32] != nullptr) {
            g_pMeshesToDraw[32]->colourRGB = glm::vec3(0.0f);
            g_pLights->theLights[0].param2.x = 0.0f;
        }

        if (key == GLFW_KEY_G && action == GLFW_PRESS && !g_pMeshesToDraw.empty() && g_pMeshesToDraw[32] != nullptr) {
            g_pMeshesToDraw[32]->colourRGB = glm::vec3(RGBify(0, 112, 255));
            g_pLights->theLights[0].param2.x = 1.0f;
        }

        if (key == GLFW_KEY_M && action == GLFW_PRESS) {
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

            // Opacity
            pNewObject->opacityAlpha = ::g_pMeshesToDraw[::g_selectedObjectIndex]->opacityAlpha;

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

        if (key == GLFW_KEY_I && action == GLFW_RELEASE) {

            std::cout << "Editing Selected Object\nEnter object new position (x y z, press Enter to skip): ";
            std::string input;
            std::getline(std::cin, input);
            if (!input.empty()) {
                std::istringstream ss(input);

                float inputX;
                float inputY;
                float inputZ;

                if (ss >> inputX >> inputY >> inputZ) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x = inputX;
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.y = inputY;
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z = inputZ;
                }
            }

            std::cout << "Enter object new orientation (x y z, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                std::istringstream ss(input);

                float inputX;
                float inputY;
                float inputZ;

                if (ss >> inputX >> inputY >> inputZ) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.x = inputX;
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.y = inputY;
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->orientation.z = inputZ;
                }
            }

            std::cout << "Enter object new scale (press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                ::g_pMeshesToDraw[::g_selectedObjectIndex]->scale = std::stof(input);
            }

            std::cout << "Enter object new colour (r g b, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                std::istringstream ss(input);

                float inputX;
                float inputY;
                float inputZ;

                if (ss >> inputX >> inputY >> inputZ) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->colourRGB = glm::vec4(RGBify(inputX, inputY, inputZ), 1.0f);
                }
            }

            std::cout << "Enter object new Specular HighLight (r g b, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                std::istringstream ss(input);

                float inputX;
                float inputY;
                float inputZ;

                if (ss >> inputX >> inputY >> inputZ) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->specularHighLightRGB.x = inputX;
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->specularHighLightRGB.y = inputY;
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->specularHighLightRGB.z = inputZ;
                }
            }

            std::cout << "Enter object new Specular Power (press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                ::g_pMeshesToDraw[::g_selectedObjectIndex]->specularPower = std::stof(input);
            }

            std::cout << "Is object ColourOverride(1 = True, 0 = False, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                if (std::stoi(input) == 1) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->bOverrideVertexModelColour = true;
                } else if (std::stoi(input) == 0) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->bOverrideVertexModelColour = false;
                }
            }

            std::cout << "Is object Wireframe (1 = True, 0 = False, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                if (std::stoi(input) == 1) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->bIsWireframe = true;
                } else if (std::stoi(input) == 0) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->bIsWireframe = false;
                }
            }

            std::cout << "Is object Visible(1 = True, 0 = False, press Enter to skip): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                if (std::stoi(input) == 1) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->bIsVisible = true;
                } else if (std::stoi(input) == 0) {
                    ::g_pMeshesToDraw[::g_selectedObjectIndex]->bIsVisible = false;
                }
            }
            std::cout << "Object Edited\n";
        }

        if (key == GLFW_KEY_INSERT && action == GLFW_RELEASE) {
            cMeshObject* pNewObject = new cMeshObject();

            std::cout << "Adding New Object\n";
            std::vector<std::string> currentVAOMeshs;
            for (const std::pair<const std::string, sModelDrawInfo>& pair : pTheMeshManager->GetMapOfMesh()) {
                currentVAOMeshs.push_back(pair.first);
            }

            for (int i = 0; i < currentVAOMeshs.size(); i++) {
                std::cout << "[" << i << "] - " << currentVAOMeshs[i] << std::endl;
            }

            std::cout << "\nSelect new object mesh from above: ";
            std::string input;
            std::getline(std::cin, input);
            pNewObject->meshFileName = currentVAOMeshs[std::stof(input)];

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
                    pNewObject->colourRGB = glm::vec4(RGBify(inputX, inputY, inputZ), 1.0f);
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
            std::cout << "Object Added\n";
        }

        if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) {
            if (::g_selectedObjectIndex >= ::g_pMeshesToDraw.size() - 1) {
                ::g_selectedObjectIndex = 0;
            } else {
                ++::g_selectedObjectIndex;
            }
        }

        if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS) {
            if (::g_selectedObjectIndex == 0) {
                ::g_selectedObjectIndex = ::g_pMeshesToDraw.size() - 1;
            } else {
                --::g_selectedObjectIndex;
            }
        }

        if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE) {
            SaveScene();
        }

        if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
            LoadScene();
        }

        if (key == GLFW_KEY_BACKSPACE && action == GLFW_RELEASE) {
            delete g_pMeshesToDraw[g_selectedObjectIndex];
            g_pMeshesToDraw.erase(g_pMeshesToDraw.begin() + g_selectedObjectIndex);

            if (g_selectedObjectIndex >= g_pMeshesToDraw.size() && !g_pMeshesToDraw.empty()) {
                g_selectedObjectIndex = g_pMeshesToDraw.size() - 1;
            } else if (g_pMeshesToDraw.empty()) {
                g_selectedObjectIndex = 0;
            }

            std::cout << "Object deleted\n";
        }

        // Save camera angle 1
        if (key == GLFW_KEY_F1 && action == GLFW_RELEASE) {
            cameraAngles[0] = std::make_pair(camera.Position, camera.Orientation);
            std::cout << "Camera Angle 1 Saved";
        }

        // Save camera angle 2
        if (key == GLFW_KEY_F2 && action == GLFW_RELEASE) {
            cameraAngles[1] = std::make_pair(camera.Position, camera.Orientation);
            std::cout << "Camera Angle 2 Saved";
        }

        // Save camera angle 3
        if (key == GLFW_KEY_F3 && action == GLFW_RELEASE) {
            cameraAngles[2] = std::make_pair(camera.Position, camera.Orientation);
            std::cout << "Camera Angle 3 Saved";
        }

        // Save camera angle 4
        if (key == GLFW_KEY_F4 && action == GLFW_RELEASE) {
            cameraAngles[3] = std::make_pair(camera.Position, camera.Orientation);
            std::cout << "Camera Angle 4 Saved";
        }
    }

    if (!areAnyModifiersDown(mods)) {
        if (key == GLFW_KEY_T && action == GLFW_PRESS) {
            if (g_LightingType >= 2) {
                g_LightingType = 0;
            } else {
                ++g_LightingType;
            }
        }

        // Load camera angle 1
        if (key == GLFW_KEY_F1 && action == GLFW_RELEASE) {
            camera.Position = cameraAngles[0].first;
            camera.Orientation = cameraAngles[0].second;
            std::cout << "Camera Angle 1 loaded";
        }

        // Load camera angle 2
        if (key == GLFW_KEY_F2 && action == GLFW_RELEASE) {
            camera.Position = cameraAngles[1].first;
            camera.Orientation = cameraAngles[1].second;
            std::cout << "Camera Angle 2 loaded";
        }

        // Load camera angle 3
        if (key == GLFW_KEY_F3 && action == GLFW_RELEASE) {
            camera.Position = cameraAngles[2].first;
            camera.Orientation = cameraAngles[2].second;
            std::cout << "Camera Angle 3 loaded";
        }

        // Load camera angle 4
        if (key == GLFW_KEY_F4 && action == GLFW_RELEASE) {
            camera.Position = cameraAngles[3].first;
            camera.Orientation = cameraAngles[3].second;
            std::cout << "Camera Angle 4 loaded";
        }
    }
}

// Scroll wheel
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (usingGui)
        return;

    if (yoffset > 0) {
        camera.speed += 10.0f;
    }
    else if (yoffset < 0 && camera.speed > 5) {
        camera.speed -= 10.0f;
    }
}
