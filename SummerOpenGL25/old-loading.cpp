//struct sVertex
//{
//    float x;
//    float y;
//    float z;
//    float r;
//    float g;
//    float b;
//};

//sVertex* pVerticies = NULL;

//static const char* vertex_shader_text =
//"#version 110\n"
//"uniform mat4 MVP;\n"
//"attribute vec3 vCol;\n"
//"attribute vec3 vPos;\n"
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    gl_Position = MVP * vec4(vPos, 1.0);\n"
//"    color = vCol;\n"
//"}\n";
//
//static const char* fragment_shader_text =
//"#version 110\n"
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    gl_FragColor = vec4(color, 1.0);\n"
//"}\n";
// GLint mvp_location, vpos_location, vcol_location;

//    GLuint vertex_buffer;


//bool LoadPLY_XYZ_Format_With_Indicies(std::string filename)
//{
//    struct sPlyVertexXYZ
//    {
//        float x;
//        float y;
//        float z;
//    };
//
//    struct sPlyTriangle
//    {
//        int numVerticies;
//        int v0;
//        int v1;
//        int v2;
//    };
//
//    std::vector<sPlyVertexXYZ> vecVerticies;
//    std::vector<sPlyTriangle> vecTriangles;
//
//    std::ifstream theFile(filename.c_str());
//
//    if (!theFile.is_open())
//    {
//        return false;
//    }
//
//    std::string curWord;
//
//    while (theFile >> curWord)
//    {
//        if (curWord == "vertex")
//        {
//            break;
//        }
//    }
//
//    unsigned int numVerticies = 0;
//
//    theFile >> numVerticies;
//
//    while (theFile >> curWord)
//    {
//        if (curWord == "face")
//        {
//            break;
//        }
//    }
//
//    unsigned int numTriangles = 0;
//
//    theFile >> numTriangles;
//
//
//    while (theFile >> curWord)
//    {
//        if (curWord == "end_header")
//        {
//            break;
//        }
//    }
//
//    // start reading the verticies...
//
//    for (unsigned int count = 0; count < numVerticies; count++)
//    {
//        sPlyVertexXYZ curVert;
//        theFile >> curVert.x >> curVert.y >> curVert.z;
//        vecVerticies.push_back(curVert);
//    }
//
//    for (unsigned int count = 0; count < numTriangles; count++)
//    {
//        sPlyTriangle curTri;
//        theFile >> curTri.numVerticies >> curTri.v0 >> curTri.v1 >> curTri.v2;
//        vecTriangles.push_back(curTri);
//    }
//
//    int numVerticiesToDraw = numTriangles * 3;
//    pVerticies = new sVertex[numVerticiesToDraw];
//
//    unsigned int vertIndex = 0;
//
//    for (unsigned int index = 0; index != numTriangles; index++)
//    {
//        sPlyTriangle curTri = vecTriangles[index];
//
//        sPlyVertexXYZ v0 = vecVerticies[curTri.v0];
//        sPlyVertexXYZ v1 = vecVerticies[curTri.v1];
//        sPlyVertexXYZ v2 = vecVerticies[curTri.v2];
//
//
//        pVerticies[vertIndex].x = v0.x;
//        pVerticies[vertIndex].y = v0.y;
//        pVerticies[vertIndex].z = v0.z;
//        // colours 
//        pVerticies[vertIndex].r = 0.0f;
//        pVerticies[vertIndex].g = 1.0f;
//        pVerticies[vertIndex].b = 0.0f;
//
//        pVerticies[vertIndex + 1].x = v1.x;
//        pVerticies[vertIndex + 1].y = v1.y;
//        pVerticies[vertIndex + 1].z = v1.z;
//        // colours 
//        pVerticies[vertIndex + 1].r = 0.0f;
//        pVerticies[vertIndex + 1].g = 1.0f;
//        pVerticies[vertIndex + 1].b = 0.0f;
//
//        pVerticies[vertIndex + 2].x = v2.x;
//        pVerticies[vertIndex + 2].y = v2.y;
//        pVerticies[vertIndex + 2].z = v2.z;
//        // colours 
//        pVerticies[vertIndex + 2].r = 0.0f;
//        pVerticies[vertIndex + 2].g = 1.0f;
//        pVerticies[vertIndex + 2].b = 0.0f;
//
//        vertIndex += 3;
//    }
//
//    ::g_NumVerticiesToDraw = numVerticiesToDraw;
//    ::g_SizeOfVertexArrayInBytes = numVerticiesToDraw * sizeof(sVertex);
//
//    theFile.close();
//
//    return true;
//}
//
//bool LoadPLY_XYZ_Format(std::string filename)
//{
//    struct sPlyVertexXYZ
//    {
//        float x;
//        float y;
//        float z;
//    };
//
//    struct sPlyTriangle
//    {
//        int numVerticies;
//        int v0;
//        int v1;
//        int v2;
//    };
//
//    std::vector<sPlyVertexXYZ> vecVerticies;
//    std::vector<sPlyTriangle> vecTriangles;
//
//    std::ifstream theFile(filename.c_str());
//
//    if (!theFile.is_open())
//    {
//        return false;
//    }
//
//    std::string curWord;
//
//    while (theFile >> curWord)
//    {
//        if (curWord == "vertex")
//        {
//            break;
//        }
//    }
//
//    unsigned int numVerticies = 0;
//
//    theFile >> numVerticies;
//
//    while (theFile >> curWord)
//    {
//        if (curWord == "face")
//        {
//            break;
//        }
//    }
//
//    unsigned int numTriangles = 0;
//
//    theFile >> numTriangles;
//
//    while (theFile >> curWord)
//    {
//        if (curWord == "end_header")
//        {
//            break;
//        }
//    }
//
//    // start reading the verticies...
//
//    for (unsigned int count = 0; count < numVerticies; count++)
//    {
//        sPlyVertexXYZ curVert;
//        theFile >> curVert.x >> curVert.y >> curVert.z;
//        vecVerticies.push_back(curVert);
//    }
//
//    for (unsigned int count = 0; count < numTriangles; count++)
//    {
//        sPlyTriangle curTri;
//        theFile >> curTri.numVerticies >> curTri.v0 >> curTri.v1 >> curTri.v2;
//        vecTriangles.push_back(curTri);
//    }
//
//    float scale = 0.1f;
//
//
//    pVerticies = new sVertex[numVerticies];
//    for (unsigned int index = 0; index != numVerticies; index++)
//    {
//        pVerticies[index].x = vecVerticies[index].x;
//        pVerticies[index].y = vecVerticies[index].y;
//        pVerticies[index].z = vecVerticies[index].z;
//
//        // colours 
//        pVerticies[index].r = 0.0f;
//        pVerticies[index].g = 1.0f;
//        pVerticies[index].b = 0.0f;
//    }
//
//    ::g_NumVerticiesToDraw = numVerticies;
//    ::g_SizeOfVertexArrayInBytes = numVerticies * sizeof(sVertex);
//
//    theFile.close();
//
//    return true;
//}
//int main()
//{
//	//GLuint vertex_shader;
//   //GLuint fragment_shader;
//    /*bool loaded = LoadPLY_XYZ_Format_With_Indicies("assets/models/cow.ply");
//
//    if (loaded)
//    {
//        std::cout << "cow is loaded" << std::endl;
//    }
//    else
//    {
//        std::cout << "ALL HOPE IS LOST!!!" << std::endl;
//        return -1;
//    }*/
//
//
//
//    // NOTE: OpenGL error checks have been omitted for brevity
//    /*glGenBuffers(1, &vertex_buffer);
//    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
//    glBufferData(GL_ARRAY_BUFFER, g_SizeOfVertexArrayInBytes, pVerticies, GL_STATIC_DRAW);*/
//
//    /* vertex_shader = glCreateShader(GL_VERTEX_SHADER);
// glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
// glCompileShader(vertex_shader);
//
// fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
// glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
// glCompileShader(fragment_shader);
// program = glCreateProgram();
//
// glAttachShader(program, vertex_shader);
// glAttachShader(program, fragment_shader);
// glLinkProgram(program);*/
//
// //vpos_location = glGetAttribLocation(program, "vPos");
////vcol_location = glGetAttribLocation(program, "vCol");
//
////glEnableVertexAttribArray(vpos_location);
////glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
////    sizeof(sVertex), (void*)0);
////glEnableVertexAttribArray(vcol_location);
////glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
////    sizeof(sVertex), (void*)(sizeof(float) * 3));
//
//}
   