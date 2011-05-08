#! /usr/bin/env python
import sys
import os
import Image
import time

start = time.time()
os.system("./rtGPU1 > debug")
stop = time.time()

print "time elapses: " + str(stop - start)

infile = open("output")

line = infile.readline()
word = line.split(" ")
width = int(word[0])
height = int(word[1][:-1])

#print width
#print height

im = Image.new("RGB", (width, height))
pix = im.load()

for i in range(width*height):
	line = infile.readline()
	word = line.split(" ")
	r = int(255*float(word[0]))
	g = int(255*float(word[1]))
	b = int(255*float(word[2]))
	pix[i % width, i / width] = (r, g, b)
	
im.show()
im.save("output.png")
