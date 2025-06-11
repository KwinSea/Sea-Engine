#include "GlobalOpenGL.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include "cMeshObject.h"

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
    }

    if (key == GLFW_KEY_K) {
        for (cMeshObject* pCM : ::g_pMeshesToDraw) {
	        
        }
    }

    if (key == GLFW_KEY_L) {
        unsigned int meshesToDraw;

        for (int index = 0; index != meshesToDraw; index++) {
	        
        }
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
