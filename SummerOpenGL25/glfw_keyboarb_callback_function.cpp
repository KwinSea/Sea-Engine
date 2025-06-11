#include "GlobalOpenGL.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include "cMeshObject.h"
#include <fstream>
#include <iostream>
#include <sstream>

extern glm::vec3 g_cameraEye;
extern std::vector<cMeshObject*> g_pMeshesToDraw;

unsigned int g_selectedObjectIndex = 0;

bool isShiftDown (int mods){
    if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT)
    {
        return true;
    }

    return false;
}

bool isCtrlDown(int mods) {
    if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL) {
        return true;
    }

    return false;
}

bool isAnyModDown(int mods) {
    if (isShiftDown(mods) || isCtrlDown(mods)){
        return true;
    }

    return false;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    const float camera_speed = 0.5f;
    const float objectMoveSpeed = 0.7f;

    if (isShiftDown(mods)) {
	    if (key == GLFW_KEY_A) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x -= objectMoveSpeed;
	    }

        if (key == GLFW_KEY_D) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x += objectMoveSpeed;
        }

        if (key == GLFW_KEY_W) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.y += objectMoveSpeed;
        }


        if (key == GLFW_KEY_S) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.y -= objectMoveSpeed;
        }

        if (key == GLFW_KEY_Q) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->scale += objectMoveSpeed;
        }


        if (key == GLFW_KEY_E) {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->scale -= objectMoveSpeed;
        }

	    if (key == GLFW_KEY_P && action == GLFW_RELEASE)
	    {
		    if (::g_selectedObjectIndex < (g_pMeshesToDraw.size() - 1))
		    {
                ++::g_selectedObjectIndex;
		    }
		    else
		    {
                ::g_selectedObjectIndex = 0;
		    }
	    }
        if (key == GLFW_KEY_O && action == GLFW_RELEASE) {
            if (::g_selectedObjectIndex > 0) {
                --::g_selectedObjectIndex;
            } else {
                ::g_selectedObjectIndex = g_pMeshesToDraw.size() - 1;
            }
        }
        if (key == GLFW_KEY_BACKSPACE && action == GLFW_RELEASE) {
            g_pMeshesToDraw.clear();
            std::cout << "Scene Cleared";
        }
    }

    // Save scene
    if (key == GLFW_KEY_K && action == GLFW_RELEASE) {
        // Create file
        std::ofstream sceneFile("scene.txt");

        // Save data to file
        for (cMeshObject* pCM : ::g_pMeshesToDraw) {
            sceneFile << pCM->meshFileName << "\n";
            sceneFile << pCM->position.x << "," << pCM->position.y << "," << pCM->position.z << "\n";
            sceneFile << pCM->orientation.x << "," << pCM->orientation.y << "," << pCM->orientation.z << "\n";
            sceneFile << pCM->scale << "\n";
            sceneFile << pCM->colourRGB.r << "," << pCM->colourRGB.b << "," << pCM->colourRGB.g << "\n";
            sceneFile << pCM->bOverrideVertexModelColour << "\n";
            sceneFile << pCM->bIsWireframe << "\n";
            sceneFile << pCM->bIsVisible << "\n\n";
        }
        // Close the file
        sceneFile.close();
        std::cout << "Scene saved";
    }

    // Load scene
    if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
        std::ifstream sceneFile("scene.txt");
        std::string meshData;

        g_pMeshesToDraw.clear();

        while (std::getline(sceneFile, meshData)) {
            // Skip empty line
            if (meshData.empty()) {continue;}

            // New mesh
            cMeshObject* pCM = new cMeshObject();

            // File name
            pCM->meshFileName = meshData;

            // Position
            std::getline(sceneFile, meshData);
            std::stringstream meshPos(meshData);
            meshPos >> pCM->position.x;
            meshPos.ignore(1);
            meshPos >> pCM->position.y;
            meshPos.ignore(1);
            meshPos >> pCM->position.z;

            // Orientation
            std::getline(sceneFile, meshData);
            std::stringstream meshOri(meshData);
            meshOri >> pCM->orientation.x;
            meshOri.ignore(1);
            meshOri >> pCM->orientation.y;
            meshOri.ignore(1);
            meshOri >> pCM->orientation.z;

            // Scale
            std::getline(sceneFile, meshData);
            pCM->scale = std::stof(meshData);

            // Colour
            std::getline(sceneFile, meshData);
            std::stringstream meshCol(meshData);
            meshCol >> pCM->colourRGB.r;
            meshCol.ignore(1);
            meshCol >> pCM->colourRGB.b;
            meshCol.ignore(1);
            meshCol >> pCM->colourRGB.g;

            // Override colour
            std::getline(sceneFile, meshData);
            pCM->bOverrideVertexModelColour = std::stoi(meshData);

            // Wireframe
            std::getline(sceneFile, meshData);
            pCM->bIsWireframe = std::stoi(meshData);

            // Visible
            std::getline(sceneFile, meshData);
            pCM->bIsVisible = std::stoi(meshData);

            // Skip
            std::getline(sceneFile, meshData);

            // Push to array
            g_pMeshesToDraw.push_back(pCM);
        }
        // Close the file
        sceneFile.close();
        std::cout << "Scene Loaded";
    }

	if (!isAnyModDown(mods)) {
	    if (key == GLFW_KEY_A) {
	    	::g_cameraEye.x += camera_speed;
	    }

    	if (key == GLFW_KEY_D) {
    		::g_cameraEye.x -= camera_speed;
    	}

    	if (key == GLFW_KEY_W) {
    		::g_cameraEye.z += camera_speed;
    	}

    	if (key == GLFW_KEY_S) {
    		::g_cameraEye.z -= camera_speed;
    	}

    	if (key == GLFW_KEY_Q) {
    		::g_cameraEye.y += camera_speed;
    	}

    	if (key == GLFW_KEY_E) {
    		::g_cameraEye.y -= camera_speed;
    	}
    }
}
