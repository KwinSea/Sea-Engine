#pragma once

#include "DrawMesh.h"

void SetUpTexturesForObjectDraw(cMeshObject* pCurrentMesh, GLint program)
{
    //    GLuint Syd_TexID = ::g_pTheTextures->getTextureIDFromName("Sydney_Sweeney.bmp");
    //    GLuint Syd_TexID = ::g_pTheTextures->getTextureIDFromName("Texture_01_A.bmp");

    {   // Texture sampler00:
        GLuint texture00_ID = ::g_pTheTextures->getTextureIDFromName(pCurrentMesh->textureNames[0]);
        // Bind this texture to the sampler
        // Choose a texture unit... 
        // Unit: #0
        GLuint texture00Unit = 0;
        glActiveTexture(GL_TEXTURE0 + texture00Unit);	// GL_TEXTURE0 = 33984
        // Bind texture to tell texture unit what it's bound to
        glBindTexture(GL_TEXTURE_2D, texture00_ID);    // <-- 0 is the texture unit
        // At this point, texture "texture00_ID" is bound to texture unit #0

        // Get the sampler (shader) uniform location
        // uniform sampler2D textSampler2D_00;	
        GLint textSampler2D_00_UL = glGetUniformLocation(program, "textSampler2D_00");
        glUniform1i(textSampler2D_00_UL, texture00Unit);   // (Uniform ID, Texture Unit #)
    }

    {   // Texture sampler01:
        GLuint texture01_ID = ::g_pTheTextures->getTextureIDFromName(pCurrentMesh->textureNames[1]);
        // Bind this texture to the sampler
        // Choose a texture unit...
        // Unit: #1
        GLuint texture01Unit = 1;
        glActiveTexture(GL_TEXTURE0 + texture01Unit);	// GL_TEXTURE0 = 33984
        // Bind texture to tell texture unit what it's bound to
        glBindTexture(GL_TEXTURE_2D, texture01_ID);    // <-- 0 is the texture unit
        // At this point, texture "texture01_ID" is bound to texture unit #0

        // Get the sampler (shader) uniform location
        // uniform sampler2D textSampler2D_01;
        GLint textSampler2D_01_UL = glGetUniformLocation(program, "textSampler2D_01");
        glUniform1i(textSampler2D_01_UL, texture01Unit);   // (Uniform ID, Texture Unit #)
    }

    {   // Texture sampler02:
        GLuint texture02_ID = ::g_pTheTextures->getTextureIDFromName(pCurrentMesh->textureNames[2]);
        // Bind this texture to the sampler
        // Choose a texture unit...
        // Unit: #2
        GLuint texture02Unit = 2;
        glActiveTexture(GL_TEXTURE0 + texture02Unit);	// GL_TEXTURE0 = 33984
        // Bind texture to tell texture unit what it's bound to
        glBindTexture(GL_TEXTURE_2D, texture02_ID);    // <-- 0 is the texture unit
        // At this point, texture "texture02_ID" is bound to texture unit #0

        // Get the sampler (shader) uniform location
        // uniform sampler2D textSampler2D_02;
        GLint textSampler2D_02_UL = glGetUniformLocation(program, "textSampler2D_02");
        glUniform1i(textSampler2D_02_UL, texture02Unit);   // (Uniform ID, Texture Unit #)
    }

    {   // Texture sampler03:
        GLuint texture03_ID = ::g_pTheTextures->getTextureIDFromName(pCurrentMesh->textureNames[3]);
        // Bind this texture to the sampler
        // Choose a texture unit...
        // Unit: #3
        GLuint texture03Unit = 3;
        glActiveTexture(GL_TEXTURE0 + texture03Unit);	// GL_TEXTURE0 = 33984
        // Bind texture to tell texture unit what it's bound to
        glBindTexture(GL_TEXTURE_2D, texture03_ID);    // <-- 0 is the texture unit
        // At this point, texture "texture03_ID" is bound to texture unit #0

        // Get the sampler (shader) uniform location
        // uniform sampler2D textSampler2D_03;
        GLint textSampler2D_03_UL = glGetUniformLocation(program, "textSampler2D_03");
        glUniform1i(textSampler2D_03_UL, texture03Unit);   // (Uniform ID, Texture Unit #)
    }



    // Also the  mix ratios
    // uniform vec4 texMixRatios;	
    GLint texMixRatios_UL = glGetUniformLocation(program, "texMixRatios");
    glUniform4f(texMixRatios_UL,
        pCurrentMesh->textureMixRatio[0],
        pCurrentMesh->textureMixRatio[1],
        pCurrentMesh->textureMixRatio[2],
        pCurrentMesh->textureMixRatio[3]);


    // Set up masking texture
    //  uniform sampler2D sampMaskTexture01;
    {   // Texture sampler 8:
        GLuint maskTextureID = ::g_pTheTextures->getTextureIDFromName("MaskingTexture.bmp");
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, maskTextureID);

        GLint sampMaskTexture01_UL = glGetUniformLocation(program, "sampMaskTexture01");
        glUniform1i(sampMaskTexture01_UL, 8);
    }



    return;
}



void DrawMesh(cMeshObject* pCurrentMesh, GLint program)
{
    if (!pCurrentMesh->bIsVisible)
    {
        return;
    }

    glm::mat4 matModel;
    GLint Model_location = glGetUniformLocation(program, "mModel");

    GLint useOverrideColor_location = glGetUniformLocation(program, "bUseOverrideColor");
    GLint overrideColor_location = glGetUniformLocation(program, "colorOverride");

    if (pCurrentMesh->bOverrideVertexModelColour)
    {
        glUniform3f(overrideColor_location,
            pCurrentMesh->colourRGB.r,
            pCurrentMesh->colourRGB.g,
            pCurrentMesh->colourRGB.b);

        glUniform1f(useOverrideColor_location, GL_TRUE); // 1.0f

    }
    else
    {
        glUniform1f(useOverrideColor_location, GL_FALSE);
    }

    // If true, DON'T use the textures
    // uniform bool bUseVertexColourNotTexture;	
    GLint bUseVertexColourNotTexture_UL = glGetUniformLocation(program, "bUseVertexColourNotTexture");
    if (pCurrentMesh->bDontUseTextures)
    {
        glUniform1f(bUseVertexColourNotTexture_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bUseVertexColourNotTexture_UL, (GLfloat)GL_FALSE);
    }

    GLint bDoNotLight_UL = glGetUniformLocation(program, "bDoNotLight");
    if (pCurrentMesh->bDoNotLight)
    {
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_FALSE);
    }

    GLint bUseMaskingTexture_UL = glGetUniformLocation(program, "bUseMaskingTexture");
    if (pCurrentMesh->bUseMaskingTexture)
    {
        glUniform1f(bUseMaskingTexture_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bUseMaskingTexture_UL, (GLfloat)GL_FALSE);
    }

    GLint bAddReflect_UL = glGetUniformLocation(program, "bAddReflect");
    if (pCurrentMesh->bAddReflect)
    {
        glUniform1f(bAddReflect_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bAddReflect_UL, (GLfloat)GL_FALSE);
    }

    GLint bAddRefract_UL = glGetUniformLocation(program, "bAddRefract");
    if (pCurrentMesh->bAddRefract)
    {
        glUniform1f(bAddRefract_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bAddRefract_UL, (GLfloat)GL_FALSE);
    }

    GLint reflectionStrength_UL = glGetUniformLocation(program, "reflectionStrength");
    glUniform1f(reflectionStrength_UL, pCurrentMesh->reflectionStrength);

    GLint refractionStrength_UL = glGetUniformLocation(program, "refractionStrength");
    glUniform1f(refractionStrength_UL, pCurrentMesh->refractionStrength);

    // Copy over the transparency
    // uniform float alphaTransparency;
    GLint alphaTransparency_UL
        = glGetUniformLocation(program, "alphaTransparency");
    // Set it
    glUniform1f(alphaTransparency_UL, pCurrentMesh->opacityAlpha);

    GLint ambientLight_UL = glGetUniformLocation(program, "ambientLight");
    glUniform3f(ambientLight_UL, ambientLight.r, ambientLight.g, ambientLight.b);

    // Set the specular value
    //uniform vec4 vertSpecular;
    GLint vertSpecular_UL = glGetUniformLocation(program, "vertSpecular");
    // Copy the object specular to the shader
    glUniform4f(vertSpecular_UL,
        pCurrentMesh->specularHighLightRGB.r,
        pCurrentMesh->specularHighLightRGB.g,
        pCurrentMesh->specularHighLightRGB.b,
        pCurrentMesh->specularPower);




    //         mat4x4_identity(m);
    matModel = glm::mat4(1.0f);

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), pCurrentMesh->position);

    //mat4x4_rotate_Z(m, m, (float) glfwGetTime());
    glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
        glm::radians(pCurrentMesh->orientation.x),
        glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
        glm::radians(pCurrentMesh->orientation.y),
        glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
        glm::radians(pCurrentMesh->orientation.z),
        glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 scaleX = glm::scale(glm::mat4(1.0f),
    glm::vec3(pCurrentMesh->scale.x, 1.0f, 1.0f));

    glm::mat4 scaleY = glm::scale(glm::mat4(1.0f),
        glm::vec3(1.0f, pCurrentMesh->scale.y, 1.0f));

    glm::mat4 scaleZ = glm::scale(glm::mat4(1.0f),
        glm::vec3(1.0f, 1.0f, pCurrentMesh->scale.z));

    matModel = matModel * translation * rotateX * rotateY * rotateZ * scaleX * scaleY * scaleZ;


    //m = m * rotateZ;

    //mat4x4_mul(mvp, p, m);
    //mvp = matProj * matView * matModel;

    if (pCurrentMesh->bIsWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }


    //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    glUniformMatrix4fv(Model_location, 1, GL_FALSE, glm::value_ptr(matModel));

    GLint mModelIt_location = glGetUniformLocation(program, "mModel_InverseTranpose");

    // gets rid of any translation (movement) and scaling. leaves only roation
    glm::mat4 matModelIt = glm::inverse(glm::transpose(matModel));
    glUniformMatrix4fv(mModelIt_location, 1, GL_FALSE, glm::value_ptr(matModelIt));



    SetUpTexturesForObjectDraw(pCurrentMesh, program);


    // 


    //glDrawArrays(GL_TRIANGLES, 0, g_NumVerticiesToDraw);
    sModelDrawInfo modelToDraw;

    if (::pTheMeshManager->FindDrawInfoByModelName(pCurrentMesh->meshFileName,
        modelToDraw))
    {
        glBindVertexArray(modelToDraw.VAO_ID);
        glDrawElements(GL_TRIANGLES, modelToDraw.numberOfIndices,
            GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
    }


    //    // Any child meshes?
    //    for (cMeshObject* pChildMesh : pCurrentMesh->vec_pChildObjects)
    //    {
    //        DrawMesh(pChildMesh);
    //    }

    return;
}
