#include "LoadModelsAndTextures.h"


cVAOManager* pTheMeshManager = NULL;

void LoadFilesIntoVAOManager(GLuint program)
{

    ::pTheMeshManager = new cVAOManager();

    sModelDrawInfo dolphinMeshInfo;
    if (! pTheMeshManager->LoadTheModel_IntoDrawInfoObject("assets/models/dolphin_xyz_n_rgba_UV.ply",
                dolphinMeshInfo, 1.0f))
    {
        std::cout << "Dolphin NOT loaded into VAO!" << std::endl;
    }
    else
    {
        // Dolphin model loaded into the dolphinMeshInfo

        // Overwrite the UV coords with the spherical coordinates
        pTheMeshManager->GenTextureCoordsSpherical(dolphinMeshInfo,
                                cVAOManager::enumTEXCOORDBIAS::POSITIVE_X,
                    	        cVAOManager::enumTEXCOORDBIAS::POSITIVE_Y, 
                                true,   // Used the normals from the model
                                1.0f,   // 1.0 = UVs go from 0.0 to 1.0
                                true); // Can't remember what I was doing here...
        
        // Now load it into the VAO
        pTheMeshManager->LoadModelDrawInfo_IntoVAO(dolphinMeshInfo, program);
    }

    sModelDrawInfo waterMeshInfo;
    if (! pTheMeshManager->LoadTheModel_IntoDrawInfoObject("assets/models/water.ply",
                waterMeshInfo, 1.0f))
    {
        std::cout << "Water NOT loaded into VAO!" << std::endl;
    }
    else
    {
        // Dolphin model loaded into the dolphinMeshInfo

        // Overwrite the UV coords with the spherical coordinates
        pTheMeshManager->GenTextureCoordsSpherical(waterMeshInfo,
                                cVAOManager::enumTEXCOORDBIAS::POSITIVE_X,
                                cVAOManager::enumTEXCOORDBIAS::POSITIVE_Y,
                                true,   // Used the normals from the model
                                1.0f,   // 1.0 = UVs go from 0.0 to 1.0
                                true); // Can't remember what I was doing here...

        // Now load it into the VAO
        pTheMeshManager->LoadModelDrawInfo_IntoVAO(waterMeshInfo, program);
    }

    sModelDrawInfo teaPotDrawInfo;
    if (!pTheMeshManager->LoadTheModel_IntoDrawInfoObject("assets/models/Utah_Teapot_xyz_n_rgba_UV.ply",
        teaPotDrawInfo, 1.0f))
    {
        std::cout << "Teapot NOT loaded!" << std::endl;
    }
    else
    {
        // Dolphin model loaded into the dolphinMeshInfo

        // Overwrite the UV coords with the spherical coordinates
        pTheMeshManager->GenTextureCoordsSpherical(teaPotDrawInfo,
            cVAOManager::enumTEXCOORDBIAS::POSITIVE_X,
            cVAOManager::enumTEXCOORDBIAS::POSITIVE_Y,
            true,   // Used the normals from the model
            1.0f,   // 1.0 = UVs go from 0.0 to 1.0
            true); // Can't remember what I was doing here...

        // Now load it into the VAO
        pTheMeshManager->LoadModelDrawInfo_IntoVAO(teaPotDrawInfo, program);
    }

    std::string AssetsModelFolder = "assets/models";
    std::vector<std::string> modelFiles;

    std::cout << "Searching for models in assets folder...\n";
    std::vector<sModelDrawInfo> meshObjects;

    try {
        for (const std::filesystem::directory_entry& file : std::filesystem::recursive_directory_iterator(AssetsModelFolder)) {
            if (file.is_regular_file() && file.path().extension() == ".ply") {
                sModelDrawInfo meshObject;
                std::string meshFile = file.path().generic_string();
                if (!::pTheMeshManager->LoadModelIntoVAO(meshFile, meshObject, program, 1.0f)) {
                    std::cout << "Model at: " << meshFile << " not loaded into VAO!" << std::endl;
                } else {
                    meshObjects.push_back(meshObject);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& ex) {
        std::cerr << ex.what() << '\n';
    }
    std::cout << "Finished searching\n" << meshObjects.size() << " Mesh found\n";

    return;
}

void LoadTexturesIntoTextureManager(cBasicTextureManager* pTheTextureManager)
{
    //std::string posX_fileName, std::string negX_fileName,
    //std::string posY_fileName, std::string negY_fileName,
    //std::string posZ_fileName, std::string negZ_fileName,
    pTheTextureManager->SetBasePath("assets/textures/CubeMaps");

    std::string errorMessage = "";
    if (!pTheTextureManager->CreateCubeTextureFromBMPFiles("SunnyDay",
        "TropicalSunnyDayRight2048.bmp", "TropicalSunnyDayLeft2048.bmp",
        "TropicalSunnyDayDown2048.bmp", "TropicalSunnyDayUp2048.bmp",
        "TropicalSunnyDayBack2048.bmp", "TropicalSunnyDayFront2048.bmp",
        true, errorMessage))
    {
        std::cout << "Didn't load sunny day texture because: " << errorMessage << std::endl;
    }

    if (!pTheTextureManager->CreateCubeTextureFromBMPFiles("Space",
        "SpaceBox_right1_posX.bmp", "SpaceBox_left2_negX.bmp",
        "SpaceBox_top3_posY.bmp", "SpaceBox_bottom4_negY.bmp",
        "SpaceBox_front5_posZ.bmp", "SpaceBox_back6_negZ.bmp",
        true, errorMessage))
    {
        std::cout << "Didn't load Space texture because: " << errorMessage << std::endl;
    }

    std::string AssetsTextureFolder = "assets/textures";
    int loadedCount = 0;

    try {
        for (const auto& file : std::filesystem::recursive_directory_iterator(AssetsTextureFolder)) {
            if (file.is_regular_file() && file.path().extension() == ".bmp") {
                std::string texFile = file.path().filename().string();
                if (pTheTextureManager->Create2DTextureFromBMPFile(file.path().filename().string(), file.path().generic_string(), true)) {
                    ++loadedCount;
                } else {
                    std::cout << "Failed to load " << texFile << std::endl;
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& ex) {
        std::cerr << ex.what() << '\n';
    }

    std::cout << "Finished searching\n" << loadedCount << " textures loaded\n" << std::endl;
    return;
}

void LoadModelsIntoScene() {
    // This is being used for the skybox
    cMeshObject* pSkyBoxMesh = new cMeshObject();
    pSkyBoxMesh->uniqueName = "skybox_mesh";
    pSkyBoxMesh->meshFileName = "assets/models/Isoshphere_smooth_inverted_normals_xyz_n_rgba_uv.ply";
    pSkyBoxMesh->scale = glm::vec3(1000.0f);
    // We AREN'T going to use these...
    pSkyBoxMesh->textureNames[0] = "Sydney_Sweeney.bmp";
    pSkyBoxMesh->textureMixRatio[0] = 1.0f;

    pSkyBoxMesh->bDoNotLight = true;
    // Will add it but it's invisible
    pSkyBoxMesh->bIsVisible = false;
    ::g_pMeshesToDraw.push_back(pSkyBoxMesh);

    return;
}

