#ifndef RTCPU_H
#define RTCPU_H
#include <iostream>
#include <GL/glut.h>

#include "global.h"
#include "global.c"

using namespace std;

void readScene(string sceneFile);
void putout();
void updateCamera();
void freeBuffer();
void cpuMain(int width, int height, string sceneFile = "evensimpler.scn");
void initGlut(int, char **, string);
#endif
