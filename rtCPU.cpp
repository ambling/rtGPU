#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>
#include <math.h>
#include "rtCPU.h"
using namespace std;

int imWidth, imHeight, size;
int sphereNum, vertexNum, meshNum, materialNum;
Camera camera;
Sphere* spheres;
Vertex* vertices;
Mesh* meshes;
Material* materials;
Color* output;
unsigned int *pixels;

void readScene(string sceneFile)
{//get information from scene file
	sceneFile = "simple.scn";
	//sceneFile = "simplest.scn";
	std::ifstream in;
	in.open(sceneFile.c_str());
	std::string tmp;
	int sphereCnt = 0;
	int vertexCnt = 0;
	int meshCnt = 0;
	int materialCnt = 0;
	while(in>>tmp)
	{
		if(tmp == "camera")
		{
			in>>camera.orig.x>>camera.orig.y>>camera.orig.z
				>>camera.targ.x>>camera.targ.y>>camera.targ.z;
			
			updateCamera();
		}
		else if(tmp == "size")
		{
			in>>vertexNum>>meshNum>>sphereNum>>materialNum;
			vertices = new Vertex[vertexNum];
			meshes = new Mesh[meshNum];
			spheres = new Sphere[sphereNum];
			materials = new Material[materialNum];
		}
		else if(tmp == "sphere")
		{
			Sphere *p = &spheres[sphereCnt];
			in>>(p->rad);
			in>>(p->pos.x)>>(p->pos.y)>>(p->pos.z)
				>>(p->emi.x)>>(p->emi.y)>>(p->emi.z)
				>>(p->color.x)>>(p->color.y)>>(p->color.z);
			in>>p->ref;
			if(p->emi.x != 0 || p->emi.y != 0 || p->emi.z != 0)
			{ 
				//vNorm(p->emi);
				float max = p->emi.x;
				if(p->emi.y > max) max = p->emi.y;
				if(p->emi.z > max) max = p->emi.z;
				vMul(p->emi, p->emi, 1.0/max);
			}
			
			//vPrint(p->emi);
			sphereCnt ++;
		}
		else if(tmp == "vertex")
		{
			Vertex *v = &vertices[vertexCnt];
			in>>v->x>>v->y>>v->z;
			vertexCnt ++;
		}
		else if(tmp == "mesh")
		{
			Mesh *m = &meshes[meshCnt];
			in>>m->a>>m->b>>m->c>>m->ma;
			meshCnt ++;
		}
		else if(tmp == "material")
		{
			Material *p = &materials[materialCnt];
			in>>(p->emi.x)>>(p->emi.y)>>(p->emi.z)
				>>(p->color.x)>>(p->color.y)>>(p->color.z);
			in>>p->ref;
			
			if(p->emi.x != 0 || p->emi.y != 0 || p->emi.z != 0)
			{ 
				//vNorm(p->emi);
				float max = p->emi.x;
				if(p->emi.y > max) max = p->emi.y;
				if(p->emi.z > max) max = p->emi.z;
				vMul(p->emi, p->emi, 1.0/max);
			}
			
			materialCnt ++;
		}
	}
}

void updateCamera()
{
	vSub(camera.dirc, camera.targ, camera.orig);	//global function, camera direction
	vNorm(camera.dirc);

	vec3f up, distance;
	float angle = 45;
	vSub(distance, camera.targ, camera.orig);
	float dis = sqrt(vDot(distance, distance));
	float scale = tan(angle / 2.0) * dis * 2.0 / imWidth;
	//printf("distance is %f, scale is %f", dis, scale);

	up.x = 0.0; up.y = 1.0; up.z = 0.0;
	//vInit(up, 7.16376, -6.19517, 6.23901);
	vCross(camera.x, camera.dirc, up);				//global function, x base direction
	vNorm(camera.x);
	vMul(camera.x, camera.x, scale);
	vCross(camera.y, camera.x, camera.dirc);		//global function, y base direction
	vNorm(camera.y);
	vMul(camera.y, camera.y, scale);

	vec3f displace_x, displace_y;					//displacement from the target to base
	vMul(displace_x, camera.x, 1.0 * imWidth / 2);
	vMul(displace_y, camera.y, 1.0 * imHeight / 2);
	vSub(camera.base, camera.targ, displace_x);
	vSub(camera.base, camera.base, displace_y);		//base of the coordinate

	/*for debug
	vPrint(camera.orig);
	vPrint(camera.targ);
	vPrint(camera.dirc);
	vPrint(camera.x);
	vPrint(camera.y);
	vPrint(camera.base);
	vPrint(displace);
	*/
}

void putout()
{
	FILE *file = fopen("output", "w");
	fprintf(file, "%d %d\n", imWidth, imHeight);
	
	for(int i = 0; i < imWidth * imHeight; i++)
	{
		fprintf(file, "%.2f %.2f %.2f\n", output[i].x, output[i].y, output[i].z);
	}
	fclose(file);
}

void freeBuffer()
{
	if(spheres) delete spheres;
	if(vertices) delete vertices;
	if(meshes) delete meshes;
	if(materials) delete materials;
	if(output) delete output;
	if(pixels) free(pixels);
}

void rendering()
{
	for(int index = 0; index <= size; index++)
	{
		int w = index % imWidth;
		int h = index / imWidth;
		Color color;
		
		Ray ray= rayGenerate(camera, w, h);
		rayCasting(ray, sphereNum, vertexNum, 
					materialNum, meshNum, spheres, vertices, 
					materials, meshes, output+index);
		pixels[index] = (int)(255*output[index].x) |
					((int)(255*output[index].y) << 8) |
					((int)(255*output[index].z) << 16);
	}
}

void cpuMain(int width, int height)
{
	imWidth = width;
	imHeight = height;
	size = width * height;
	
	const int pixelCount = imWidth * imHeight ;
	pixels = (unsigned int*)malloc(sizeof(unsigned int[pixelCount]));	
	
	readScene();
	
	/*
	for(int i = 0; i < meshNum; i++)
	{
		printf("%d: %d %d %d\n", i, meshes[i].a, meshes[i].b, meshes[i].c);
	}
	*/
	
	output = new Color[size];
	rendering();

}

void ReInit(const int reallocBuffers) {
	// Check if I have to reallocate buffers
	if (reallocBuffers) {
		freeBuffer();
		const int pixelCount = imWidth * imHeight ;
		pixels = (unsigned int*)malloc(sizeof(unsigned int[pixelCount]));	
		output = new Color[size];
		readScene();
	}

	updateCamera();

	rendering();
}

void idleFunc(void) {
	rendering();

	glutPostRedisplay();
}

void displayFunc(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(0, 0);
	glDrawPixels(imWidth, imHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glutSwapBuffers();
}

void reshapeFunc(int newWidth, int newHeight) {
	imWidth = newWidth;
	imHeight = newHeight;

	glViewport(0, 0, imWidth, imHeight);
	glLoadIdentity();
	glOrtho(0.f, imWidth - 1.f, 0.f, imHeight - 1.f, -1.f, 1.f);

	ReInit(1);

	glutPostRedisplay();
}

void keyFunc(unsigned char key, int x, int y) {
	switch (key) {
		case 'p': {
			break;
		}
		default:
			break;
	}
}

#define MOVE_STEP 10.0
#define ROTATE_STEP (10.0 * 3.1415926 / 180.0)
void specialFunc(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP: {
			vec3f t = camera.targ;
			vSub(t, t, camera.orig);
			t.y = t.y * cos(-ROTATE_STEP) + t.z * sin(-ROTATE_STEP);
			t.z = -t.y * sin(-ROTATE_STEP) + t.z * cos(-ROTATE_STEP);
			vSub(t, camera.targ, t);
			camera.orig = t;
			ReInit(0);
			break;
		}
		case GLUT_KEY_DOWN: {
			vec3f t = camera.targ;
			vSub(t, t, camera.orig);
			t.y = t.y * cos(ROTATE_STEP) + t.z * sin(ROTATE_STEP);
			t.z = -t.y * sin(ROTATE_STEP) + t.z * cos(ROTATE_STEP);
			vSub(t, camera.targ, t);
			camera.orig = t;
			ReInit(0);
			break;
		}
		case GLUT_KEY_LEFT: {
			vec3f t = camera.targ;
			vSub(t, t, camera.orig);
			t.x = t.x * cos(-ROTATE_STEP) - t.z * sin(-ROTATE_STEP);
			t.z = t.x * sin(-ROTATE_STEP) + t.z * cos(-ROTATE_STEP);
			vSub(t, camera.targ, t);
			camera.orig = t;
			ReInit(0);
			break;
		}
		case GLUT_KEY_RIGHT: {
			vec3f t = camera.targ;
			vSub(t, t, camera.orig);
			t.x = t.x * cos(ROTATE_STEP) - t.z * sin(ROTATE_STEP);
			t.z = t.x * sin(ROTATE_STEP) + t.z * cos(ROTATE_STEP);
			vSub(t, camera.targ, t);
			camera.orig = t;
			ReInit(0);
			break;
		}
		case GLUT_KEY_PAGE_UP:
			camera.targ.y += MOVE_STEP;
			ReInit(0);
			break;
		case GLUT_KEY_PAGE_DOWN:
			camera.targ.y -= MOVE_STEP;
			ReInit(0);
			break;
		default:
			break;
	}
}

void initGlut(int argc, char **argv, string windowTittle)
{
    glutInitWindowSize(imWidth, imHeight);
    glutInitWindowPosition(0,0);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInit(&argc, argv);

	glutCreateWindow(windowTittle.c_str());

    glutReshapeFunc(reshapeFunc);
    glutKeyboardFunc(keyFunc);
    glutSpecialFunc(specialFunc);
    glutDisplayFunc(displayFunc);
	glutIdleFunc(idleFunc);

	glViewport(0, 0, imWidth, imHeight);
	glLoadIdentity();
	glOrtho(0.f, imWidth - 1.f, 0.f, imHeight - 1.f, -1.f, 1.f);
}
