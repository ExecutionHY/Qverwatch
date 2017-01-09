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
#include "AudioManager.hpp"
#include "PageManager.hpp"
#include "tangentspace.hpp"

#define max(a, b)  (((a) > (b)) ? (a) : (b))



extern GLFWwindow* window;
extern int windowWidth;
extern int windowHeight;

extern GLuint DepthTexture;
extern GLuint DepthFrameBuffer;
extern GLuint quad_vertexbuffer;

extern GLuint UIID;
extern GLuint VertexArrayID;
extern GLuint QuadProgramID;
extern GLuint QuadTextureID;
extern GLuint DepthProgramID;
extern GLuint DepthMVPMatrixID;
extern GLuint DepthBiasID;
extern GLuint ShadowMapID;

extern GLuint programID;
extern GLuint MVPMatrixID;
extern GLuint ViewMatrixID;
extern GLuint ModelMatrixID;
extern GLuint ModelView3x3MatrixID;

extern GLuint DiffuseTextureID;
extern GLuint NormalTextureID;
extern GLuint SpecularTextureID;
extern GLuint SurfaceID;
extern GLuint TextureID;

extern GLuint LightID;
extern GLuint LightInvDirID;

extern GLuint HomePageTexture;
extern GLuint DiffuseTexture[1000];
extern GLuint NormalTexture[1000];
extern GLuint SpecularTexture[1000];

extern OBJ obj[10000];
extern Object object[10000];
extern int objIndex, objIndex1, rcdIndex;
extern ALuint source;

// particles
extern GLuint ParticleProgramID;
extern GLuint ParticleTextureID;
extern GLuint CameraRight_worldspace_ID;
extern GLuint CameraUp_worldspace_ID;
extern GLuint ViewProjMatrixID;
extern GLuint ParticleTexture;
extern GLuint UITexture;

#endif /* macro_h */
