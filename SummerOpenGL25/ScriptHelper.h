#pragma once
#include "GlobalOpenGL.h"
#include <iostream>
#include "cScript.h"

void LoadScripts();

cScript* GetScriptByName(const std::string& scriptName);

