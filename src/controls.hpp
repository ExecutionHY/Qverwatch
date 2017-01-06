//
//  controls.hpp
//  OpenGL
//
//  Created by Execution on 11/5/16.
//  Copyright © 2016 Execution. All rights reserved.
//

#ifndef controls_hpp
#define controls_hpp

#include "macro.h"

extern int windowWidth;
extern int windowHeight;
extern int rcdIndex;
extern Object object[10000];

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

#endif /* controls_hpp */
