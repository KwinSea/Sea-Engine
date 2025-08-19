#include "cMeshObject.h"

cMeshObject::cMeshObject()
{
	this->meshFileName = "";
	this->uniqueName = "";
	this->position = glm::vec3(0.0f);
	this->orientation = glm::vec3(0.0f);

	this->scale = 1.0f;
	this->colourRGB = glm::vec3(1.0f);
	this->opacityAlpha = 1.0f;

	this->specularHighLightRGB = glm::vec3(1.0f);
	this->specularPower = 1.0f;

	this->bOverrideVertexModelColour = false;
	this->bIsWireframe = false;
	this->bIsVisible = true;

	this->bDoNotLight = false;
	this->bDontUseTextures = false;
	this->bUseMaskingTexture = false;
	this->bAddReflectAndRefract = false;

	this->textureMixRatio[0] = 1.0f;
	this->textureMixRatio[1] = 0.0f;
	this->textureMixRatio[2] = 0.0f;
	this->textureMixRatio[3] = 0.0f;

	this->scripts.clear();
}

void cMeshObject::AttachScript(cScript* script) {
	if (script) {
		script->SetParent(this);
		scripts.push_back(script);
		script->OnCreation();
	}
}

void cMeshObject::DetachScript(cScript* script) {
	if (!script) {
		return;
	}

	for (unsigned int i = 0; i < scripts.size();) {
		if (scripts[i] == script) {
			scripts.erase(scripts.begin() + i);
		} else {
			++i;
		}
	}
}

cScript* cMeshObject::GetScriptByName(const std::string& scriptName) {
	for (unsigned int i = 0; i < scripts.size(); i++){
		if (scripts[i]->GetName() == scriptName){
			return scripts[i];
		}
	}
	return nullptr;
}

void cMeshObject::UpdateScripts(float deltaTime){
	for (unsigned int i = 0; i < scripts.size(); i++) {
		scripts[i]->OnUpdate(deltaTime);
	}
}
