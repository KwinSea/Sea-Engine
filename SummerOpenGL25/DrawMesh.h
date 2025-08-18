#include "GlobalOpenGL.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include "cMeshObject.h"
#include "cBasicTextureManager/cBasicTextureManager.h"
#include "cVAOManager/cVAOManager.h"

extern cBasicTextureManager* g_pTheTextures;
extern cVAOManager* pTheMeshManager;

void SetUpTexturesForObjectDraw(cMeshObject* pCurrentMesh, GLint program);

void DrawMesh(cMeshObject* pCurrentMesh, GLint program);