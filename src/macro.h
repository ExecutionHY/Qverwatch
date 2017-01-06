//
//  macro.h
//  OpenGL
//
//  Created by Execution on 11/9/16.
//  Copyright © 2016 Execution. All rights reserved.
//

#ifndef macro_h
#define macro_h

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

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <AL/al.h>
#include <AL/alc.h>

using namespace glm;
using namespace std;

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "objloader.hpp"
#include "vboindexer.hpp"
#include "text2D.hpp"
#include "ObjectManager.hpp"
#include "MapLoader.hpp"

#define max(a, b)  (((a) > (b)) ? (a) : (b))

#define DEPTH_TEXTURE_SIZE 1024
#define FRUSTUM_DEPTH 1.0f

#endif /* macro_h */
