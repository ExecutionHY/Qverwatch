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
    //DiffuseTexture[0] = loadDDS("res/diffuse.DDS");
    NormalTexture[0] = loadBMP_custom("res/normal.bmp");
    SpecularTexture[0] = loadDDS("res/specular.DDS");
 
    loadMap("res/Decode/HANAMURA.owmapx");
    //loadMap("res/car.owmapx");
    //obj[9999].initOBJ("res/test.obj");
    //Object test = Object("test", 9999, 0);
    
    printf("begin\n");
    
    
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
        
        vec3 lightInvDir = vec3(10, 10, 10);
        
        // Compute the MVP from the light's point of view
        DepthProjectionMatrix = ortho<float>(-100, 100, -100, 100, -100, 200);
        DepthViewMatrix = lookAt(lightInvDir, vec3(0, 0, 0), vec3(0, 1, 0));
        
        //test.loadDepth();
        
        
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
