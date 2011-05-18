#include <iostream>
#include <string>
#include <CL/opencl.h>

#include "util.h"
#include "clclass.h"
#include "rtCPU.h"

using namespace std;


int main(int argc, char **argv)
{
	int h, w;
	//cout<<"please input width and height:"<<endl;
	//cin>>w;
	//cin>>h;
	w = 800;
	h = 600;

	//cout<<"Choose device('g' for GPU, 'c' for CPU):";
	char c = 'g';
	//cin>>c;
	
	if(c == 'g')
	{
		CL rtGPU;
		rtGPU.loadProgram();
		rtGPU.dataPrepare(w, h);
		rtGPU.runKernel();
	
		//rtGPU.putout();
		rtGPU.initGlut(argc, argv, "rtGPU(Written by Ambling)");
	}
	else
	{
		cpuMain(w, h);
		initGlut(argc, argv, "rtCPU(Written by Ambling)");
	}
	
	glutMainLoop();	
	return 0;
}
