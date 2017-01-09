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
    DiffuseTexture[0] = loadBMP_custom("res/wall.bmp");
    //GLuint test = loadBMP_custom("res/bmp/000000001080.bmp");
    //DiffuseTexture[0] = loadDDS("res/diffuse.DDS");
    NormalTexture[0] = loadBMP_custom("res/bmp/000000001B16.bmp");
    SpecularTexture[0] = loadDDS("res/specular.DDS");
 
    //loadMap("res/Decode/HANAMURA.owmapx");
    //loadMap("res/car.owmapx");
    loadMap("res/Decode/simple.owmapx");
    //obj[0].initOBJ("res/phy.obj");
    //Object test = Object("test", 0, 0, 0);
    //test.setModel(scale(mat4x4(1.0), vec3(10,10,10)));
    
    //initAL("res/Hanamura.wav");
    
    //selectingPage();
    
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
    
    /*
    // right back up light
    vec3 nearLightPos = vec3(13.802021, 13.373522, -87.761742);
    vec3 basePos = vec3(-13.333028, 3.317286, -84.592285);
    vec3 lightInvDir = nearLightPos - basePos;
    vec3 lightPos = basePos + lightInvDir * 1000.0f;
    lightPos = basePos + lightInvDir * 10.0f;
    DepthProjectionMatrix = perspective(90.0f, 1.0f/1.0f, 20.0f, 100.0f);
    DepthViewMatrix = lookAt(nearLightPos, basePos, vec3(0, 1, 0));
    */
    
    
    // sword spot
    
    vec3 nearLightPos = vec3(-15.086610, 1.191215, -90.229813);
    vec3 basePos = vec3(-17.558996, 0.074182, -91.201866);
    vec3 lightInvDir = nearLightPos - basePos;
    vec3 lightPos = basePos + lightInvDir * 4.0f;
    DepthProjectionMatrix = ortho<float>(-1, 1, -1, 1, 2, 20);
    DepthViewMatrix = lookAt(nearLightPos, basePos, vec3(0, 1, 0));
    
    
    
    
    
    
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
        
        char text2[256] = "|";
        printText2D(text2, 390, 270, 20);
        char text3[256] = "= =";
        printText2D(text3, 370, 290, 20);
        char text4[256] = "|";
        printText2D(text4, 390, 310, 20);
        
        
        // ********************** Debug *************************
        
        // Render to the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glViewport(0,0,windowWidth/2,windowHeight/2);
        
        // Use our shader
        glUseProgram(QuadProgramID);
        
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DepthTexture);
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
    
    
    //deleteAL();
    
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;

    
}
