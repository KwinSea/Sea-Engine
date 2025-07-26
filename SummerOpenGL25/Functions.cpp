// Functions that add or edit aspects of the scene

#include "Functions.h"

extern cLightManager* g_pLights;
extern std::vector<cMeshObject*> g_pMeshesToDraw;

glm::vec3 RGBify (int red, int green, int blue) {return glm::vec3(red / 255.0f, green / 255.0f, blue / 255.0f);}

void EditLight(
    unsigned int lightIndex,
    float lightState = 0.0,
    float lightType = 0.0,
    glm::vec4 lightPosition = glm::vec4(0.0, 0.0, 0.0, 0.0),
    glm::vec4 lightDiffuse = glm::vec4(0.0, 0.0, 0.0, 0.0)
) {
    // Light 1
    g_pLights->theLights[lightIndex].param2.x = 1.0f; // turn on
    g_pLights->theLights[lightIndex].param1.x = 0.0f; // light type = point light
    g_pLights->theLights[lightIndex].position = glm::vec4(-10.0f, -10.0f, -10.0f, 0.5f);
    g_pLights->theLights[lightIndex].diffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void AddMeshObject (
    std::string meshFileName,
    std::string uniqueName,
    glm::vec3 position,
    glm::vec3 orientation,
    float scale,
    glm::vec3 colourRGB,
    glm::vec3 specularHighLightRGB,
    float specularPower,

    bool bOverrideVertexModelColour,
    bool bIsWireframe,
    bool bIsVisible
) {
    cMeshObject* pNewObject = new cMeshObject();

    // Mesh name
    pNewObject->meshFileName = meshFileName;

    // Position
    pNewObject->position.x = position.x;
    pNewObject->position.y = position.y;
    pNewObject->position.z = position.z;

    // Orientation
    pNewObject->orientation.x = orientation.x;
    pNewObject->orientation.y = orientation.y;
    pNewObject->orientation.z = orientation.z;

    // Scale
    pNewObject->scale = scale;

    // Color
    pNewObject->colourRGB.r = colourRGB.r;
    pNewObject->colourRGB.g = colourRGB.g;
    pNewObject->colourRGB.b = colourRGB.b;

    // Specular highlight
    pNewObject->specularHighLightRGB.r = specularHighLightRGB.r;
    pNewObject->specularHighLightRGB.g = specularHighLightRGB.g;
    pNewObject->specularHighLightRGB.b = specularHighLightRGB.b;

    // Specular power
    pNewObject->specularPower = specularPower;

    // Attributes
    pNewObject->bOverrideVertexModelColour = bOverrideVertexModelColour; // Override color
    pNewObject->bIsVisible = bIsVisible; // is Visible
    pNewObject->bIsWireframe = bIsWireframe; // is Wireframe

    ::g_pMeshesToDraw.push_back(pNewObject);
}

void LoadMaze(std::string file_name) {
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

    std::ifstream file(file_name);

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
}

