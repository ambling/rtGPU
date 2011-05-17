#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>
#include "clclass.h"
#include "global.h"
#include "global.c"
#include "util.h"
#include <math.h>

CL::CL()
{
	//this function is defined in util.cpp
	//it comes from the NVIDIA SDK example code
	err = oclGetPlatformID(&platform);
	if(err != CL_SUCCESS)
	{
		printf("oclGetPlatformID: %s\n", oclErrorString(err));
		exit(-1);
	}
	
	cl_uint numDevices;
	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
	if(err != CL_SUCCESS)
	{
		printf("clGetDeviceIDs (get number of devices): %s\n", oclErrorString(err));
		exit(-1);
	}
	devices = new cl_device_id[numDevices];
	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
	if(err != CL_SUCCESS)
	{
		printf("clGetDeviceIDs (create device list): %s\n", oclErrorString(err));
		exit(-1);
	}
	
	context = clCreateContext(0, 1, devices, NULL, NULL, &err);
	if(err != CL_SUCCESS)
	{
		printf("clCreateContext : %s\n", oclErrorString(err));
		exit(-1);
	}
	device_used = 0;
	command_queue = clCreateCommandQueue(context, devices[device_used], 0, &err);
	if(err != CL_SUCCESS)
	{
		printf("clCreateCommandQueue : %s\n", oclErrorString(err));
		exit(-1);
	}
	
	//printf("OpenCL initialized successful.\n");
}

CL::~CL()
{
	if(kernel) clReleaseKernel(kernel); 
	if(program) clReleaseProgram(program);
	if(command_queue) clReleaseCommandQueue(command_queue);
   
	//need to release any other OpenCL memory objects here

	if(context) clReleaseContext(context);
	
	if(devices) delete devices;	
	if(spheres) delete spheres;
	if(vertices) delete vertices;
	if(meshes) delete meshes;
	if(materials) delete materials;
	if(output)	delete output;
}

void CL::loadProgram()
{
	sources = readKernelSources();
	//printf("kernel: \n%s", sources);
	//std::cout<<sources; //print the kernel source for debugging
	
	program = clCreateProgramWithSource(context, 1, &sources, 0, &err);
	if(err != CL_SUCCESS)
	{
		printf("clCreateProgramWithSource : %s\n", oclErrorString(err));
		exit(-1);
	}
	
	err = clBuildProgram(program, 1, devices, "-I. ", NULL, NULL);	
	///* for debug
	size_t len;
	char *buffer;
	clGetProgramBuildInfo(program, devices[device_used], CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
	buffer = (char *)malloc(len);
	clGetProgramBuildInfo(program, devices[device_used], CL_PROGRAM_BUILD_LOG, len, buffer, NULL);
	printf("%s\n", buffer);
	//*/
	if(err != CL_SUCCESS)
	{
		printf("clBuildProgram : %s\n", oclErrorString(err));
		exit(-1);
	}	
	
	kernel = clCreateKernel(program, "test", &err);
	if(err != CL_SUCCESS)
	{
		printf("clCreateKernel : %s\n", oclErrorString(err));
		exit(-1);
	}		
	
}

char* CL::readKernelSources()
{
	FILE *file = fopen("kernel.cl", "r");
	fseek(file, 0, SEEK_END);
	long long int size = ftell(file);
	rewind(file);
	
	char *source = (char *) malloc(sizeof(char) * size + 1);
	fread(source, 1, sizeof(char) * size, file);
	source[size] = '\0';
	//printf ("%s", source);
	
	fclose(file);
	return source;
	/*
	std::ifstream file;
	std::string tmp, source;
	file.open("kernel.cl");
	while(! file.eof())
	{
		std::getline(file, tmp);
		tmp += '\n';
		source.append(tmp);
	}
	//std::cout<<"kernel source:"<<std::endl;
	//std::cout<<source; //print the kernel source for debugging
	return source.c_str();
	*/
}

void CL::dataPrepare(int w, int h, std::string sceneFile)
{
	imWidth = w;
	imHeight = h;
	
	readScene(sceneFile); //default as simple.scn
	
	output = new Color[w*h];
	for(int i = 0; i < w*h; i++)
	{
		output[i].x = 0;
		output[i].y = 0;
		output[i].z = 0;
	}
	
	//create buffer for kernel
	outputBuf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(Color) * imWidth * imHeight, NULL, NULL);
	cameraBuf = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(Camera), &camera, NULL);
	sphereBuf = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(Sphere) * sphereNum, spheres, NULL);
	vertexBuf = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(Vertex) * vertexNum, vertices, NULL);
	meshBuf = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(Mesh) * meshNum, meshes, NULL);
	materialBuf = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(Material) * materialNum, materials, NULL);

	
	clEnqueueWriteBuffer(command_queue, outputBuf, CL_TRUE, 0, sizeof(Color)*w*h, (void*)output, 0, NULL, &event);
	clReleaseEvent(event);
	clFinish(command_queue);
	clEnqueueWriteBuffer(command_queue, cameraBuf, CL_TRUE, 0, sizeof(Camera), (void*)&camera, 0, NULL, &event);
	clReleaseEvent(event);
	clFinish(command_queue);
	clEnqueueWriteBuffer(command_queue, sphereBuf, CL_TRUE, 0, sizeof(Sphere) * sphereNum, 
							(void*)spheres, 0, NULL, &event);
	clReleaseEvent(event);
	clFinish(command_queue);
	clEnqueueWriteBuffer(command_queue, vertexBuf, CL_TRUE, 0, sizeof(Vertex) * vertexNum, 
							(void*)vertices, 0, NULL, &event);
	clReleaseEvent(event);
	clFinish(command_queue);
	clEnqueueWriteBuffer(command_queue, meshBuf, CL_TRUE, 0, sizeof(Mesh) * meshNum, 
							(void*)meshes, 0, NULL, &event);
	clReleaseEvent(event);
	clFinish(command_queue);
	clEnqueueWriteBuffer(command_queue, materialBuf, CL_TRUE, 0, sizeof(Material) * materialNum, 
							(void*)materials, 0, NULL, &event);
	clReleaseEvent(event);
	clFinish(command_queue);
	
	/*
	__kernel void test(const int width, const int sphereNum, const int vertexNum, 
		const int materialNum, const int meshNum, 
		__constant Camera* camera, 
		__global Sphere* spheres, 
		__global Vertex* vertices, 
		__global Material* materials,
		__global Mesh* meshes,
		__global Color* output)
	 */
	clSetKernelArg(kernel, 0, sizeof(int), (void* )&imWidth);
	clSetKernelArg(kernel, 1, sizeof(int), (void* )&sphereNum);
	clSetKernelArg(kernel, 2, sizeof(int), (void* )&vertexNum);
	clSetKernelArg(kernel, 3, sizeof(int), (void* )&materialNum);
	clSetKernelArg(kernel, 4, sizeof(int), (void* )&meshNum);
	clSetKernelArg(kernel, 5, sizeof(cl_mem), (void* )&cameraBuf);
	clSetKernelArg(kernel, 6, sizeof(cl_mem), (void* )&sphereBuf);
	clSetKernelArg(kernel, 7, sizeof(cl_mem), (void* )&vertexBuf);
	clSetKernelArg(kernel, 8, sizeof(cl_mem), (void* )&materialBuf);
	clSetKernelArg(kernel, 9, sizeof(cl_mem), (void* )&meshBuf);
	clSetKernelArg(kernel, 10, sizeof(cl_mem), (void* )&outputBuf);
	clFinish(command_queue);
}

void CL::readScene(std::string sceneFile)
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
			//printf("scale is %.2f", scale);
			
			up.x = 0.0; up.y = 1.0; up.z = 0.0;
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


void CL::runKernel()
{
	size_t size = imWidth * imHeight;
	
	err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &size, NULL, 0, NULL, &event);
	if(err != CL_SUCCESS)
	{
		printf("clEnqueueNDRangeKernel : %s\n", oclErrorString(err));
		clReleaseEvent(event);
		exit(-1);
	}
	clReleaseEvent(event);
	err = clFinish(command_queue);
	if(err != CL_SUCCESS)
	{
		printf("clFinish : %s\n", oclErrorString(err));
		exit(-1);
	}
	///*
	err = clEnqueueReadBuffer(command_queue, outputBuf, CL_TRUE, 0, sizeof(Color) * size, output, 0, NULL, &event);
	if(err != CL_SUCCESS)
	{
		printf("clEnqueueReadBuffer : %s\n", oclErrorString(err));
		clReleaseEvent(event);
		exit(-1);
	}
	clReleaseEvent(event);
	clFinish(command_queue);
	//*/
	/*
	for(int i = 0; i < size; i++)
	{
		std::cout<<"No.: "<<i<<std::endl;
		std::cout<<output[i].x<<std::endl;
		std::cout<<output[i].y<<std::endl;
		std::cout<<output[i].z<<std::endl;
	}
	
	*/
	
	/*for test
	Camera c;
	clEnqueueReadBuffer(command_queue, cameraBuf, CL_TRUE, 0, sizeof(Camera), &c, 0, NULL, &event);
	clReleaseEvent(event);
	printf("camera %f %f %f  %f %f %f  %f %f %f\n",
			c.orig.x, c.orig.y, c.orig.z,
			c.targ.x, c.targ.y, c.targ.z,
			c.dirc.x, c.dirc.y, c.dirc.z);
	//*/
}

void CL::putout()
{
	FILE *file = fopen("output", "w");
	fprintf(file, "%d %d\n", imWidth, imHeight);
	
	for(int i = 0; i < imWidth * imHeight; i++)
	{
		fprintf(file, "%.2f %.2f %.2f\n", output[i].x, output[i].y, output[i].z);
	}
	fclose(file);
}

