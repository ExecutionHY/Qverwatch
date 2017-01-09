//
//  main.cpp
//  OpenGL
//
//  Created by Execution on 11/4/16.
//  Copyright © 2016 Execution. All rights reserved.
//

#include "macro.h"


mat4 ProjectionMatrix;
mat4 ViewMatrix;
mat4 ModelMatrix;
mat4 MVP;

// For shadow - depth texure
mat4 DepthProjectionMatrix;
mat4 DepthViewMatrix;
mat4 DepthModelMatrix;
mat4 DepthMVP;
mat4 BiasMatrix;
mat4 DepthBiasMVP;

// CPU representation of a particle
struct Particle{
    glm::vec3 pos, speed;
    unsigned char r,g,b,a; // Color
    float size, angle, weight;
    float life; // Remaining life of the particle. if <0 : dead and unused.
    float cameradistance; // *Squared* distance to the camera. if dead : -1.0f
    
    bool operator<(const Particle& that) const {
        // Sort in reverse order : far particles drawn first.
        return this->cameradistance > that.cameradistance;
    }
};

const int MaxParticles = 500;
const float MaxLife = 5.0f;
Particle ParticlesContainer[MaxParticles];
int LastUsedParticle = 0;

// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int FindUnusedParticle(){
    
    for(int i=LastUsedParticle; i<MaxParticles; i++){
        if (ParticlesContainer[i].life < 0){
            LastUsedParticle = i;
            return i;
        }
    }
    
    for(int i=0; i<LastUsedParticle; i++){
        if (ParticlesContainer[i].life < 0){
            LastUsedParticle = i;
            return i;
        }
    }
    
    return 0; // All particles are taken, override the first one
}

void SortParticles(){
    std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}



int main( void )
{
    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    int fps = 0;

    
    initGL();
    HomePageTexture = loadBMP_custom("res/hanamura.bmp");
    // Initialize our little text library with the Holstein font
    initText2D("res/Holstein.dds");
    loadingPage();
    
    // ***************************** resource loading *******************************
    UITexture = loadBMP_custom("res/ui.bmp");
    ParticleTexture = loadDDS("res/particle.DDS");
    //Texture[0] = loadBMP_custom("res/wall.bmp");
    DiffuseTexture[0] = loadBMP_custom("res/wall.bmp");
    //GLuint test = loadBMP_custom("res/bmp/000000001080.bmp");
    //DiffuseTexture[0] = loadDDS("res/diffuse.DDS");
    NormalTexture[0] = loadBMP_custom("res/bmp/000000001B16.bmp");
    SpecularTexture[0] = loadDDS("res/specular.DDS");
 
    //loadMap("res/Decode/HANAMURA.owmapx");
    //loadMap("res/car.owmapx");
    loadMap("res/Decode/simple.owmapx");
    obj[0].initOBJ("res/phy.obj");
    //Object test = Object("test", 0, 0, 0);
    //test.setModel(scale(mat4x4(1.0), vec3(10,10,10)));
    
    //initAL("res/Hanamura.wav");
    
    selectingPage();
    
    //alSourcePlay(source);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    
    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, DepthFrameBuffer);
    glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
    
    // not using bias but draw back faces !!!
    glCullFace(GL_FRONT);
    
    // Clear the screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // Use our shader
    glUseProgram(DepthProgramID);
    /*
     //vec3 lightInvDir = vec3(10, 10, 10);
     vec3 lightPos = vec3(-11, 11, -84);
     vec3 lightInvDir = vec3(-15.929764, 18.679287, -88.606285) - vec3(-7.360991, 3.405631, -81.104904);
     */
    /*
    //vec3 lightPos = vec3( -12, 15, -85 );
    vec3 nearLightPos = vec3( -55.915115, 10.807584, -36.536823 );
    vec3 basePos = vec3( -8.680655, 0.710400, -88.737778 );
    vec3 lightInvDir = nearLightPos - basePos;
    vec3 lightPos = basePos + lightInvDir * 1000.0f;
    DepthProjectionMatrix = ortho<float>(-4, 4, -4, 4, 20, 400);
    DepthViewMatrix = lookAt(nearLightPos, basePos, vec3(0, 1, 0));
    */
    
    
    // right back up light
    vec3 nearLightPos = vec3(13.802021, 13.373522, -87.761742);
    vec3 basePos = vec3(-13.333028, 3.317286, -84.592285);
    vec3 lightInvDir = nearLightPos - basePos;
    vec3 lightPos = basePos + lightInvDir * 1000.0f;
    lightPos = basePos + lightInvDir * 10.0f;
    DepthProjectionMatrix = ortho<float>(-20, 20, -20, 20, 20, 200);
    //DepthProjectionMatrix = perspective(90.0f, 1.0f/1.0f, 20.0f, 100.0f);
    DepthViewMatrix = lookAt(nearLightPos, basePos, vec3(0, 1, 0));
    
    
    /*
    // sword spot
    
    vec3 nearLightPos = vec3(-15.086610, 1.191215, -90.229813);
    vec3 basePos = vec3(-17.558996, 0.074182, -91.201866);
    vec3 lightInvDir = nearLightPos - basePos;
    vec3 lightPos = basePos + lightInvDir * 6.0f;
    DepthProjectionMatrix = ortho<float>(-1, 1, -1, 1, 2, 20);
    DepthViewMatrix = lookAt(nearLightPos, basePos, vec3(0, 1, 0));
    */
    
    
    
    
    // Compute the MVP from the light's point of view
    //DepthProjectionMatrix = perspective(20.0f, 1.0f/1.0f, 2.0f, 20.0f);
    //DepthViewMatrix = lookAt(lightInvDir, vec3(0, 0, 0), vec3(0, 1, 0));
    
    //test.loadDepth();
    
    
    for (int i = 0; i < objIndex; i++) {
        object[i].loadDepth();
    }
    for (int i = objIndex; i < rcdIndex; i++) {
        object[i].loadDepth();
    }
    
    
    
    
    static GLfloat* g_particule_position_size_data = new GLfloat[MaxParticles * 4];
    static GLubyte* g_particule_color_data         = new GLubyte[MaxParticles * 4];
    
    for(int i=0; i<MaxParticles; i++){
        ParticlesContainer[i].life = -1.0f;
        ParticlesContainer[i].cameradistance = -1.0f;
    }
    
    
    // The VBO containing the 4 vertices of the particles.
    // Thanks to instancing, they will be shared by all particles.
    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
        0.5f,  0.5f, 0.0f,
    };
    GLuint billboard_vertex_buffer;
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    // The VBO containing the positions and sizes of the particles
    GLuint particles_position_buffer;
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
    
    // The VBO containing the colors of the particles
    GLuint particles_color_buffer;
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
    
    double currentFrame, lastFrame, delta;
    
    int k = 0;
    bool upReleased = true;
    
    do{
        
        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);
        
        // not using bias but draw back faces ..?
        glCullFace(GL_BACK);
        
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
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
        
        //test.drawObject();
        
        
        for (int i = 0; i < objIndex; i++) {
            if (object[i].isValid()) object[i].drawObject();
        }
        for (int i = objIndex; i < rcdIndex; i++) {
            if (object[i].isValid()) object[i].drawObject();
        }
        
        
        
        
        
        // We will need the camera's position in order to sort the particles
        // w.r.t the camera's distance.
        // There should be a getCameraPosition() function in common/controls.cpp,
        // but this works too.
        glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);
        glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
        
        currentFrame = glfwGetTime();
        delta = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        int newparticles = (int)(delta *MaxParticles/3.0f);
        
        for(int i=0; i<newparticles; i++){
            int particleIndex = FindUnusedParticle();
            ParticlesContainer[particleIndex].life = MaxLife*(100+rand() % 400)/500.0f; // This particle will live 5 seconds.
            ParticlesContainer[particleIndex].pos = vec3(-13.391409, 9.002603, -87.632225);
            
            float spread = 1.5f;
            glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
            // Very bad way to generate a random direction;
            // See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
            // combined with some user-controlled parameters (main direction, spread, etc)
            glm::vec3 randomdir = glm::vec3(
                                            (rand()%200 - 100.0f)/1000.0f,
                                            (rand()%200 - 100.0f)/1000.0f,
                                            (rand()%200 - 100.0f)/1000.0f
                                            );
            
            ParticlesContainer[particleIndex].speed = maindir + randomdir*spread;/*
            ParticlesContainer[particleIndex].r = 128;
            ParticlesContainer[particleIndex].g = 128;
            ParticlesContainer[particleIndex].b = 128;
            ParticlesContainer[particleIndex].a = 30;*/
            
            ParticlesContainer[particleIndex].r = 255;
            ParticlesContainer[particleIndex].g = 0;
            ParticlesContainer[particleIndex].b = 0;
            ParticlesContainer[particleIndex].a = 255;
            
            /*
             // Very bad way to generate a random color
             ParticlesContainer[particleIndex].r = rand() % 256;
             ParticlesContainer[particleIndex].g = rand() % 256;
             ParticlesContainer[particleIndex].b = rand() % 256;
             ParticlesContainer[particleIndex].a = (rand() % 256) / 3;
             */
            ParticlesContainer[particleIndex].size = (rand()%1000)/2000.0f + 0.1f;
            
        }
        
        
        
        // Simulate all particles
        int ParticlesCount = 0;
        for(int i=0; i<MaxParticles; i++){
            
            Particle& p = ParticlesContainer[i]; // shortcut
            
            if(p.life > 0.0f){
                
                // Decrease life
                p.life -= delta;
                if (p.life > 0.0f){
                    
                    // Simulate simple physics : gravity only, no collisions
                    //p.speed += glm::vec3(0.0f,-9.81f, 0.0f) * (float)delta * 0.5f;
                    p.pos += p.speed * (float)delta;
                    p.cameradistance = glm::length2( p.pos - CameraPosition );
                    //ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;
                    
                    // Fill the GPU buffer
                    g_particule_position_size_data[4*ParticlesCount+0] = p.pos.x;
                    g_particule_position_size_data[4*ParticlesCount+1] = p.pos.y;
                    g_particule_position_size_data[4*ParticlesCount+2] = p.pos.z;
                    
                    g_particule_position_size_data[4*ParticlesCount+3] = p.size;
                    
                    g_particule_color_data[4*ParticlesCount+0] = p.r;
                    g_particule_color_data[4*ParticlesCount+1] = p.g;
                    g_particule_color_data[4*ParticlesCount+2] = p.b;
                    g_particule_color_data[4*ParticlesCount+3] = p.a;
                    
                }else{
                    // Particles that just died will be put at the end of the buffer in SortParticles();
                    p.cameradistance = -1.0f;
                }
                
                ParticlesCount++;
                
            }
        }
        
        SortParticles();
        
        
        //printf("%d ",ParticlesCount);
        
        
        // Update the buffers that OpenGL uses for rendering.
        // There are much more sophisticated means to stream data from the CPU to the GPU,
        // but this is outside the scope of this tutorial.
        // http://www.opengl.org/wiki/Buffer_Object_Streaming
        
        
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);
        
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);
        
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Use our shader
        glUseProgram(ParticleProgramID);
        
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ParticleTexture);
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(ParticleTextureID, 0);
        
        // Same as the billboards tutorial
        glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
        
        glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);
        
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
        glVertexAttribPointer(
                              0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        // 2nd attribute buffer : positions of particles' centers
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glVertexAttribPointer(
                              1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                              4,                                // size : x + y + z + size => 4
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        // 3rd attribute buffer : particles' colors
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glVertexAttribPointer(
                              2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                              4,                                // size : r + g + b + a => 4
                              GL_UNSIGNED_BYTE,                 // type
                              GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        // These functions are specific to glDrawArrays*Instanced*.
        // The first parameter is the attribute buffer we're talking about.
        // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
        // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
        glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
        glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
        glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1
        
        // Draw the particles !
        // This draws many times a small triangle_strip (which looks like a quad).
        // This is equivalent to :
        // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
        // but faster.
        
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        // Delete
        glVertexAttribDivisor(0, 0);
        glVertexAttribDivisor(1, 0);
        glVertexAttribDivisor(2, 0);
        
        glDisable(GL_BLEND);
        
        
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
        }
        
        char text[256];
        sprintf(text, "TIME: %.2f s\nFPS: %d", glfwGetTime(), fps);
        printText2D(text, 10, 570, 15);
        /*
        char text2[256] = "|";
        printText2D(text2, 390, 270, 20);
        char text3[256] = "= =";
        printText2D(text3, 370, 290, 20);
        char text4[256] = "|";
        printText2D(text4, 390, 310, 20);
        */
        
        // ********************** Debug *************************
        /*
        if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_RELEASE)
            upReleased = true;
        
        if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS) {
            if (upReleased) {
                k++;
                printf("%d---", k);
            }
        }*/
        
        glEnable(GL_BLEND);
        
        // Render to the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glViewport(0,0,windowWidth, windowHeight);
        
        // Use our shader
        glUseProgram(QuadProgramID);
        glUniform1i(UIID, 1);
        
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, UITexture);
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
        
        glDisable(GL_BLEND);
        
        
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();        
        
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
    
    
    //deleteAL();
    
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;

    
}
