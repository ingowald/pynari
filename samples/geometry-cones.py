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
print('@pynari: renders a set of random ANARI "cone" geometries  ')
print('@pynari: -------------------------------------------------------')

use_vertex_color = False
#use_vertex_color = True

# stolen from shadertoy example, https://www.shadertoy.com/view/Md3cWj 
cone_model = [
        [      0,     -1.3,     0,    10,      0,          -2,     0 ],
        [      0,       -1,     0,   3.5,      0,          -2,     0 ],
        [ -.7071,-1.+.7071,     0,     1,  .7071,-1.+3.*.7071,     0 ],
        [      0,       -1,     3,    .2,      0,           4,     3 ],
         
        [   2.12,       -1,  2.12,   .2,   2.12,           4,  2.12 ],
        [      3,       -1,     0,   .2,      3,           4,     0 ],
        [   2.12,       -1, -2.12,   .2,   2.12,           4, -2.12 ],
        [      0,       -1,    -3,   .2,      0,           4,    -3 ],
         
        [ -2.12,     -1.,-2.12,   .2, -2.12,         4.,-2.12 ],
        [   -3.,     -1.,   0.,   .2,   -3.,         4.,   0. ],
        [ -2.12,     -1., 2.12,   .2, -2.12,         4., 2.12 ],
        [      0,        4,     0,   3.5,      0,           5,     0 ]
]

vertex_position = []
primitive_index = []
vertex_radius = []
vertex_color = []
primitive_color = []

def average(a, b):
    return [(a[0]+b[0])/2.,
            (a[1]+b[1])/2.,
            (a[2]+b[2])/2.]

def randomColor(i):
    i = i + 0x234567
    r = i ^ 0x1234339
    r = r * 11 ^ 0x1234339
    r = r * 17 ^ i
    g = i ^ 0x2773477
    g = g * 3 ^ 0x2773477
    g = g * 5 ^ 0x2773477
    g = g * 7 ^ i
    b = i ^ 0x223766
    b = b * 13 ^ 0x223766
    b = b * 19 ^ i
    return ((r&255)/255.,
            (g&255)/255.,
            (b&255)/255.)

for cc in cone_model:
    idx = len(vertex_position);
    a = [cc[0],cc[1],cc[2]]
    b = [cc[4],cc[5],cc[6]]
    r = cc[3]
    ctr = average(a,b)
    col_a = randomColor(idx+0)
    col_b = randomColor(idx+2)
    col_ctr = average(col_a,col_b)
    # ----------- first half -----------
    # --- first vertex ---
    vertex_position.append(a)
    vertex_radius.append(r)
    vertex_color.append(col_a)
    # --- second vertex - in center ---
    vertex_position.append(ctr)
    vertex_radius.append(r*.8)
    vertex_color.append(col_ctr)
    # the prim
    primitive_color.append(col_a)
    primitive_index.append([idx,idx+1])
    # ----------- second half -----------
    # --- first vertex ---
    vertex_position.append(b)
    vertex_radius.append(r)
    vertex_color.append(col_b)
    # --- second vertex - in center ---
    vertex_position.append(ctr)
    vertex_radius.append(r*.8)
    vertex_color.append(col_ctr)
    # the prim
    primitive_color.append(col_b)
    primitive_index.append([idx+2,idx+3])

np_vertex_position  = np.array(vertex_position,dtype=np.float32)
np_vertex_radius    = np.array(vertex_radius,dtype=np.float32)
np_primitive_index  = np.array(primitive_index,dtype=np.uint32)
np_primitive_color  = np.array(primitive_color,dtype=np.float32)
np_vertex_color     = np.array(vertex_color,dtype=np.float32)

device = anari.newDevice('default')
mat    = device.newMaterial('physicallyBased')

cones  = device.newGeometry('cone')
cones.setParameter('vertex.radius',anari.ARRAY1D,
                       device.newArray1D(anari.float,np_vertex_radius))
cones.setParameter('primitive.index',anari.ARRAY1D,
                       device.newArray1D(anari.uint2,np_primitive_index))
cones.setParameter('vertex.position',anari.ARRAY1D,
                       device.newArray1D(anari.float3,np_vertex_position))
if use_vertex_color:
    cones.setParameter('vertex.color',anari.ARRAY1D,
                        device.newArray1D(anari.float3,np_vertex_color))
else:
    cones.setParameter('primitive.color',anari.ARRAY1D,
                        device.newArray1D(anari.float3,np_primitive_color))
    
mat.setParameter("baseColor",anari.float3,(.3,.3,.5))
mat.setParameter("metallic",anari.float,.5)
mat.setParameter("roughness",anari.float,.2)
mat.commitParameters();
cones.commitParameters()

surface = device.newSurface()
surface.setParameter('geometry', anari.GEOMETRY, cones)
surface.setParameter('material', anari.MATERIAL, mat)
surface.commitParameters()

light = device.newLight('directional')
light.setParameter('direction', anari.float3, ( 1., -1., -1. ) )
light.commitParameters()

array = device.newArray1D(anari.LIGHT, [light])

world = device.newWorld();
world.setParameterArray1D('surface',anari.SURFACE,[surface]);
world.setParameter('light', anari.ARRAY1D, array)
world.commitParameters()
camera = device.newCamera('perspective')
camera.setParameter('up',anari.float3,(0,1,0))
camera.setParameter('position',anari.float3,(5,8,-30))
camera.setParameter('direction',anari.float3,(-5,-8,+30))
camera.setParameter('fovy',anari.float,35*math.pi/180)
camera.commitParameters()

renderer = device.newRenderer('default')
renderer.setParameter('ambientRadiance',anari.FLOAT32, 1.)
renderer.setParameter('pixelSamples', anari.INT32, 16)
bg_values = np.array(((.9,.9,.9,1.),(.15,.25,.8,1.)), dtype=np.float32).reshape((2,1,4))
bg_gradient = device.newArray2D(anari.float4, bg_values)
renderer.setParameter('background', anari.ARRAY2D, bg_gradient)
renderer.commitParameters()

frame = device.newFrame()

frame.setParameter('size', anari.uint2, (1024,1024))

frame.setParameter('channel.color', anari.DATA_TYPE, anari.UFIXED8_RGBA_SRGB)
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

out_file_name = 'testOrb.png'
args = sys.argv[1:]
opts, args = getopt.getopt(args,"o:",["output="])
for opt,arg in opts:
    if  opt == '-o':
        out_file_name = arg
print(f'@pynari: done. saving to {out_file_name}')
im.save(out_file_name)

