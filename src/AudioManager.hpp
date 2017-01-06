//
//  AudioManager.hpp
//  OpenGL
//
//  Created by Execution on 06/01/2017.
//  Copyright © 2017 Execution. All rights reserved.
//

#ifndef AudioManager_hpp
#define AudioManager_hpp

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>

#include <AL/al.h>
#include <AL/alc.h>

void initAL(char* filename);
void deleteAL();

#endif /* AudioManager_hpp */
