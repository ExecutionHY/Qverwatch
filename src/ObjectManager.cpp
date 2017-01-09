//
//  ObjectManager.cpp
//  OpenGL
//
//  Created by Execution on 11/9/16.
//  Copyright © 2016 Execution. All rights reserved.
//

#include "ObjectManager.hpp"

OBJ obj[10000];
Object object[10000];

OBJ::OBJ() {
    this->name = "";
}
void OBJ::initBuffer() {
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size()*sizeof(vec3), &indexed_vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size()*sizeof(vec2), &indexed_uvs[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size()*sizeof(vec3), &indexed_normals[0], GL_STATIC_DRAW);
    
    // Buffer for indices
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &tangentbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tangentbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexed_tangents.size()*sizeof(vec3), &indexed_tangents[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &bitangentbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bitangentbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexed_bitangents.size()*sizeof(vec3), &indexed_bitangents[0], GL_STATIC_DRAW);
    
}
void OBJ::initOBJ(char* name) {
    this->name = name;
    if (name != "") {
        // Read our .obj file
        vector<vec3> verticles;
        vector<vec2> uvs;
        vector<vec3> normals;
        vector<vec3> tangents;
        vector<vec3> bitangents;
        loadOBJ(name, verticles, uvs, normals);
        computeTangentBasis(verticles, uvs, normals, tangents, bitangents);
        
        //  Load it into a VBO
        indexVBO_TBN(verticles, uvs, normals, tangents, bitangents, indices, indexed_vertices, indexed_uvs, indexed_normals, indexed_tangents, indexed_bitangents);
        // release vectors
        verticles.clear();
        uvs.clear();
        normals.clear();
        tangents.clear();
        bitangents.clear();
        
        initBuffer();
    }
}
void OBJ::bindBuffer() {
    
    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0,        // attribute
                          3,        // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0,        // stride
                          (void*)0  // array buffer offset
                          );
    
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // size=2
    
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // size=3
    
    // 4th attribute buffer : tangents
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // size=3
    
    // 5th attribute buffer : bitangents
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // size=3
    
    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
}
OBJ::~OBJ() {
    indices.clear();
    indexed_vertices.clear();
    indexed_uvs.clear();
    indexed_normals.clear();
    indexed_tangents.clear();
    indexed_bitangents.clear();
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteBuffers(1, &tangentbuffer);
    glDeleteBuffers(1, &bitangentbuffer);
}
int OBJ::getIndiceSize() {
    return indices.size();
}


Object::Object() {
    this->name = "";
}

Object::Object(string name, int ObjNum, int DiffuseTextureNum, int NormalTextureNum, int surface) {
    this->ObjNum = ObjNum;
    this->DiffuseTextureNum = DiffuseTextureNum;
    this->NormalTextureNum = NormalTextureNum;
    this->name = name;
    this->ModelMatrix = mat4(1.0);
    this->surface = surface;
}
/*
Object::Object(string name, int ObjNum, int TextureNum, int surface) {
    this->ObjNum = ObjNum;
    this->TextureNum = TextureNum;
    this->name = name;
    this->ModelMatrix = mat4(1.0);
    this->surface = surface;
}*/
void Object::drawObject() {
    // load its own MVP
    
    mat4 MVP = ProjectionMatrix * ViewMatrix * this->ModelMatrix;
    
    mat3 ModelView3x3Matrix = glm::mat3(ViewMatrix * ModelMatrix);
    
    mat4 BiasMatrix = mat4(0.5, 0.0, 0.0, 0.0,
                      0.0, 0.5, 0.0, 0.0,
                      0.0, 0.0, 0.5, 0.0,
                      0.5, 0.5, 0.5, 1.0
                      );
    mat4 DepthBiasMVP = BiasMatrix * this->DepthMVP;
    
    // Send transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &DepthBiasMVP[0][0]);
    glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
    
    // for PCSS
    glUniformMatrix4fv(DepthMVPID, 1, GL_FALSE, &DepthMVP[0][0]);
    
    /*
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture[this->TextureNum]);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(TextureID, 0);
    */
    
    // Bind our diffuse texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, DiffuseTexture[this->DiffuseTextureNum]);
    // Set our "DiffuseTextureSampler" sampler to user Texture Unit 0
    glUniform1i(DiffuseTextureID, 0);
    glUniform1i(SurfaceID, this->surface);

    // Bind our normal texture in Texture Unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, NormalTexture[this->NormalTextureNum]);
    // Set our "NormalTextureSampler" sampler to user Texture Unit 1
    glUniform1i(NormalTextureID, 1);
    
    // Bind our normal texture in Texture Unit 2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, SpecularTexture[0]);
    // Set our "SpecularTextureSampler" sampler to user Texture Unit 2
    glUniform1i(SpecularTextureID, 2);
    
    
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, DepthTexture);
    glUniform1i(ShadowMapID, 3);
    
    obj[this->ObjNum].bindBuffer();
    // Draw the triangles !
    glDrawElements(GL_TRIANGLES, obj[this->ObjNum].getIndiceSize(), GL_UNSIGNED_SHORT, (void*)0);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);

    
}
void Object::loadDepth() {
    // load its own MVP
    this->DepthMVP = DepthProjectionMatrix * DepthViewMatrix * this->ModelMatrix;
    glUniformMatrix4fv(DepthMVPMatrixID, 1, GL_FALSE, &DepthMVP[0][0]);
    
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, obj[this->ObjNum].vertexbuffer);
    glVertexAttribPointer(
                          0,  // The attribute we want to configure
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj[this->ObjNum].elementbuffer);
    
    // Draw the triangles !
    glDrawElements(
                   GL_TRIANGLES,      // mode
                   obj[this->ObjNum].getIndiceSize(),    // count
                   GL_UNSIGNED_SHORT, // type
                   (void*)0           // element array buffer offset
                   );
    
    glDisableVertexAttribArray(0);
}

void Object::setModel(mat4 model) {
    this->ModelMatrix = model;
}

mat4 Object::getModel() {
    return this->ModelMatrix;
}

void Object::setPos(vec3 pos) {
    this->pos = pos;
}

void Object::checkPos(vec3 myPos, float dist) {
    if (distance(pos, myPos) < dist) valid = true;
    else valid = false;
}

bool Object::isValid() {
    return valid;
}

void Object::setRange(float range) {
    this->range = range;
}
