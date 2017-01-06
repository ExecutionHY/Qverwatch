//
//  PageManager.hpp
//  OpenGL
//
//  Created by Execution on 06/01/2017.
//  Copyright © 2017 Execution. All rights reserved.
//

#ifndef PageManager_hpp
#define PageManager_hpp

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
#include "shader.hpp"
#include "text2D.hpp"

#define DEPTH_TEXTURE_SIZE 1024
#define FRUSTUM_DEPTH 1.0f

extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

void initGL();
void loadingPage();
void selectingPage();

#endif /* PageManager_hpp */
