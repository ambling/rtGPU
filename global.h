//this file contains the definition of global types and functions 

#ifndef STRUCTS_H
#define STRUCTS_H

#ifndef GPU_KERNEL
#include <stdio.h>
//#include <math.h>

#if defined(__linux__) || defined(__APPLE__)
#include <sys/time.h>
#elif defined (WIN32)
#include <windows.h>
#else
        Unsupported Platform !!!
#endif
#endif

//global types
typedef struct
{
	float x;
	float y;
	float z;
} vec3f;

typedef vec3f Color;
typedef vec3f Vertex;

typedef struct
{
	vec3f orig;
	vec3f dirc;
} Ray;

typedef struct
{
	vec3f orig;
	vec3f targ;
	vec3f dirc;
	vec3f base, x, y;
} Camera;

typedef struct 
{
	float rad; //radius 
	vec3f pos, emi, color;  // position, emission, color 
	int ref;  //reflection type, 0 for DIFF, 1 for SPEC, 2 for REFR
	float outrfr, inrfr;	//the index of refraction for space outside and inside the sphere 
} Sphere;

typedef struct
{
	vec3f emi, color;  // position, emission, color 
	int ref;  //reflection type, 0 for DIFF, 1 for SPEC, 2 for REFR
	float outrfr, inrfr;	//the index of refraction for space outside and inside the mesh 
} Material;

typedef struct
{
	int a, b, c; 	//the Number of vertex
	int ma;			//the Number of material
} Mesh;

// for kd-tree
typedef struct
{
	int axis;	//0 for x, 1 for y, 2 for z
	float value;
} Plane;

typedef struct KDTreeNodeName
{
	struct KDTreeNodeName* parent;
	struct KDTreeNodeName* leftChild;
	struct KDTreeNodeName* rightChild;
	struct KDTreeNodeName* sibling;
	
	int isleaf;		//the value is 1 if is the leaf node, otherwise 0
	int depth;
	vec3f start, end;	//bounding box
	Plane* plane;	//information of the plane, NULL if is the leaf node
	int* meshes;	//record the index of meshes that in this box
	//int meshCnt;	//number of meshes
} KDTreeNode;

// global functions

#define EPSILON 0.001
#define vPrint(v) { printf("%.2f %.2f %.2f\n", (v).x, (v).y, (v).z); }
#define vInit(v, a, b, c) { (v).x = a; (v).y = b; (v).z = c; }
#define vAssign(a, b) vInit(a, (b).x, (b).y, (b).z)
#define vAdd(v, a, b) vInit(v, (a).x + (b).x, (a).y + (b).y, (a).z + (b).z)
#define vSub(v, a, b) vInit(v, (a).x - (b).x, (a).y - (b).y, (a).z - (b).z)
#define vMul(v, a, b) { float k = (b); vInit(v, k * (a).x, k * (a).y, k * (a).z) }
#define vvMul(v, a, b) vInit(v, (a).x * (b).x, (a).y * (b).y, (a).z * (b).z)
#define vDot(a, b) ((a).x * (b).x + (a).y * (b).y + (a).z * (b).z)
#define vNorm(v) { float l = 1.f / sqrt(vDot(v, v)); vMul(v, v, l); }
#define vCross(v, a, b) vInit(v, (a).y * (b).z - (a).z * (b).y, (a).z * (b).x - (a).x * (b).z, (a).x * (b).y - (a).y * (b).x)
#define det3v(a, b, c) ((a).x*(b).y*(c).z + (b).x*(c).y*(a).z + (c).x*(a).y*(b).z - (c).x*(b).y*(a).z - (a).x*(c).y*(b).z - (b).x*(a).y*(c).z)

#ifndef GPU_KERNEL
static double WallClockTime() {
#if defined(__linux__) || defined(__APPLE__)
	struct timeval t;
	gettimeofday(&t, NULL);

	return t.tv_sec + t.tv_usec / 1000000.0;
#elif defined (WIN32)
	return GetTickCount() / 1000.0;
#else
	Unsupported Platform !!!
#endif
}
#endif


#ifndef GPU_KERNEL
static float hitSphere(Ray ray, Sphere sphere);
static float hitMesh(Ray ray, Vertex a, Vertex b, Vertex c);
static Ray setColor(KDTreeNode* root, Ray ray, float t, int obSphere, int obMesh, int sphereNum, 
	int vertexNum, int materialNum, int meshNum, Sphere* spheres, 
	Vertex* vertices, Material* materials, Mesh* meshes, Color *color);
static Ray rayGenerate(Camera camera, int w, int h);
static void rayCasting(KDTreeNode* root, Ray ray, int sphereNum, int vertexNum, 
	int materialNum, int meshNum, Sphere* spheres, 
	Vertex* vertices, Material* materials, Mesh* meshes, Color *color);
#endif	

#endif
