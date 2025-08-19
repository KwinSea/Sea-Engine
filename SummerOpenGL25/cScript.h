#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>

class cMeshObject;

class cScript {
public:
    cScript() {
        parentObject = nullptr;
    }

    // Called when Created
    virtual void OnCreation() {}

    // Called every frame
    virtual void OnUpdate(float deltaTime) {}

    void CreateScript(std::string script_name) {
        std::filesystem::path scriptsDir("assets/scripts");

        std::string scriptPath = "assets/scripts/" + script_name + ".cpp";
        std::ofstream scriptFile(scriptPath);

        LoadScriptStarterCode(script_name);

        scriptFile << scriptStarterCode.str();
        scriptFile.close();
    }

    void SetParent(cMeshObject* parent) {
        parentObject = parent;
    }

    void SetName(const std::string& name) {
        scriptName = name;
    }

    cMeshObject* GetParent() {
        return parentObject;
    }

    const std::string& GetName() const {
        return this->scriptName;
    }

protected:
    cMeshObject* parentObject;
    std::string scriptName;

    std::stringstream scriptStarterCode;

    void LoadScriptStarterCode(const std::string& script_name) {
        scriptStarterCode
        << "#pragma once\n"
        << "#include \"cScript.h\"\n\n"<< "class " << script_name << " : public cScript {\n\n"
        << "private:\n"
        << "    // Add Private variables here\n"
        << "public:\n"
        << "    // Add Public variables here\n"
        << "protected:\n"
        << "    // Add Protected variables here\n\n"
        << "public:\n"
        << "    void OnCreation() override {\n"
        << "        // Add your Creation logic here\n"
        << "    }\n"
        << "    void OnUpdate(float deltaTime) override {\n"
        << "        // Add your Update logic here\n"
        << "    }\n"
        << "};\n";
    }
};
