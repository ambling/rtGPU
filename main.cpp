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
	CL rtGPU;
	//cout<<"please input width and height:"<<endl;
	//cin>>w;
	//cin>>h;
	w = 800;
	h = 600;

	//cout<<"Choose device('g' for GPU, 'c' for CPU):";
	char c;
	//cin>>c;
	c = argv[1][0];
	string scenefile(argv[2]);
	//cout<<c<<endl;
	//cout<<scenefile<<endl;
	
	
	if(c == 'g')
	{
		rtGPU.loadProgram();
		rtGPU.dataPrepare(w, h, scenefile);
		rtGPU.runKernel();
	
		//rtGPU.putout();
		rtGPU.initGlut(argc, argv, "rtGPU(Written by Ambling)");
	}
	else
	{
		cpuMain(w, h, scenefile);
		initGlut(argc, argv, "rtCPU(Written by Ambling)");
	}
	
	glutMainLoop();	
	return 0;
}
