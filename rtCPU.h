#ifndef RTCPU_H
#define RTCPU_H
#include <iostream>
#include <GL/glut.h>

#include "global.h"

using namespace std;

void readScene(string sceneFile = "evensimpler.scn");
void putout();
void updateCamera();
void freeBuffer();
void cpuMain(int width, int height);
void initGlut(int, char **, string);
#endif
