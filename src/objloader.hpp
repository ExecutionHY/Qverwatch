//
//  objloader.hpp
//  OpenGL
//
//  Created by Execution on 11/5/16.
//  Copyright © 2016 Execution. All rights reserved.
//

#ifndef objloader_hpp
#define objloader_hpp

#include <stdio.h>
bool loadOBJ(
             const char * path,
             std::vector<glm::vec3> & out_vertices,
             std::vector<glm::vec2> & out_uvs,
             std::vector<glm::vec3> & out_normals
             );

#endif /* objloader_hpp */
