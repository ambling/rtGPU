#include <iostream>
#include <string>
#include <CL/opencl.h>

#include "util.h"
#include "clclass.h"
#include "rtCPU.h"

using namespace std;


int main()
{
	int h, w;
	//cout<<"please input width and height:"<<endl;
	//cin>>w;
	//cin>>h;
	w = 800;
	h = 600;

	//cout<<"Choose device('g' for GPU, 'c' for CPU):";
	char c = 'c';
	//cin>>c;
	
	if(c == 'g')
	{
		CL rtGPU;
		rtGPU.loadProgram();
		rtGPU.dataPrepare(w, h);
		rtGPU.runKernel();
	
		rtGPU.putout();
	}
	else
	{
		cpuMain(w, h);
	}
}
