#!/usr/bin/pyton3

import pynari as anari
import random
import math
import numpy as np
import matplotlib.pyplot as plt
import sys, getopt
import PIL

print('@pynari: -------------------------------------------------------')
print('@pynari: running geometry-cylinders sample')
print('@pynari: renders a set of random ANARI "cylinders" geometries  ')
print('@pynari: -------------------------------------------------------')

# stolen from shadertoy example, https://www.shadertoy.com/view/Md3cWj 
cylinder_model = [
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
primitive_radius = []

for cc in cylinder_model:
    idx = len(vertex_position);
    vertex_position.append([cc[0],cc[1],cc[2]])
    primitive_radius.append(cc[3])
    vertex_position.append([cc[4],cc[5],cc[6]])
    primitive_index.append([idx,idx+1])

np_vertex_position  = np.array(vertex_position,dtype=np.float32)
np_primitive_radius = np.array(primitive_radius,dtype=np.float32)
np_primitive_index  = np.array(primitive_index,dtype=np.uint32)

device = anari.newDevice('default')
mat    = device.newMaterial('physicallyBased')

cylinders  = device.newGeometry('cylinder')
cylinders.setParameter('primitive.radius',anari.ARRAY,
                       device.newArray(anari.float,np_primitive_radius))
cylinders.setParameter('primitive.index',anari.ARRAY,
                       device.newArray(anari.uint2,np_primitive_index))
cylinders.setParameter('vertex.position',anari.ARRAY,
                       device.newArray(anari.float3,np_vertex_position))
    
mat.setParameter("baseColor",anari.float3,(.3,.3,.5))
mat.setParameter("metallic",anari.float,.5)
mat.setParameter("roughness",anari.float,.2)
mat.commitParameters();
cylinders.commitParameters()

surface = device.newSurface()
surface.setParameter('geometry', anari.GEOMETRY, cylinders)
surface.setParameter('material', anari.MATERIAL, mat)
surface.commitParameters()

light = device.newLight('directional')
light.setParameter('direction', anari.float3, ( 1., -1., -1. ) )
light.commitParameters()

array = device.newArray(anari.LIGHT, [light])

world = device.newWorld();
world.setParameterArray('surface',anari.SURFACE,[surface]);
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
bg_values = np.array(((.9,.9,.9,1.),(.15,.25,.8,1.)), dtype=np.float32).reshape((4,1,2))
bg_gradient = device.newArray(anari.float4, bg_values)
renderer.setParameter('background', anari.ARRAY, bg_gradient)
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
out_file_name = 'cylinders.png'
print(f'@pynari: done. saving to {out_file_name}')
im.save(out_file_name)

