#include "ScriptHelper.h"

std::vector<cScript> scriptObjects;

void LoadScripts() {

    std::string AssetsScriptFolder = "assets/scripts";

    std::cout << "Searching for script in assets folder...\n";


    try {
        for (const std::filesystem::directory_entry& file : std::filesystem::recursive_directory_iterator(AssetsScriptFolder)) {
            if (file.is_regular_file() && file.path().extension() == ".cpp") {
                cScript scriptObject;
                std::string scriptFile = file.path().generic_string();
                scriptObjects.emplace_back(std::move(scriptObject));
            }
        }
    } catch (const std::filesystem::filesystem_error& ex) {
        std::cerr << ex.what() << '\n';
    }
    std::cout << "Finished searching\n" << scriptObjects.size() << " Script found\n";
}

cScript* GetScriptByName(const std::string& scriptName) {
    for (unsigned int i = 0; i < scriptObjects.size(); i++){
        if (scriptObjects[i].GetName() == scriptName){
            return &scriptObjects[i];
        }
    }
    return nullptr;
}