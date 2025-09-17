#include "GlobalOpenGL.h"   // For OpenGL types like GLint, etc.

#include <vector>
#include <fstream>
#include <iostream>
#include "cMeshObject.h"
#include "cVAOManager/cVAOManager.h"
#include "Functions.h"
#include "cBasicTextureManager/cBasicTextureManager.h"

extern std::vector<cMeshObject*> g_pMeshesToDraw;

void LoadFilesIntoVAOManager(GLuint program);

void LoadTexturesIntoTextureManager(cBasicTextureManager* pTheTextureManager);

void LoadModelsIntoScene();