#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>
#include <math.h>
#include "global.h"
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
			
			vSub(camera.dirc, camera.targ, camera.orig);	//global function, camera direction
			vNorm(camera.dirc);
			
			vec3f up, distance;
			float angle = 45;
			vSub(distance, camera.targ, camera.orig);
			float dis = sqrt(vDot(distance, distance));
			float scale = tan(angle / 2.0) * dis * 2.0 / imWidth;
			printf("distance is %f, scale is %f", dis, scale);
			
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


void cpuMain(int width, int height)
{
	imWidth = width;
	imHeight = height;
	imWidth = 800;
	imHeight = 600;
	size = width * height;
	
	readScene();
	
	for(int i = 0; i < meshNum; i++)
	{
		printf("%d: %d %d %d\n", i, meshes[i].a, meshes[i].b, meshes[i].c);
	}
	
	output = new Color[size];
	for(int index = 0; index <= size; index++)
	{
		int w = index % width;
		int h = index / width;
		Color color;
		
		Ray ray= rayGenerate(camera, w, h);
		rayCasting(ray, sphereNum, vertexNum, 
					materialNum, meshNum, spheres, vertices, 
					materials, meshes, output+index);
	}
	
	putout();
	
	if(spheres) delete spheres;
	if(vertices) delete vertices;
	if(meshes) delete meshes;
	if(materials) delete materials;
	if(output) delete output;
}
