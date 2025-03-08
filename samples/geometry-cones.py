#!/usr/bin/pyton3

import pynari as anari
import random
import math
import numpy as np
import matplotlib.pyplot as plt
import sys, getopt
import PIL

print('@pynari: -------------------------------------------------------')
print('@pynari: running geometry-cones sample')
print('@pynari: renders a set of random ANARI "cones" geometries  ')
print('@pynari: -------------------------------------------------------')

def dot(a,b):
    return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]

def normalize(a):
    return a * (1./math.sqrt(dot(a,a)))

def randomf():
    return random.uniform(0.,1.)

def random3f():
    return np.array([randomf(),randomf(),randomf()])

def randomDir():
    while True:
        v = random3f()
        if (dot(v,v) < 1):
            return normalize(v)

vertices = []
radii     = []
n         = 5
for ix in range(0,n):
    for iy in range(0,n):
        for iz in range(0,n):
            cx = (ix+.5)/n * 2 - 1
            cy = (iy+.5)/n * 2 - 1
            cz = (iz+.5)/n * 2 - 1
            #center = np.array([cx,cy,cz])
            center = (cx,cy,cz)
            orientation = randomDir()
            ra = randomf() * .2/n
            rb = max(0.,(randomf()-.1) * .2/n)
            pa = center - .4/n * orientation
            pb = center + .4/n * orientation
    
            vertices.append(pa)
            radii.append(ra)
            vertices.append(pb)
            radii.append(rb)
                
np_radii = np.array(radii,dtype=np.float32)
np_vertices = np.array(vertices,dtype=np.float32)

barney = anari.newDevice('default')
mat    = barney.newMaterial('physicallyBased')

cones  = barney.newGeometry('cones')
cones.setParameter('radii',anari.ARRAY,barney.newArray(anari.float,np_radii))
arr = barney.newArray(anari.float3,np_vertices)
cones.setParameter('vertices',anari.ARRAY,arr)
cones.commitParameters()

surface = barney.newSurface()
surface.setParameter('geometry', anari.GEOMETRY, cones)
surface.setParameter('material', anari.MATERIAL, mat)
surface.commitParameters()

world = barney.newWorld();
world.setParameterArray('surface',anari.SURFACE,[surface]);
world.commitParameters()
camera = barney.newCamera('perspective')
camera.setParameter('up',anari.float3,(0,1,0))
camera.setParameter('position',anari.float3,(0,1,0))
camera.setParameter('up',anari.float3,(.5,.8,-3))
camera.setParameter('direction',anari.float3,(-.5,-.8,+3))
camera.commitParameters()

renderer = barney.newRenderer('default')
renderer.setParameter('ambientRadiance',anari.FLOAT32, 1.)
renderer.setParameter('pixelSamples', anari.INT32, 16)
bg_values = np.array(((.9,.9,.9,1.),(.15,.25,.8,1.)), dtype=np.float32).reshape((4,1,2))
bg_gradient = barney.newArray(anari.float4, bg_values)
renderer.setParameter('background', anari.ARRAY, bg_gradient)
renderer.commitParameters()

frame = barney.newFrame()

frame.setParameter('size', anari.uint2, (1024,1024))

frame.setParameter('channel.color', anari.DATA_TYPE, anari.UFIXED8_VEC4)
frame.setParameter('renderer', anari.OBJECT, renderer)
frame.setParameter('camera', anari.OBJECT, camera)
frame.setParameter('world', anari.OBJECT, world)
frame.commitParameters()

frame.render()
fb_color = frame.get('channel.color')

pixels = np.array(fb_color)#.reshape([height, width, 4])

im = PIL.Image.fromarray(pixels)
im = im.transpose(PIL.Image.FLIP_TOP_BOTTOM)
im = im.convert('RGB')
out_file_name = 'cones.png'
print(f'@pynari: done. saving to {out_file_name}')
im.save(out_file_name)

