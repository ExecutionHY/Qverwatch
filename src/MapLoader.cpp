//
//  MapLoader.cpp
//  OpenGL
//
//  Created by Execution on 02/01/2017.
//  Copyright © 2017 Execution. All rights reserved.
//
#include "macro.h"
#include "MapLoader.hpp"
#include <string>

int objIndex = 1, objIndex1 = 1, rcdIndex = 1;
vector<int> diffuseT;
vector<int> normalT;
int diffuseTextureCount = 1, normalTextureCount = 1;
string diffuseTextureList[1000];
string normalTextureList[1000];

using namespace glm;
bool loadMap(char * filename) {
    printf("Loading OWMAPX file %s...\n", filename);
    
    FILE* mapfile = fopen(filename, "r");
    if( mapfile == NULL ){
        printf("Impossible to open the file ! Are you in the right path ?\n");
        getchar();
        return false;
    }
    
    char str[256];  // save temp string
    int t, objectCount, detailCount, lightCount;
    
    // map header
    fscanf(mapfile, "%s %d %d %s %d %d %d\n", str, &t, &t, str, &objectCount, &detailCount, &lightCount);
    
    char MdlName[256];
    char MatName[256];
    int entityCount, recordCount;
    
    // read detail
    for (int detail = 0; detail < detailCount; detail++) {
        fscanf(mapfile, "%s %s %s\n", str, MdlName, MatName);
        
        vec3 pos, scl, rtt;
        quat rot;
        fscanf(mapfile, "[%f, %f, %f]\n", &pos.x, &pos.y, &pos.z);
        fscanf(mapfile, "[%f, %f, %f]\n", &scl.x, &scl.y, &scl.z);
        fscanf(mapfile, "[%f, %f, %f, %f]\n", &rot.x, &rot.y, &rot.z, &rot.w);
        
        //if (distance(pos, position) > 50.0f) continue;
        
        float agl = acosf(rot.w)*2;
        rtt.x = rot.x / sin(agl/2);
        rtt.y = rot.y / sin(agl/2);
        rtt.z = rot.z / sin(agl/2);
        
        mat4x4 model;
        model = translate(mat4x4(1.0), pos) *
        rotate(mat4x4(1.0), agl, rtt) *
        scale(mat4x4(1.0), scl);
        
        printf("Detail: %d - ", detail);
        int objIndex2 = loadMdl(MdlName, objIndex);
        for (int i = objIndex; i < objIndex2; i++) {
            object[i] = Object(MdlName, i, 0, 0);
            object[i].setModel(model);
        }
        objIndex = objIndex2;
    }
    objIndex1 = objIndex;
    rcdIndex = objIndex;
    int mtlIndex = 1;
    // read object
     for (int objct = 0; objct < objectCount; objct++) {
         fscanf(mapfile, "%s %s %d\n", str, MdlName, &entityCount);
         mat4x4 model = mat4x4(1.0);
         printf("objects: %d - ", objct);
         int objIndex2 = loadMdl(MdlName, objIndex1);
         int meshCount = objIndex2 - objIndex1;
         
         // read entity
         int rcdcnt = 0;
         for (int entity = 0; entity < entityCount; entity++) {
             fscanf(mapfile, "%s %s %d\n", str, MatName, &recordCount);
             for (int record = 0; record < recordCount; record++) {
                 rcdcnt++;
                 fscanf(mapfile, "%s\n", str);
                 
                 vec3 pos, scl, rtt;
                 vec4 rot;
                 fscanf(mapfile, "[%f, %f, %f]\n", &pos.x, &pos.y, &pos.z);
                 fscanf(mapfile, "[%f, %f, %f]\n", &scl.x, &scl.y, &scl.z);
                 fscanf(mapfile, "[%f, %f, %f, %f]\n", &rot.x, &rot.y, &rot.z, &rot.w);
                 
                 //if (distance(pos, position) > 50.0f) continue;
                 
                 float agl = acosf(rot.w)*2;
                 rtt.x = rot.x / sin(agl/2);
                 rtt.y = rot.y / sin(agl/2);
                 rtt.z = rot.z / sin(agl/2);
                 
                 mat4x4 model;
                 model = translate(mat4x4(1.0), pos) *
                 rotate(mat4x4(1.0), agl, rtt) *
                 scale(mat4x4(1.0), scl);
                 
                 // add (meshCount) records into object[]
                 loadMat(MatName, mtlIndex);
                 if (shouldDelete(MatName, rcdcnt)) model = mat4x4(0);
                 for (int i = 0; i < meshCount; i++) {
                     object[i+rcdIndex] = Object(MdlName, i+objIndex1, diffuseT[i], 0);
                     object[i+rcdIndex].setModel(model);
                     object[i+rcdIndex].setPos(pos);
                     object[i+rcdIndex].setRange(max(max(scl.x, scl.y), scl.z));
                 }
                 rcdIndex += meshCount;
             }
             mtlIndex += meshCount;
         }
         objIndex1 = objIndex2;
     }
    
    
    printf("loadMAP complete.\n");
    return true;
}

int loadMdl( const char * path, int objIndex ) {
    
    char fullpath[256] = "";
    sprintf(fullpath,"%s%sx","res/Decode/",path);
    printf("Loading MDL file %s...\n", fullpath);
    FILE * file = fopen(fullpath, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
        return objIndex;
    }
    
    int meshCount, t;
    int uvCount, vertexCount, indexCount;
    
    // OWMDLHeader -> meshCount
    fscanf(file, "%d %d %d %d\n", &t, &t, &meshCount, &t);
    
    // read mesh
    for (int meshIndex = 0; meshIndex < meshCount; meshIndex++) {
        int idx = objIndex + meshIndex;
        
        // mesh header
        fscanf(file, "%d %d %d\n",&uvCount, &vertexCount, &indexCount);
        // read vertex
        for (int i = 0; i < vertexCount; i++) {
            glm::vec3 vertex;
            fscanf(file, "v: [%f, %f, %f] ", &vertex.x, &vertex.y, &vertex.z);
            obj[idx].indexed_vertices.push_back(vertex);
            glm::vec3 normal;
            fscanf(file, "[%f, %f, %f] [", &normal.x, &normal.y, &normal.z );
            normal = -normal;
            obj[idx].indexed_normals.push_back(normal);
            // I don't know why there is two uvs in a vertex, but we choose the 2nd cauz it's < 1
            glm::vec2 uv;
            for (int j = 0; j < uvCount; j++) {
                fscanf(file, "[%f, %f] ", &uv.x, &uv.y );
                if (j < uvCount-1) fscanf(file, ", ");
            }
            uv.y = 1 - uv.y;
            obj[idx].indexed_uvs.push_back(uv);
            fscanf(file, "]\n");
            
            // init tangents
            vec3 empty = vec3(0, 0, 0);
            obj[idx].indexed_tangents.push_back(empty);
            obj[idx].indexed_bitangents.push_back(empty);
        }
        // read index
        for (int i = 0; i < indexCount; i++) {
            int pointCount;
            unsigned short index;
            fscanf(file, "i: %d\n", &pointCount);
            if (pointCount != 3) printf("Attention: pointCount != 3, = %d\n", pointCount);
            fscanf(file, "%hd ", &index);
            obj[idx].indices.push_back(index);
            fscanf(file, "%hd ", &index);
            obj[idx].indices.push_back(index);
            fscanf(file, "%hd \n", &index);
            obj[idx].indices.push_back(index);
        }
        // get tangents
        for (int i = 0; i < indexCount; i += 3) {
            vec3 v0 = obj[idx].indexed_vertices[obj[idx].indices[i+0]];
            vec3 v1 = obj[idx].indexed_vertices[obj[idx].indices[i+1]];
            vec3 v2 = obj[idx].indexed_vertices[obj[idx].indices[i+2]];
            
            vec2 uv0 = obj[idx].indexed_uvs[obj[idx].indices[i+0]];
            vec2 uv1 = obj[idx].indexed_uvs[obj[idx].indices[i+1]];
            vec2 uv2 = obj[idx].indexed_uvs[obj[idx].indices[i+2]];
            
            // Edges of the triangle : postion delta
            glm::vec3 deltaPos1 = v1-v0;
            glm::vec3 deltaPos2 = v2-v0;
            
            // UV delta
            glm::vec2 deltaUV1 = uv1-uv0;
            glm::vec2 deltaUV2 = uv2-uv0;
            
            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            glm::vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
            glm::vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;
            
            // Set the same tangent for all three vertices of the triangle.
            // They will be merged later, in vboindexer.cpp
            obj[idx].indexed_tangents[obj[idx].indices[i+0]] = tangent;
            obj[idx].indexed_tangents[obj[idx].indices[i+1]] = tangent;
            obj[idx].indexed_tangents[obj[idx].indices[i+2]] = tangent;
            
            // Same thing for binormals
            obj[idx].indexed_bitangents[obj[idx].indices[i+0]] = bitangent;
            obj[idx].indexed_bitangents[obj[idx].indices[i+1]] = bitangent;
            obj[idx].indexed_bitangents[obj[idx].indices[i+2]] = bitangent;
        }
        
        obj[idx].initBuffer();
    }
    
    return objIndex+meshCount;
}

int loadMat(const char* path, int mtlIndex) {
    char fullpath[256] = "";
    sprintf(fullpath,"%s%sx","res/Decode/",path);
    printf("Loading MAT file %s...\n", fullpath);
    FILE * file = fopen(fullpath, "r");
    if( file == NULL ){
        printf("Impossible to open the file %s!\n", path);
        return objIndex;
    }
    int flag = 0, t, materialCount, textureCount, textureType;
    
    // OWMATHeader -> materialCount
    fscanf(file, "%d %d %d\n", &flag, &t, &materialCount);
    
    diffuseT.clear();
    normalT.clear();
    char ddsFile[256], str[256];
    for (int mtl = 0; mtl < materialCount; mtl++) {
        fscanf(file, "%s %d\n", str, &textureCount);
        int diffuseIndex = 0, normalIndex = 0;
        for (int t = 0; t < textureCount; t++) {
            fscanf(file, "%s %d\n", ddsFile, &textureType);
            char bmpPath[256] = "";
            sprintf(bmpPath, "%s%s", "res/bmp/", ddsFile);
            int length = strlen(bmpPath);
            bmpPath[length-3] = 'b';
            bmpPath[length-2] = 'm';
            bmpPath[length-1] = 'p';
            if (textureType == 0 && diffuseIndex == 0) {
                diffuseIndex = findDiffuseTexture(bmpPath);
                if (diffuseIndex == 0) {
                    DiffuseTexture[diffuseTextureCount] = loadBMP_custom(bmpPath);
                    diffuseIndex = diffuseTextureCount;
                    diffuseTextureList[diffuseTextureCount] = string(bmpPath);
                    diffuseTextureCount++;
                }
            }
            else if (textureType == 1 && normalIndex == 0) {
                normalIndex = findNormalTexture(bmpPath);
                if (normalIndex == 0) {
                    NormalTexture[normalTextureCount] = loadBMP_custom(bmpPath);
                    normalIndex = normalTextureCount;
                    normalTextureList[normalTextureCount] = string(bmpPath);
                    normalTextureCount++;
                }
            }
        }
        diffuseT.push_back(diffuseIndex);
        normalT.push_back(normalIndex);
    }
    return 1;
}

bool shouldDelete(const char* entity, const int rcd) {
    if (strcmp(entity, "00000000078C_0000000010D2.owmat") == 0) return true;
    return false;
}

int findDiffuseTexture(const char* path) {
    for (int i = 1; i < diffuseTextureCount; i++) {
        if (strcmp(path, diffuseTextureList[i].c_str()) == 0)
            return i;
    }
    return 0;
}
int findNormalTexture(const char* path) {
    for (int i = 1; i < normalTextureCount; i++) {
        if (strcmp(path, normalTextureList[i].c_str()) == 0)
            return i;
    }
    return 0;
}
