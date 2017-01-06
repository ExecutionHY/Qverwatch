//
//  texture.hpp
//  OpenGL
//
//  Created by Execution on 11/5/16.
//  Copyright © 2016 Execution. All rights reserved.
//

#ifndef texture_hpp
#define texture_hpp

#include <stdio.h>
// Load a .BMP file using our custom loader
GLuint loadBMP_custom(const char * imagepath);

//// Since GLFW 3, glfwLoadTexture2D() has been removed. You have to use another texture loading library,
//// or do it yourself (just like loadBMP_custom and loadDDS)
//// Load a .TGA file using GLFW's own loader
//GLuint loadTGA_glfw(const char * imagepath);

// Load a .DDS file using GLFW's own loader
GLuint loadDDS(const char * imagepath);

#endif /* texture_hpp */
