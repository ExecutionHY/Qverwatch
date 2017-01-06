//
//  AudioManager.cpp
//  OpenGL
//
//  Created by Execution on 06/01/2017.
//  Copyright © 2017 Execution. All rights reserved.
//

#include "AudioManager.hpp"

FILE* fp = NULL;
ALCdevice* device;
ALCcontext* context;
ALuint buffer;
unsigned char* buf;
ALuint source;

void deleteAL() {
    fclose(fp);
    delete[] buf;
    
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

void initAL(char* filename) {
    
    fp = fopen(filename, "rb");
    
    char type[4];
    ALuint size, chunkSize;
    short formatType, channels;
    ALuint sampleRate, avgBytesPerSec;
    short bytesPerSample, bitsPerSample;
    ALuint dataSize;
    
    fread(type, sizeof(char), 4, fp);
    if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F') {
        printf("OpenAL Error: No RIFF\n");
        exit(-1);
    }
    
    fread(&size, sizeof(ALuint), 1, fp);
    fread(type, sizeof(char), 4, fp);
    if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E') {
        printf("OpenAL Error: Not a WAVE file\n");
        exit(-1);
    }
    
    fread(type, sizeof(char), 4, fp);
    if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ') {
        printf("OpenAL Error: Not a fmt\n");
        exit(-1);
    }
    
    fread(&chunkSize, sizeof(ALuint), 1, fp);
    fread(&formatType, sizeof(short), 1, fp);
    fread(&channels, sizeof(short), 1, fp);
    fread(&sampleRate, sizeof(ALuint), 1, fp);
    fread(&avgBytesPerSec, sizeof(ALuint), 1, fp);
    fread(&bytesPerSample, sizeof(short), 1, fp);
    fread(&bitsPerSample, sizeof(short), 1, fp);
    
    /*
     fread(type, sizeof(char), 4, fp);
     if (type[0] != 'd' || type[1] != 'a' || type[2] != 't' || type[3] != 'a') {
     printf("OpenAL Error: Missing DATA\n");
     return -1;
     }
     
     */
    
    fread(&dataSize, sizeof(ALuint), 1, fp);
    
    
    device = alcOpenDevice(NULL);
    if (!device) {
        printf("OpenAL Error: No sound device detected\n");
        exit(-1);
    }
    
    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);
    if (!context) {
        printf("OpenAL Error: No sound context\n");
        exit(-1);
    }
    
    buf = new unsigned char[dataSize];
    fread(buf, sizeof(ALuint), dataSize, fp);
    
    ALuint frequency = sampleRate;
    ALenum format = 0;
    
    alGenBuffers(1, &buffer);
    alGenSources(1, &source);
    
    if (bitsPerSample == 8)
    {
        if (channels == 1)
            format = AL_FORMAT_MONO8;
        else if (channels == 2)
            format = AL_FORMAT_STEREO8;
    }
    else if (bitsPerSample == 16)
    {
        if (channels == 1)
            format = AL_FORMAT_MONO16;
        else if (channels == 2)
            format = AL_FORMAT_STEREO16;
    }
    
    alBufferData(buffer, format, buf, dataSize, frequency);
    
    ALfloat SourcePos[] = { 0.0f, 0.0f, 0.0f };
    ALfloat SourceVel[] = { 0.0f, 0.0f, 0.0f };
    ALfloat ListenerPos[] = { 0.0f, 0.0f, 0.0f };
    ALfloat ListenerVel[] = { 0.0f, 0.0f, 0.0f };
    ALfloat ListenerOri[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
    
    // Listener
    alListenerfv(AL_POSITION, ListenerPos);
    alListenerfv(AL_VELOCITY, ListenerVel);
    alListenerfv(AL_ORIENTATION, ListenerOri);
    
    // Source
    alSourcei(source, AL_BUFFER, buffer);
    alSourcef(source, AL_PITCH, 1.0f);
    alSourcef(source, AL_GAIN, 1.0f);
    alSourcefv(source, AL_POSITION, SourcePos);
    alSourcefv(source, AL_VELOCITY, SourceVel);
    alSourcei(source, AL_LOOPING, AL_FALSE);
}
