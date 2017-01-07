//
//  tangentspace.hpp
//  OpenGL
//
//  Created by Execution on 07/01/2017.
//  Copyright © 2017 Execution. All rights reserved.
//

#ifndef tangentspace_hpp
#define tangentspace_hpp

#include <stdio.h>

void computeTangentBasis(
                         // inputs
                         std::vector<glm::vec3> & vertices,
                         std::vector<glm::vec2> & uvs,
                         std::vector<glm::vec3> & normals,
                         // outputs
                         std::vector<glm::vec3> & tangents,
                         std::vector<glm::vec3> & bitangents
                         );


#endif
