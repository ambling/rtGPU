//this is the header file for opencl class definition 

#ifndef CLCLASS_H
#define CLCLASS_H

#include "CL/cl.h"
#include "global.h"

class CL
{
public:
	CL();	
	~CL();
	
	void loadProgram();
	
	void dataPrepare(int, int, std::string = "simple.scn");
	
	void runKernel();	
	
	void putout();	
	
private:
	//variables for architecture
	cl_platform_id platform;
	
	cl_device_id* devices;
	unsigned int device_used;
	
	cl_context context;
	cl_command_queue command_queue;
	
	cl_kernel kernel;
	const char* sources;
	cl_program program;
	
	cl_int err;
	cl_event event;
	
	//variables for memory
	int imWidth, imHeight;
	int sphereNum, vertexNum, meshNum, materialNum;
	Camera camera;
	Sphere* spheres;
	Vertex* vertices;
	Mesh* meshes;
	Material* materials;
	Color* output;
	cl_mem outputBuf; //buffer for storing the output color information
	cl_mem cameraBuf; //buffer for storing the camera information
	cl_mem sphereBuf; //buffer for storing the spheres information
	cl_mem vertexBuf;
	cl_mem meshBuf;
	cl_mem materialBuf;
	
	//functions for temporarily using
	char* readKernelSources();
	void readScene(std::string);
};

#endif
