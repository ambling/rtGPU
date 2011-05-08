#include "global.h"
#ifndef GPU_KERNEL
#include <stdio.h>
#include <math.h>
#endif

float hitSphere(Ray ray, Sphere sphere)
{
	float r, a, b, c;
	r = sphere.rad;
	
	vec3f R1;
	vSub(R1, ray.orig, sphere.pos);		//R1 = R - P0
	
	#ifndef GPU_KERNEL
	//vPrint(ray.orig);
	//vPrint(sphere.pos);
	//vPrint(R1);
	#endif
	
	
	b = 2 * vDot(R1, ray.dirc);		//b = 2D*R1
	#ifndef GPU_KERNEL
	//printf("b = %.2f\n", b);
	#endif
	if(b > 0)						//no intersection
		return -1;
	
	a = vDot(ray.dirc, ray.dirc);	//a = D^2
	#ifndef GPU_KERNEL
	//printf("a = %.2f\n", a);
	#endif
	c = vDot(R1, R1) - r*r;			//c = R1^2 - r^2
	#ifndef GPU_KERNEL
	//printf("c = %.2f\n", c);
	#endif
	
	float delta = b*b - 4*a*c;
	#ifndef GPU_KERNEL
	//printf("delta = %.2f\n", delta);
	#endif
	if( delta < 0)				//no intersection
		return -1;
	
	if(c < 0)						//inside the sphere
		return (((0.0-b) + sqrt(delta))/2/a); 	
	
	return (((0.0-b) - sqrt(delta))/2/a); 	//others
}


float hitMesh(Ray ray, Vertex a, Vertex b, Vertex c)
{

}

void setColor(Ray ray, float t, int obSphere, int obMesh, 
	int sphereNum, int vertexNum, int materialNum, int meshNum, 
#ifdef GPU_KERNEL
__global
#endif
	Sphere* spheres, 
#ifdef GPU_KERNEL
__global
#endif
	Vertex* vertices, 
#ifdef GPU_KERNEL
__global
#endif
	Material* materials, 	
#ifdef GPU_KERNEL
__global
#endif
	Mesh* meshes, Color *color)
{
	if(obSphere != -1)
	{//is sphere
		if(spheres[obSphere].ref == 0)
		{//diffuse
			vAssign((*color), spheres[obSphere].emi);
	
			vec3f norm, pos;
			vMul(pos, ray.dirc, t);
			vAdd(pos, ray.orig, pos);		//position = R + tD
			vSub(norm, pos, spheres[obSphere].pos);		//normal = position - p0
			vNorm(norm);
	
			int i;
			for(i = 0; i < sphereNum; i++)
			{//find the light source
				if(i == obSphere) continue;
				if(spheres[i].emi.x == 0 && spheres[i].emi.y == 0 
					&& spheres[i].emi.z == 0)
					continue;
		
				vec3f light;
				vSub(light, spheres[i].pos, pos);	//light = pi - position
				vNorm(light);
				float d = vDot(norm, light);		//d = L * N
				if(d < 0) continue;
		
				Ray lt;
				vAssign(lt.orig, pos);
				vAssign(lt.dirc, light);
				float distance = hitSphere(lt, spheres[i]);
				int j;
				int isshadow = 0;
				for(j = 0; j < sphereNum; j++)
				{//check if is shadow
					if(j == obSphere) continue;
					if(j == i) continue;
					float dis = hitSphere(lt, spheres[j]);
					if(dis > 0 && dis < distance)
					{//is shadow
						isshadow = 1;
						break;
					}
				}
				if(isshadow == 1) continue;
		
				vec3f addcolor;
				vMul(addcolor, spheres[obSphere].color, d);
				vvMul(addcolor, addcolor, spheres[i].emi);
				vAdd((*color), (*color), addcolor);
			}
		}
		else if(spheres[obSphere].ref == 1)
		{//specula
	
		}
	}
	else if(obMesh != -1)
	{//is mesh
	
	}
}

Ray rayGenerate(
#ifdef GPU_KERNEL
__constant
#endif
	Camera camera, int w, int h)
{
	Ray ray;
	ray.orig.x = camera.orig.x;
	ray.orig.y = camera.orig.y;
	ray.orig.z = camera.orig.z;
	
	vec3f tmp, targ; //targ = base + wx + hy
	vMul(tmp, camera.x, 1.0 * w);
	vAdd(targ, camera.base, tmp);
	vMul(tmp, camera.y, 1.0 * h);
	vAdd(targ, targ, tmp);
	
	vSub(ray.dirc, targ, camera.orig); //dirction = target - origin
	vNorm(ray.dirc);
	
	return ray;
}

void rayCasting(Ray ray, int sphereNum, int vertexNum, 
	int materialNum, int meshNum, 
#ifdef GPU_KERNEL
__global
#endif
	Sphere* spheres, 
#ifdef GPU_KERNEL
__global
#endif
	Vertex* vertices, 
#ifdef GPU_KERNEL
__global
#endif
	Material* materials, 	
#ifdef GPU_KERNEL
__global
#endif
	Mesh* meshes, Color *color)
{
	int i;
	int hitOrNot = 0;
	int minIndex;
	for(i = 0; i < sphereNum; i++)
	{
		float hitIndex = hitSphere(ray, spheres[i]);
		#ifndef GPU_KERNEL
		//printf("%.2f\n", hitIndex);
		#endif
		if(hitIndex > 0) 
		{	
			if(hitOrNot == 0)
			{
				minIndex = hitIndex;
				setColor(ray, hitIndex, i, -1, sphereNum, vertexNum, 
						materialNum, meshNum, spheres, vertices, 
						materials, meshes, color);
				hitOrNot = 1;
			}
			else
			{
				if(hitIndex < minIndex)
				{
					minIndex = hitIndex;
					setColor(ray, hitIndex, i, -1, sphereNum, vertexNum, 
						materialNum, meshNum, spheres, vertices, 
						materials, meshes, color);
				}
			}			
		}
	}
	
	for(i = 0; i < meshNum; i++)
	{
		float hitIndex = hitMesh(ray, vertices[meshes[i].a], 
									vertices[meshes[i].b],
									vertices[meshes[i].c]);
		#ifndef GPU_KERNEL
		//printf("%.2f\n", hitIndex);
		#endif
		if(hitIndex > 0) 
		{	
			if(hitOrNot == 0)
			{
				minIndex = hitIndex;
				setColor(ray, hitIndex, -1, i, sphereNum, vertexNum, 
						materialNum, meshNum, spheres, vertices, 
						materials, meshes, color);
				hitOrNot = 1;
			}
			else
			{
				if(hitIndex < minIndex)
				{
					minIndex = hitIndex;
					setColor(ray, hitIndex, -1, i, sphereNum, vertexNum, 
						materialNum, meshNum, spheres, vertices, 
						materials, meshes, color);
				}
			}			
		}
	}
	
	if(hitOrNot == 0)
	{//not intersection, set black
		color->x = 0;
		color->y = 0;
		color->z = 0;
	}
}

