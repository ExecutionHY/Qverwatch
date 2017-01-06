//
//  main.cpp
//  OpenGL
//
//  Created by Execution on 11/4/16.
//  Copyright © 2016 Execution. All rights reserved.
//

#include "macro.h"

GLFWwindow* window;
ALuint source;

int windowWidth = 800;
int windowHeight = 600;

mat4 ProjectionMatrix;
mat4 ViewMatrix;
mat4 ModelMatrix;
mat4 MVP;

GLuint programID;
GLuint MVPMatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint ModelView3x3MatrixID;

GLuint SurfaceID;
GLuint TextureID;

GLuint DiffuseTextureID;
GLuint NormalTextureID;
GLuint SpecularTextureID;

GLuint LightID;
GLuint LightInvDirID;

GLuint HomePageTexture;
GLuint DiffuseTexture[1000];
GLuint NormalTexture[1000];
GLuint SpecularTexture[1000];
extern OBJ obj[10000];
extern Object object[10000];
extern int objIndex, objIndex1, rcdIndex;

GLuint VertexArrayID;

// For shadow - depth texure
mat4 DepthProjectionMatrix;
mat4 DepthViewMatrix;
mat4 DepthModelMatrix;
mat4 DepthMVP;
mat4 BiasMatrix;
mat4 DepthBiasMVP;

GLuint QuadProgramID;
GLuint QuadTextureID;
GLuint DepthProgramID;
GLuint DepthMVPMatrixID;
GLuint DepthBiasID;
GLuint ShadowMapID;

GLuint DepthTexture;
GLuint DepthFrameBuffer;
GLuint quad_vertexbuffer;

FILE* fp = NULL;
ALCdevice* device;
ALCcontext* context;
ALuint buffer;
unsigned char* buf;

void initGL();
void initAL(char* filename);
void deleteAL();
void loadingPage();
void selectingPage();
int main( void )
{
    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    int fps = 0;

    
    initGL();
    HomePageTexture = loadBMP_custom("res/home.bmp");
    // Initialize our little text library with the Holstein font
    initText2D("res/Holstein.dds");
    loadingPage();
    
    // ***************************** resource loading *******************************
    
    //Texture[0] = loadBMP_custom("res/wall.bmp");
    DiffuseTexture[0] = loadDDS("res/diffuse.DDS");
    NormalTexture[0] = loadBMP_custom("res/normal.bmp");
    SpecularTexture[0] = loadDDS("res/specular.DDS");
 
    //loadMap("res/Decode/HANAMURA.owmapx");
    loadMap("res/car.owmapx");
    
    //initAL("res/Hanamura.wav");
    
    //selectingPage();
    
    //alSourcePlay(source);
    
    do{
        
        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);
        
        
        // Render to our framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, DepthFrameBuffer);
        glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
        
        // not using bias but draw back faces ..?
        glCullFace(GL_BACK);
        
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // Use our shader
        glUseProgram(DepthProgramID);
        
        vec3 lightInvDir = vec3(30, 30, -30);
        
        // Compute the MVP from the light's point of view
        DepthProjectionMatrix = ortho<float>(-100, 100, -100, 100, -100, 200);
        DepthViewMatrix = lookAt(lightInvDir, vec3(0, 0, 0), vec3(0, 1, 0));
        
        //wall.loadDepth();
        for (int i = 0; i < objIndex; i++) {
            if (object[i].isValid()) object[i].loadDepth();
        }
        for (int i = objIndex; i < rcdIndex; i++) {
            if (object[i].isValid()) object[i].loadDepth();
        }
        
        // *************************** Draw Objects ****************************
        
        // Render to the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowWidth, windowHeight);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(programID);
        
        
        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        ProjectionMatrix = getProjectionMatrix();
        ViewMatrix = getViewMatrix();
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
        
        glUniform3f(LightInvDirID, lightInvDir.x, lightInvDir.y, lightInvDir.z);
        // spot light
        //glUniform3f(LightID, lightpos.x, lightpos.y, lightpos.z);
        
        //wall.drawObject();
        for (int i = 0; i < objIndex; i++) {
            if (object[i].isValid()) object[i].drawObject();
        }
        for (int i = objIndex; i < rcdIndex; i++) {
            if (object[i].isValid()) object[i].drawObject();
        }
        
        
        // *************************** Draw 2D text ****************************
        glViewport(0,0,windowWidth, windowHeight);
        
        glDisable(GL_DEPTH_TEST);
        
        
        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
            fps = nbFrames;
            nbFrames = 0;
            lastTime += 1.0;
            printf("fps = %d\n", fps);
        }
        
        char text[256];
        sprintf(text, "TIME: %.2f s\nFPS: %d", glfwGetTime(), fps);
        printText2D(text, 10, 570, 15);
        
        char text2[256] = "|";
        printText2D(text2, 390, 270, 20);
        char text3[256] = "= =";
        printText2D(text3, 370, 290, 20);
        char text4[256] = "|";
        printText2D(text4, 390, 310, 20);
        
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(4);
        
        
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    // Cleanup VBO and shader
    
    glDeleteProgram(programID);
    glDeleteProgram(DepthProgramID);
    for (int i = 0; i < 1000; i++) {
        glDeleteTextures(1, &DiffuseTexture[i]);
        glDeleteTextures(1, &NormalTexture[i]);
        glDeleteTextures(1, &SpecularTexture[i]);
    }
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteFramebuffers(1, &DepthFrameBuffer);
    glDeleteTextures(1, &DepthTexture);
    glDeleteTextures(1, &HomePageTexture);
    
    
    deleteAL();
    
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;

    
}

void deleteAL() {
    fclose(fp);
    delete[] buf;
    
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

void initAL(char* filename) {
    
    fp = fopen(filename, "rb");
    
    char type[4];
    ALuint size, chunkSize;
    short formatType, channels;
    ALuint sampleRate, avgBytesPerSec;
    short bytesPerSample, bitsPerSample;
    ALuint dataSize;
    
    fread(type, sizeof(char), 4, fp);
    if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F') {
        printf("OpenAL Error: No RIFF\n");
        exit(-1);
    }
    
    fread(&size, sizeof(ALuint), 1, fp);
    fread(type, sizeof(char), 4, fp);
    if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E') {
        printf("OpenAL Error: Not a WAVE file\n");
        exit(-1);
    }
    
    fread(type, sizeof(char), 4, fp);
    if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ') {
        printf("OpenAL Error: Not a fmt\n");
        exit(-1);
    }
    
    fread(&chunkSize, sizeof(GLuint), 1, fp);
    fread(&formatType, sizeof(short), 1, fp);
    fread(&channels, sizeof(short), 1, fp);
    fread(&sampleRate, sizeof(GLuint), 1, fp);
    fread(&avgBytesPerSec, sizeof(GLuint), 1, fp);
    fread(&bytesPerSample, sizeof(short), 1, fp);
    fread(&bitsPerSample, sizeof(short), 1, fp);
    
    /*
     fread(type, sizeof(char), 4, fp);
     if (type[0] != 'd' || type[1] != 'a' || type[2] != 't' || type[3] != 'a') {
     printf("OpenAL Error: Missing DATA\n");
     return -1;
     }
     
     */
    
    fread(&dataSize, sizeof(GLuint), 1, fp);
    
    
    device = alcOpenDevice(NULL);
    if (!device) {
        printf("OpenAL Error: No sound device detected\n");
        exit(-1);
    }
    
    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);
    if (!context) {
        printf("OpenAL Error: No sound context\n");
        exit(-1);
    }
    
    buf = new unsigned char[dataSize];
    fread(buf, sizeof(ALuint), dataSize, fp);
    
    ALuint frequency = sampleRate;
    ALenum format = 0;
    
    alGenBuffers(1, &buffer);
    alGenSources(1, &source);
    
    if (bitsPerSample == 8)
    {
        if (channels == 1)
            format = AL_FORMAT_MONO8;
        else if (channels == 2)
            format = AL_FORMAT_STEREO8;
    }
    else if (bitsPerSample == 16)
    {
        if (channels == 1)
            format = AL_FORMAT_MONO16;
        else if (channels == 2)
            format = AL_FORMAT_STEREO16;
    }
    
    alBufferData(buffer, format, buf, dataSize, frequency);
    
    ALfloat SourcePos[] = { 0.0f, 0.0f, 0.0f };
    ALfloat SourceVel[] = { 0.0f, 0.0f, 0.0f };
    ALfloat ListenerPos[] = { 0.0f, 0.0f, 0.0f };
    ALfloat ListenerVel[] = { 0.0f, 0.0f, 0.0f };
    ALfloat ListenerOri[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
    
    // Listener
    alListenerfv(AL_POSITION, ListenerPos);
    alListenerfv(AL_VELOCITY, ListenerVel);
    alListenerfv(AL_ORIENTATION, ListenerOri);
    
    // Source
    alSourcei(source, AL_BUFFER, buffer);
    alSourcef(source, AL_PITCH, 1.0f);
    alSourcef(source, AL_GAIN, 1.0f);
    alSourcefv(source, AL_POSITION, SourcePos);
    alSourcefv(source, AL_VELOCITY, SourceVel);
    alSourcei(source, AL_LOOPING, AL_FALSE);
}

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
    
    
    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "ShadowMapping.vertexshader", "ShadowMapping.fragmentshader" );
    
    // Get a handle for uniform
    TextureID = glGetUniformLocation(programID, "myTextureSampler");
    
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
