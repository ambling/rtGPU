
#define GPU_KERNEL

#include "global.h"
#include "global.c"

__kernel void test(const int width, const int sphereNum, const int vertexNum, 
	const int materialNum, const int meshNum, 
		__constant Camera* camera, 
		__global Sphere* spheres, 
		__global Vertex* vertices, 
		__global Material* materials,
		__global Mesh* meshes,
		__global Color* output)
{
    unsigned int index = get_global_id(0);
    int w = index % width;
    int h = index / width;
    Color color;
    
    vInit(color, 0, 0, 0);  
    
    Ray ray= rayGenerate(*camera, w, h);
	rayCasting(ray, sphereNum, vertexNum, 
					materialNum, meshNum, spheres, vertices, 
					materials, meshes, &color);
	
	output[index].x = color.x;
	output[index].y = color.y;
	output[index].z = color.z;
}

