//
//  text2D.hpp
//  OpenGL
//
//  Created by Execution on 11/5/16.
//  Copyright © 2016 Execution. All rights reserved.
//

#ifndef text2D_hpp
#define text2D_hpp

#include <stdio.h>
void initText2D(const char * texturePath);
void printText2D(const char * text, int x, int y, int size);
void cleanupText2D();

#endif /* text2D_hpp */
