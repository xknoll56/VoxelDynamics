
GLFWwindow* window;
void initApplication()
{

    /* Initialize the library */
    if (!glfwInit())
        return;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Voxel Dynamics", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(0);

    memset(keys, false, 1024 * sizeof(bool));
    memset(mouse, false, 32 * sizeof(bool));
    memset(keysDown, false, 1024 * sizeof(bool));
    memset(mouseDown, false, 32 * sizeof(bool));

    // Load OpenGL functions using GLAD
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_CULL_FACE);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);


    // Create and compile our GLSL program from the shaders
    //GLuint programInstancedID = LoadShaders("instanceVert.glsl", "instanceFrag.glsl");

    instancedShader.LoadShaders("Shaders/instanceVert.glsl", "Shaders/instanceFrag.glsl");

    instancedShader.use();
    instancedShader.insertUniformVector3("solidColor");
    instancedShader.setUniformVector3("solidColor", { 1,1,1 });

    shader.LoadShaders("Shaders/vert.glsl", "Shaders/frag.glsl");
    shader.insertUniformInt("colorMix");
    shader.setUniformFloat("colorMix", 0.0f);
    shader.insertUniformVector3("solidColor");

    wireShader.LoadShaders("Shaders/wireVert.glsl", "Shaders/wireFrag.glsl");
    wireShader.insertUniformVector3("solidColor");


    static const float vertexDataOriginCentered[] = {
        // Position             // Normal           // UV
        // Front face
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f,  0.0f, 0.5f,
         0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,  0.5f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f,  0.0f, 0.5f,
         0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f,  0.5f, 0.5f,
         0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,  0.5f, 0.0f,

         // Back face
         -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  1.0f,  0.0f, 0.0f,
          0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  1.0f,  0.5f, 0.0f,
         -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,  1.0f,  0.0f, 0.5f,
         -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,  1.0f,  0.0f, 0.5f,
          0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  1.0f,  0.5f, 0.0f,
          0.5f,  0.5f,  0.5f,    0.0f, 0.0f,  1.0f,  0.5f, 0.5f,

          // Right face
           0.5f, -0.5f, -0.5f,    1.0f, 0.0f,  0.0f,  0.5f, 0.0f,
           0.5f,  0.5f, -0.5f,    1.0f, 0.0f,  0.0f,  0.5f, 0.5f,
           0.5f,  0.5f,  0.5f,    1.0f, 0.0f,  0.0f,  1.0f, 0.5f,
           0.5f, -0.5f, -0.5f,    1.0f, 0.0f,  0.0f,  0.5f, 0.0f,
           0.5f,  0.5f,  0.5f,    1.0f, 0.0f,  0.0f,  1.0f, 0.5f,
           0.5f, -0.5f,  0.5f,    1.0f, 0.0f,  0.0f,  1.0f, 0.0f,

           // Left face
           -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f,  0.0f,  0.5f, 0.0f,
           -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f,  0.0f,  1.0f, 0.5f,
           -0.5f,  0.5f, -0.5f,   -1.0f, 0.0f,  0.0f,  0.5f, 0.5f,
           -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f,  0.0f,  0.5f, 0.0f,
           -0.5f, -0.5f,  0.5f,   -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
           -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f,  0.0f,  1.0f, 0.5f,

           // Top face
           -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,  0.0f,  0.5f, 0.5f,
            0.5f,  0.5f,  0.5f,    0.0f, 1.0f,  0.0f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,    0.0f, 1.0f,  0.0f,  1.0f, 0.5f,
           -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,  0.0f,  0.5f, 0.5f,
           -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,  0.0f,  0.5f, 1.0f,
            0.5f,  0.5f,  0.5f,    0.0f, 1.0f,  0.0f,  1.0f, 1.0f,

            // Bottom face
            -0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,  0.0f, 0.5f,
             0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,  0.5f, 0.5f,
             0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,  0.5f, 1.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,  0.0f, 0.5f,
             0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,  0.5f, 1.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
    };

    static const float vertexDataPositiveQuadrant[] = {
        // Position             // Normal           // UV
        // Front face
         0.0f,  0.0f,  0.0f,    0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
         0.0f,  1.0f,  0.0f,    0.0f, 0.0f, -1.0f,  0.0f, 0.5f,
         1.0f,  0.0f,  0.0f,    0.0f, 0.0f, -1.0f,  0.5f, 0.0f,
         0.0f,  1.0f,  0.0f,    0.0f, 0.0f, -1.0f,  0.0f, 0.5f,
         1.0f,  1.0f,  0.0f,    0.0f, 0.0f, -1.0f,  0.5f, 0.5f,
         1.0f,  0.0f,  0.0f,    0.0f, 0.0f, -1.0f,  0.5f, 0.0f,

         // Back face
         0.0f,  0.0f,  1.0f,    0.0f, 0.0f,  1.0f,  0.0f, 0.0f,
         1.0f,  0.0f,  1.0f,    0.0f, 0.0f,  1.0f,  0.5f, 0.0f,
         0.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,  0.0f, 0.5f,
         0.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,  0.0f, 0.5f,
         1.0f,  0.0f,  1.0f,    0.0f, 0.0f,  1.0f,  0.5f, 0.0f,
         1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,  0.5f, 0.5f,

         // Right face
         1.0f,  0.0f,  0.0f,    1.0f, 0.0f,  0.0f,  0.5f, 0.0f,
         1.0f,  1.0f,  0.0f,    1.0f, 0.0f,  0.0f,  0.5f, 0.5f,
         1.0f,  1.0f,  1.0f,    1.0f, 0.0f,  0.0f,  1.0f, 0.5f,
         1.0f,  0.0f,  0.0f,    1.0f, 0.0f,  0.0f,  0.5f, 0.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 0.0f,  0.0f,  1.0f, 0.5f,
         1.0f,  0.0f,  1.0f,    1.0f, 0.0f,  0.0f,  1.0f, 0.0f,

         // Left face
         0.0f,  0.0f,  0.0f,   -1.0f, 0.0f,  0.0f,  0.5f, 0.0f,
         0.0f,  1.0f,  1.0f,   -1.0f, 0.0f,  0.0f,  1.0f, 0.5f,
         0.0f,  1.0f,  0.0f,   -1.0f, 0.0f,  0.0f,  0.5f, 0.5f,
         0.0f,  0.0f,  0.0f,   -1.0f, 0.0f,  0.0f,  0.5f, 0.0f,
         0.0f,  0.0f,  1.0f,   -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
         0.0f,  1.0f,  1.0f,   -1.0f, 0.0f,  0.0f,  1.0f, 0.5f,

         // Top face
         0.0f,  1.0f,  0.0f,    0.0f, 1.0f,  0.0f,  0.5f, 0.5f,
         1.0f,  1.0f,  1.0f,    0.0f, 1.0f,  0.0f,  1.0f, 1.0f,
         1.0f,  1.0f,  0.0f,    0.0f, 1.0f,  0.0f,  1.0f, 0.5f,
         0.0f,  1.0f,  0.0f,    0.0f, 1.0f,  0.0f,  0.5f, 0.5f,
         0.0f,  1.0f,  1.0f,    0.0f, 1.0f,  0.0f,  0.5f, 1.0f,
         1.0f,  1.0f,  1.0f,    0.0f, 1.0f,  0.0f,  1.0f, 1.0f,

         // Bottom face
         0.0f,  0.0f,  0.0f,    0.0f, -1.0f, 0.0f,  0.0f, 0.5f,
         1.0f,  0.0f,  0.0f,    0.0f, -1.0f, 0.0f,  0.5f, 0.5f,
         1.0f,  0.0f,  1.0f,    0.0f, -1.0f, 0.0f,  0.5f, 1.0f,
         0.0f,  0.0f,  0.0f,    0.0f, -1.0f, 0.0f,  0.0f, 0.5f,
         1.0f,  0.0f,  1.0f,    0.0f, -1.0f, 0.0f,  0.5f, 1.0f,
         0.0f,  0.0f,  1.0f,    0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
    };

    float cubeWireVerts[] = {

        //back face
        -0.5f, -0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        //connectors
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, 0.5f,

        //front face
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f
    };

    vbOrigin.init((float*)vertexDataOriginCentered, 36);
    vbPositiveQuadrant.init((float*)vertexDataPositiveQuadrant, 36);
    vbWire.initLines(cubeWireVerts, 24);


    stbi_set_flip_vertically_on_load(true);

    texArr.initCheckersTexture(0);

    instancedShader.insertUniformInt("textureArray");
    instancedShader.setUniformInt("textureArray", 0);
    shader.insertUniformInt("textureArray");
    shader.setUniformInt("textureArray", 0);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    //VDGrid chunk = deserializeChunk("Data/test.vd");

    instancedShader.insertUniformMatrix4("mvp");
    shader.insertUniformMatrix4("mvp");
    shader.insertUniformInt("textInd");
    shader.setUniformInt("textInd", 0);
    wireShader.insertUniformMatrix4("mvp");
    proj = VDPerspective(aspect, PI * 0.25f, 0.1f, 1000.0f);
}

extern float dYaw;
extern float dPitch;

void runApplication(Scene* pScene)
{
    pScene->init();
    double elapsedTime = glfwGetTime();
    bool mouseLocked = false;
    double mouseLockX, mouseLockY;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double newTime = glfwGetTime();
        float dt = (float)(newTime - elapsedTime);
        elapsedTime = newTime;

        dYaw = 0.0f;
        dPitch = 0.0f;
        float mouseSensitivity = 1.0f;
        if (mouse[GLFW_MOUSE_BUTTON_2])
        {
            if (!mouseLocked)
            {
                mouseLocked = true;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                mouseLockX = mouseX;
                mouseLockY = mouseY;
            }
            dYaw = (mouseX - mouseLockX) * mouseSensitivity;
            dPitch = (mouseY - mouseLockY) * mouseSensitivity;
            glfwSetCursorPos(window, mouseLockX, mouseLockY);
        }
        else
        {
            if (mouseLocked)
            {
                mouseLocked = false;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
        //camera.handleInputs(dt, -dYaw, -dPitch);
        pScene->update(dt);
        viewProjection = pScene->camera.view * proj;


        //drawInstanceBuffer(scene.ib, texArr);
        pScene->draw(dt);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        memset(keysDown, false, 1024 * sizeof(bool));
        memset(mouseDown, false, 32 * sizeof(bool));

    }

    glfwTerminate();
}
