#! /usr/bin/env python
import sys

filename = sys.argv[1]

infile = open(filename+".xml", 'r')
outfile = open(filename+".scn", 'w')

inAttr = 0 
#0 for nothing, 1 in camera, 2 in material, 
#3 in mesh, 4 in texture, 5 in smooth, 6 in light, 
#7 in background, 8 in integrator, 9 in render

vertexNum = 0
meshNum = 0
sphereNum = 0
materialNum = 0
mat = 0
materials = []

def run(i):
	global vertexNum
	global meshNum
	global sphereNum
	global materialNum
	global mat
	global materials
	if i == 1:
		orig = ['','','']
		targ = ['','','']
		line = infile.readline()
		while line != "</camera>\n":
			words = line.split(' ')
			if words[0] == '	<from':
				numbers = line.split('"')
				orig[0] = numbers[1]
				orig[1] = numbers[3]
				orig[2] = numbers[5]
			elif words[0] == '	<to':
				numbers = line.split('"')
				targ[0] = numbers[1]
				targ[1] = numbers[3]
				targ[2] = numbers[5]
			line = infile.readline()			
		output = "camera "+orig[0]+" "+orig[1]+" "+orig[2]+" "+targ[0]+" "+targ[1]+" "+targ[2]+'\n'
		outfile.write(output)
		inAttr = 0
	elif i == 2:
		r = ''
		g = ''
		b = ''
		line = infile.readline()
		while line != "</material>\n":
			words = line.split(' ')
			if words[0] == '	<color':
				numbers = line.split('"')
				r = numbers[1]
				g = numbers[3]
				b = numbers[5]
			line = infile.readline()			
		output = "material "+" 0 0 0 "+r+" "+g+" "+b+" 0"+'\n'
		outfile.write(output)
		materialNum += 1
		inAttr = 0
	elif i == 3:
		line = infile.readline()
		preVertex = vertexNum
		while line != "</mesh>\n":
			words = line.split(' ')
			if words[0] == '			<p':
				numbers = line.split('"')
				x = numbers[1]
				y = numbers[3]
				z = numbers[5]
				output = "vertex "+" "+x+" "+y+" "+z+'\n'
				outfile.write(output)
				vertexNum += 1
			elif words[0] == '			<f':
				numbers = line.split('"')
				a = int(numbers[1]) + preVertex
				b = int(numbers[3]) + preVertex
				c = int(numbers[5]) + preVertex
				output = "mesh "+" "+str(a)+" "+str(b)+" "+str(c)+" "+str(mat)+'\n'
				outfile.write(output)
				meshNum += 1
			elif words[0] == '			<set_material':
				name = line.split('"')
				i = 0
				for material in materials:
					if name == material:
						mat = i
						break
					i += 1
			line = infile.readline()		
		inAttr = 0
	elif i == 4:#do nothing
		line = infile.readline()
		while line != "</texture>\n":
			line = infile.readline()			
		inAttr = 0
	elif i == 5:#do nothing		
		inAttr = 0
	elif i == 6:#use light as a sphere
		orig = ['','','']
		color = ['','','']
		line = infile.readline()
		while line != "</light>\n":
			words = line.split(' ')
			if words[0] == '	<from':
				numbers = line.split('"')
				orig[0] = numbers[1]
				orig[1] = numbers[3]
				orig[2] = numbers[5]
			elif words[0] == '	<color':
				numbers = line.split('"')
				color[0] = numbers[1]
				color[1] = numbers[3]
				color[2] = numbers[5]
			line = infile.readline()			
		output = "sphere 0.0001 "+orig[0]+" "+orig[1]+" "+orig[2]+" 1 1 1 "+color[0]+" "+color[1]+" "+color[2]+" 0"+'\n'
		outfile.write(output)
		sphereNum += 1
		inAttr = 0
	elif i == 7:#use background as the lightsource
		orig = ['','','']
		line = infile.readline()
		while line != "</background>\n":
			words = line.split(' ')
			if words[0] == '	<from':
				numbers = line.split('"')
				orig[0] = numbers[1]
				orig[1] = numbers[3]
				orig[2] = numbers[5]
			line = infile.readline()			
		output = "sphere 0.0001 "+orig[0]+" "+orig[1]+" "+orig[2]+" 1 1 1 1 1 1 0"+'\n'
		outfile.write(output)
		sphereNum += 1
		inAttr = 0
	elif i == 8:#do nothing
		line = infile.readline()
		while line != "</integrator>\n":
			line = infile.readline()			
		inAttr = 0
	elif i == 9:#do nothing
		line = infile.readline()
		while line != "</render>\n":
			line = infile.readline()			
		inAttr = 0


line = infile.readline()
while line != '</scene>\n':
	words = line.split(' ')
	if words[0] == '<camera':
		inAttr = 1
		run(1)
	elif words[0] == '<material':
		name = line.split('"')
		materials.append(name[1])
		inAttr = 2
		run(2)
	elif words[0] == '<mesh':
		inAttr = 3
		run(3)
	elif words[0] == '<texture':
		inAttr = 4
		run(4)
	elif words[0] == '<smooth':
		inAttr = 5
		run(5)
	elif words[0] == '<light':
		inAttr = 6
		run(6)
	elif words[0] == '<background':
		inAttr = 7
		run(7)
	elif words[0] == '<integrator':
		inAttr = 8
		run(8)
	elif words[0] == '<render':
		inAttr = 9
		run(9)
	line = infile.readline()

outfile.close()
outfile = open(filename+".scn", 'r+')
output = "size "+str(vertexNum)+" "+str(meshNum)+" "+str(sphereNum)+" "+str(materialNum)+'\n'
outfile.write(output)
outfile.close()

