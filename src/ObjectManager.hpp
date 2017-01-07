//
//  ObjectManager.hpp
//  OpenGL
//
//  Created by Execution on 11/9/16.
//  Copyright © 2016 Execution. All rights reserved.
//

#ifndef ObjectManager_hpp
#define ObjectManager_hpp

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

#include "objloader.hpp"
#include "vboindexer.hpp"
#include "tangentspace.hpp"

using namespace std;
using namespace glm;

extern mat4 ProjectionMatrix;
extern mat4 ViewMatrix;
extern mat4 ModelMatrix;
extern mat4 MVP;
extern mat4 BiasMatrix;
extern mat4 DepthBiasMVP;
extern mat4 DepthProjectionMatrix;
extern mat4 DepthViewMatrix;
extern mat4 DepthModelMatrix;
extern mat4 DepthMVP;

extern GLuint programID;
extern GLuint MVPMatrixID;
extern GLuint ViewMatrixID;
extern GLuint ModelMatrixID;
extern GLuint ModelView3x3MatrixID;
extern GLuint SurfaceID;
extern GLuint TextureID;

extern GLuint DiffuseTextureID;
extern GLuint NormalTextureID;
extern GLuint SpecularTextureID;

extern GLuint QuadProgramID;
extern GLuint QuadTextureID;
extern GLuint DepthProgramID;
extern GLuint DepthMVPMatrixID;
extern GLuint DepthBiasID;
extern GLuint ShadowMapID;

extern GLuint LightID;
extern GLuint LightInvDirID;

extern GLuint DiffuseTexture[1000];
extern GLuint NormalTexture[1000];
extern GLuint SpecularTexture[1000];

extern GLuint DepthTexture;

class OBJ {
public:
    char* name;
    vector<unsigned short> indices;
    vector<vec3> indexed_vertices;
    vector<vec2> indexed_uvs;
    vector<vec3> indexed_normals;
    vector<vec3> indexed_tangents;
    vector<vec3> indexed_bitangents;
    
public:
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    GLuint elementbuffer;
    GLuint tangentbuffer;
    GLuint bitangentbuffer;

public:
    OBJ();
    void initOBJ(char* name);
    void initBuffer();
    void bindBuffer();
    ~OBJ();
    int getIndiceSize();
};

class Object {
private:
    string name;
    mat4 ModelMatrix;       // the matrix of model
    mat4 DepthMVP;
    bool movable;            // whether the model movable
    int surface;          // light on all faces
    int ObjNum;             // the num in Objs
    int DiffuseTextureNum;         // the num in textures
    int NormalTextureNum;
    vec3 pos;
    bool valid;
public:
    float range;
public:
    Object();
    Object(string name, int ObjNum, int DiffuseTextureNum, int NormalTextureNum, int surface = 0);
    //Object(string name, int ObjNum, int TextureNum, int surface);
    void drawObject();
    void loadDepth();
    void setModel(mat4 model);
    mat4 getModel();
    void setPos(vec3 pos);
    void checkPos(vec3 myPos, float dist);
    bool isValid();
    void setRange(float range);
};

#endif /* ObjectManager_hpp */
