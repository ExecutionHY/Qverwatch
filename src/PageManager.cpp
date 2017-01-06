//
//  PageManager.cpp
//  OpenGL
//
//  Created by Execution on 06/01/2017.
//  Copyright © 2017 Execution. All rights reserved.
//

#include "PageManager.hpp"

GLFWwindow* window;
int windowWidth = 800;
int windowHeight = 600;

GLuint DepthTexture;
GLuint DepthFrameBuffer;
GLuint quad_vertexbuffer;

GLuint VertexArrayID;
GLuint QuadProgramID;
GLuint QuadTextureID;
GLuint DepthProgramID;
GLuint DepthMVPMatrixID;
GLuint DepthBiasID;
GLuint ShadowMapID;

GLuint programID;
GLuint MVPMatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint ModelView3x3MatrixID;

GLuint DiffuseTextureID;
GLuint NormalTextureID;
GLuint SpecularTextureID;
GLuint SurfaceID;
GLuint TextureID;

GLuint LightID;
GLuint LightInvDirID;

GLuint HomePageTexture;
GLuint DiffuseTexture[1000];
GLuint NormalTexture[1000];
GLuint SpecularTexture[1000];

void initGL() {
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        exit(-1);
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( windowWidth, windowHeight, "playground", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    
    printf("%d %d\n", windowWidth, windowHeight);
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    printf("%d %d\n", windowWidth, windowHeight);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        exit(-1);
    }
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, windowWidth/2, windowHeight/2);
    
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
    
    
    
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    
    // ***************** For shadow *****************
    
    // the depth program for shader
    DepthProgramID = LoadShaders("DepthRTT.vertexshader", "DepthRTT.fragmentshader");
    DepthMVPMatrixID = glGetUniformLocation(DepthProgramID, "depthMVP");
    
    // Create a depth texture
    glGenTextures(1, &DepthTexture);
    glBindTexture(GL_TEXTURE_2D, DepthTexture);
    // Allocate storage for the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
                 DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    // Set the default filtering modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Set up depth comparison mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    // Set up wrapping modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    DepthFrameBuffer = 0;
    // Create FBO to reader depth into
    glGenFramebuffers(1, &DepthFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, DepthFrameBuffer);
    // Attach the depth texure to it;
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthTexture, 0);
    // Disable color rendering as there are no color attachments
    glDrawBuffer(GL_NONE);
    
    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        exit(-2);
    
    // The quad's FBO. Used only for visualizing the shadowmap.
    static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };
    
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    
    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "Passthrough.vertexshader", "SimpleTexture.fragmentshader" );
    
    // Create and compile our GLSL program from the shaders
    QuadProgramID = LoadShaders( "Passthrough.vertexshader", "SimpleTexture.fragmentshader" );
    QuadTextureID = glGetUniformLocation(QuadProgramID, "quadTexture");
    
    // Get a handle for uniform
    TextureID = glGetUniformLocation(QuadProgramID, "myTextureSampler");
    
    // Create and compile our GLSL program from the shaders
    //programID = LoadShaders( "ShadowMapping.vertexshader", "ShadowMapping.fragmentshader" );
    programID = LoadShaders( "MainAdvanced.vertexshader", "MainAdvanced.fragmentshader" );
    
    // Get a handle for our "myTextureSampler" uniform
    DiffuseTextureID  = glGetUniformLocation(programID, "DiffuseTextureSampler");
    NormalTextureID  = glGetUniformLocation(programID, "NormalTextureSampler");
    SpecularTextureID  = glGetUniformLocation(programID, "SpecularTextureSampler");
    
    // Get a handle for our "MVP" uniform
    MVPMatrixID = glGetUniformLocation(programID, "MVP");
    ViewMatrixID = glGetUniformLocation(programID, "V");
    ModelMatrixID = glGetUniformLocation(programID, "M");
    ModelView3x3MatrixID = glGetUniformLocation(programID, "MV3x3");
    
    DepthBiasID = glGetUniformLocation(programID, "DepthBiasMVP");
    ShadowMapID = glGetUniformLocation(programID, "shadowMap");
    SurfaceID = glGetUniformLocation(programID, "Surface");
    
    
    // Get a handle for our "LightPosition" uniform --- for spot light
    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    // for directional light
    LightInvDirID = glGetUniformLocation(programID, "LightInvDirection_worldspace");
    
    // ???? init the obj ???
    memset(DiffuseTexture, 0, sizeof(DiffuseTexture));
    memset(NormalTexture, 0, sizeof(NormalTexture));
    memset(SpecularTexture, 0, sizeof(SpecularTexture));
}

void loadingPage() {
    // ***** Loading Page
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glViewport(0,0,windowWidth,windowHeight);
    
    // Use our shader
    glUseProgram(QuadProgramID);
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, HomePageTexture);
    // Set our "renderedTexture" sampler to user Texture Unit 0
    glUniform1i(TextureID, 0);
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glVertexAttribPointer(
                          0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    // Draw the triangle !
    // You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
    glDisableVertexAttribArray(0);
    char loading[64] = "LOADING FILES...";
    printText2D(loading, windowWidth/2*0.2, windowHeight/2*0.1, 30);
    // Swap buffers
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}


void selectingPage() {
    // ***** Selecting Page
    int pos = 0;
    bool downReleased = true, upReleased = true;
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render to the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glViewport(0,0,windowWidth,windowHeight);
        
        // Use our shader
        glUseProgram(QuadProgramID);
        
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, HomePageTexture);
        // Set our "renderedTexture" sampler to user Texture Unit 0
        glUniform1i(TextureID, 0);
        
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glVertexAttribPointer(
                              0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        // Draw the triangle !
        // You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
        glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
        glDisableVertexAttribArray(0);
        
        // **************************** Text 2D *******************************
        
        char ready[64] = "PRESS ENTER TO START";
        printText2D(ready, windowWidth/2*0.2, windowHeight/2*0.1, 25);
        
        char text1[64] = "hanamura.wav";
        printText2D(text1, windowWidth/2*0.1, windowHeight/2*0.8, 20);
        
        char text2[64] = "kkk.wav";
        printText2D(text2, windowWidth/2*0.1, windowHeight/2*0.75, 20);
        
        char arrow[64] = ">";
        printText2D(arrow, windowWidth/2*0.07, windowHeight/2*(0.8 - pos*0.05), 20);
        
        if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_RELEASE)
            downReleased = true;
        
        if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS) {
            if (downReleased) {
                pos = (pos + 1) % 2;
                downReleased = false;
            }
        }
        
        if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_RELEASE)
            upReleased = true;
        
        if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS) {
            if (upReleased) {
                pos = (pos + 1) % 2;
                upReleased = false;
            }
        }
        
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    } while ( glfwGetKey(window, GLFW_KEY_ENTER ) != GLFW_PRESS );
    
}
