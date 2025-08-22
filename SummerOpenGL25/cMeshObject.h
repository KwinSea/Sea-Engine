#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "cScript.h"

// CRC: Class, Responsibility, Collboration
// should usually be a noun (and just one thing)
class cMeshObject {

    std::vector<cScript*> scripts;

public:
    cMeshObject();
    std::string meshFileName;
    std::string uniqueName;
    glm::vec3 position;
    glm::vec3 orientation; // eurler axis
    //glm::vec3 scaleXYZ;
    glm::vec3 scale; // uniform scale
    glm::vec3 colourRGB; // diffuse// 0.0f to 1.0f (invisible to solid)
    float opacityAlpha;

    glm::vec3 specularHighLightRGB;
    float specularPower;
    float reflectionStrength;
    float refractionStrength;

    bool bOverrideVertexModelColour;
    bool bIsWireframe;
    bool bIsVisible;

    bool bDoNotLight;
    bool bDontUseTextures;	// default = false;
    bool bUseMaskingTexture;
    bool bAddReflect;
    bool bAddRefract;

    static const unsigned int NUM_TEXTURES = 4;
    std::string textureNames[NUM_TEXTURES];
    float textureMixRatio[NUM_TEXTURES];

    void AttachScript(cScript* script);
    void DetachScript(cScript* script);
    void DetachScriptByName(const std::string& scriptName);
    cScript* GetScriptByName(const std::string& scriptName);
    void UpdateScripts(float deltaTime);
};
