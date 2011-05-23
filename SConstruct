sources = ['main.cpp', 'clclass.cpp', 'util.cpp', 'kdtree.c', 'global.c', 'rtCPU.cpp']

env = Environment()
env.Program(target = 'rtGPU1', source = sources)
env.Append(CCFLAGS='-g')
env.Append(CPPPATH=['/usr/local/cuda/include'])
env.Append(LIBPATH=['/usr/local/cuda/lib'])
env.Append(LIBS = ['OpenCL', 'glut'])
