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


/*
Vec3f origin = r.getOrigin();
	Vec3f direct = r.getDirection();
	float isParallel = normal.Dot3(direct);
	Vec3f tempbeta0;
	Vec3f tempbeta1;
	Vec3f tempbeta2;
	float beta0;
	float beta1;
	float beta2;
	if(fabs(isParallel)>0.0f)
	{
		float dist = -(normal.Dot3(origin)+d)/isParallel;
		Vec3f q = origin + direct*dist;
		Vec3f::Cross3(tempbeta0,(c-b),(q-b));
		Vec3f::Cross3(tempbeta1,(a-c),(q-c));
		Vec3f::Cross3(tempbeta2,(b-a),(q-a));
		if(i0 == 0)
		{
			beta0 = tempbeta0.x()/normal.x();	
			beta1 = tempbeta1.x()/normal.x();
			beta2 = tempbeta2.x()/normal.x();
		}
		else if(i0 == 1)
		{
			beta0 = tempbeta0.y()/normal.y();
			beta1 = tempbeta1.y()/normal.y();
			beta2 = tempbeta2.y()/normal.y();
		}
		else
		{
			beta0 = tempbeta0.z()/normal.z();
			beta1 = tempbeta1.z()/normal.z();
			beta2 = tempbeta2.z()/normal.z();
		}
		if(beta0>=0 && beta0<=1 && beta1>=0 && beta1<=1 && beta2>=0 && beta2<=1)
		{
			if(dist > tmin && dist < h.getT())
			{
				h.set(dist,material,normal,r);
				return true;	
			}
		}
	}
	return false;
}
*/
float hitMesh(Ray ray, Vertex a, Vertex b, Vertex c)
{
	///*
	float tmpA, beta, gama, t;
	vec3f t1, t2, t3, norm;
	vSub(t1, a, b);
	vSub(t2, a, c);
	vSub(t3, a, ray.orig)
	
	tmpA = det3v(t1, t2, ray.dirc);
	beta = det3v(t3, t2, ray.dirc) / tmpA;
	gama = det3v(t1, t3, ray.dirc) / tmpA;
	
	if(beta + gama <= 1 && beta >= 0 && gama >= 0)
	{//intersected
		t = det3v(t1, t2, t3) / tmpA;
		#ifndef GPU_KERNEL
		//printf("hit mesh, t = %.2f\n", t);
		#endif
		return t;
	}
	//*/
	/*
	float t;			
	vec3f ba, ca, norm;
	vSub(ba, a, b);
	vSub(ca, a, c);
	vCross(norm, ba, ca);
	//vNorm(norm);		//P.N - a.N = 0, on the plane
	#ifndef GPU_KERNEL
	//vPrint(norm);
	#endif	
	float isParallel = vDot(norm, ray.dirc);
	if(fabs(isParallel) > EPSILON)
	{
		t = (vDot(a, norm) - vDot(ray.orig, norm)) / isParallel;
		vec3f p, t0, t1, t2, tmp1, tmp2;
		vMul(tmp1, ray.dirc, t);
		vAdd(p, tmp1, ray.orig);		//P = R + tD, on the ray
		
		vSub(tmp1, c, b);
		vSub(tmp2, p, b);
		vCross(t0, tmp1, tmp2);
		vSub(tmp1, a, c);
		vSub(tmp2, p, c);
		vCross(t1, tmp1, tmp2);
		vSub(tmp1, b, a);
		vSub(tmp2, p, a);
		vCross(t2, tmp1, tmp2);

		float alpha, beta, gama;
		if(fabs(norm.x) >= fabs(norm.y) && fabs(norm.x) >= fabs(norm.z))
		{
			alpha = t0.x / norm.x;
			beta = t1.x / norm.x;
			gama = t2.x / norm.x;
		}
		else if(fabs(norm.y) >= fabs(norm.x) && fabs(norm.y) >= fabs(norm.z))
		{
			alpha = t0.y / norm.y;
			beta = t1.y / norm.y;
			gama = t2.y / norm.y;
		}
		else
		{
			alpha = t0.z / norm.z;
			beta = t1.z / norm.z;
			gama = t2.z / norm.z;
		}
		#ifndef GPU_KERNEL
		printf("alpha = %.2f, beta = %.2f, gama = %.2f, t = %.2f\n", alpha, beta, gama, t);
		#endif	
		if(alpha >= 0 && alpha <=1 && beta >= 0 && beta <=1 && gama >= 0 && gama <=1)
		{//intersected
			return t;
		}
	}
	//*/
	return -1;		//no intersection
}

int isShadow(int i, int obSphere, int obMesh, float distance, 
	int sphereNum, int vertexNum, int meshNum, vec3f pos, vec3f light, 
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
	Mesh* meshes)
{
	Ray lt;
	vAssign(lt.orig, pos);
	vMul(lt.dirc, light, -1);
	//float distance = hitSphere(lt, spheres[i]);		//not suit for dot light
	
	
	int j;
	int isshadow = 0;
	for(j = 0; j < sphereNum; j++)
	{//check if is shadow by sphere
		if(j == obSphere) continue;
		if(j == i) continue;
		float dis = hitSphere(lt, spheres[j]);
		if(dis > EPSILON && dis < distance)
		{//is shadow
			isshadow = 1;
			return isshadow;
		}
	}
	///*
	for(j = 0; j < meshNum; j++)
	{//check if is shadow by mesh
		if(j == obMesh) continue;
		float dis = hitMesh(lt, vertices[meshes[j].a], 
						vertices[meshes[j].b],
						vertices[meshes[j].c]);
		if(dis > EPSILON && dis < distance)
		{//is shadow
			isshadow = 1;
			return isshadow;
		}
	}
	//*/
	//if(isshadow == 1) continue;	
	return isshadow;
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
	vec3f background;
	vInit(background, 0.1, 0.1, 0.1);
	
	if(obSphere != -1)
	{//is sphere
		#ifndef GPU_KERNEL
		//printf("hit sphere, t = %.2f\n", t);
		#endif
		if(spheres[obSphere].ref == 0)
		{//diffuse
			vAssign((*color), spheres[obSphere].emi);
			vAdd((*color), (*color), background);
	
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
				vSub(light, pos, spheres[i].pos);	//light = position - pi
				float distance = sqrt(vDot(light, light));	//for dot light source
				vNorm(light);
				float d = vDot(norm, light);		//d = L x N
				if(d > EPSILON) continue;
		
				if(isShadow(i, obSphere, -1, distance, sphereNum, vertexNum, meshNum, 
					pos, light, spheres, vertices, meshes) == 1) continue;
		
				vec3f addcolor;
				vMul(addcolor, spheres[obSphere].color, (-1 * d));
				vvMul(addcolor, addcolor, spheres[i].emi);
				vAdd((*color), (*color), addcolor);
			}
		}
		else if(spheres[obSphere].ref == 1)
		{//specula
	
		}
	}
	else if(obMesh != -1)
	{//is mesh;
		//vAssign((*color), materials[meshes[obMesh].ma].color);
		///*
		#ifndef GPU_KERNEL
		//printf("hit mesh, t = %.2f\n", t);
		#endif
		if(materials[meshes[obMesh].ma].ref == 0)
		{//diffuse
			vAssign((*color), materials[meshes[obMesh].ma].emi);
			vAdd((*color), (*color), background);

			vec3f norm, pos, pa, pb;
			vMul(pos, ray.dirc, t);
			vAdd(pos, ray.orig, pos);		//position = R + tD
			vSub(pa, vertices[meshes[obMesh].a], pos);	//pa = a - p
			vSub(pb, vertices[meshes[obMesh].b], pos);	//pa = a - p
			vCross(norm, pa, pb);
			#ifndef GPU_KERNEL
			//printf("norm: ");vPrint(norm);
			//printf("ray.dirc: ");vPrint(ray.dirc);
			#endif
			if(vDot(norm, ray.dirc) > EPSILON) vMul(norm, norm, -1.0); //set the direction of norm
			vNorm(norm);
	
			int i;			
			for(i = 0; i < sphereNum; i++)
			{//find the light source
				if(spheres[i].emi.x == 0 && spheres[i].emi.y == 0 
					&& spheres[i].emi.z == 0)
					continue;
		
				vec3f light;
				vSub(light, pos, spheres[i].pos);	//light = position - pi
				float distance = sqrt(vDot(light, light));	//for dot light source
				vNorm(light);
				float d = vDot(norm, light);		//d = L x N
				if(d > 0) continue;
			
				if(isShadow(i, -1, obMesh, distance, sphereNum, vertexNum, meshNum, pos, light, spheres, vertices, meshes)) continue;
				
				vec3f addcolor;
				vMul(addcolor, materials[meshes[obMesh].ma].color, (-1.0 * d));
				vvMul(addcolor, addcolor, spheres[i].emi);
				vAdd((*color), (*color), addcolor);
			}

		}
		else if(materials[meshes[obMesh].ma].ref == 1)
		{//specular
		
		}
		else if(materials[meshes[obMesh].ma].ref == 2)
		{
		
		}
		//*/
	}
	
}

Ray rayGenerate(
#ifdef GPU_KERNEL
__constant
#endif
	Camera camera, int w, int h)
{
	Ray ray;
	vAssign(ray.orig, camera.orig);
	
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
	int minSphere = -1;
	for(i = 0; i < sphereNum; i++)
	{
		float hitIndex = hitSphere(ray, spheres[i]);
		if(hitIndex > 0) 
		{	
			if(hitOrNot == 0)
			{
				minIndex = hitIndex;
				minSphere = i;
				hitOrNot = 1;
			}
			else
			{
				if(hitIndex < minIndex)
				{
					minIndex = hitIndex;
					minSphere = i;
				}
			}			
		}
	}	
	
	int minMesh = -1;
	for(i = 0; i < meshNum; i++)
	{
		float hitIndex = hitMesh(ray, vertices[meshes[i].a], 
									vertices[meshes[i].b],
									vertices[meshes[i].c]);
		#ifndef GPU_KERNEL
		//printf("hit mesh: %d\n", meshNum);
		#endif
		if(hitIndex > 0) 
		{	
			if(hitOrNot == 0)
			{
				minIndex = hitIndex;
				minMesh = i;
				hitOrNot = 1;
			}
			else
			{
				if(hitIndex < minIndex)
				{
					minIndex = hitIndex;
					minMesh = i;
					minSphere = -1;
				}
			}			
		}
	}
	if((minMesh != -1) || (minSphere != -1))
	{
		#ifndef GPU_KERNEL
		printf("hit mesh: %d\n", minMesh);
		#endif
		Color sample[12];
		vInit(sample[0], 1, 1 ,1);
		vInit(sample[1], 0, 1 ,0);
		vInit(sample[2], 0, 0 ,1);
		vInit(sample[3], 1, 0 ,0);
		vInit(sample[4], 0, 1 ,1);
		vInit(sample[5], 1, 1 ,0);
		vInit(sample[6], 1, 0 ,1);
		vInit(sample[7], 0, 0 ,0.5);
		vInit(sample[8], 0, 0.5 ,0);
		vInit(sample[9], 0.5, 0 ,0);
		vInit(sample[10], 0.5, 0.5 ,0);
		vInit(sample[11], 0, 0.5 ,0.5);

		vAssign((*color), sample[minMesh]);
		//setColor(ray, minIndex, minSphere, minMesh, sphereNum, vertexNum, 
		//				materialNum, meshNum, spheres, vertices, 
		//				materials, meshes, color);
	}
	else
	{//not intersection, set black
		color->x = 0;
		color->y = 0;
		color->z = 0;
	}
}
