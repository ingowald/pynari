import matplotlib.pyplot as plt
import numpy as np
#from pynari import *
import pynari as anari
import random
import sys, getopt,PIL

print('@pynari: -------------------------------------------------------')
print('@pynari: running sampler-image3d - two triangles')
print('@pynari: within a "image3d" sampler object')
print('@pynari: -------------------------------------------------------')

device = anari.newDevice('default')

out_file_name = ''
args = sys.argv[1:]
opts, args = getopt.getopt(args,"hmo:",["magnetic","help","output="])
for opt,arg in opts:
    if opt == '-h':
        printf('sampler-image3d.py [-o outfile.jpg]')
        sys.exit(0)
    elif opt == '-o':
        out_file_name = arg

N = 16
array = np.zeros((N,N,N,3),dtype=np.float32)
for z in range(0,N):
    for y in range(0,N):
        for x in range(0,N):
            r = 1- x / N
            g = 1- y / N
            b = 1- z / N
            array[x,y,z,0] = r
            array[x,y,z,1] = g
            array[x,y,z,2] = b

array_data = device.newArray3D(anari.FLOAT32_VEC3,array)
sampler = device.newSampler('image3D')
sampler.setParameter('image',anari.ARRAY3D,array_data)
sampler.setParameter('inAttribute','worldPosition')
#sampler.setParameter('inAttribute','attribute0')
sampler.commitParameters()

vertex = np.array([-.3,-.3, .5,
                   -.3,1.3,-.3,
                   1.3,-.3,1.3,
                   1.3,1.3,1.3]
                  ,dtype=np.float32)
index = np.array([0,1,2,
         1,3,2],dtype=np.uint32)

world = device.newWorld()

mesh = device.newGeometry('triangle')

array = device.newArray1D(anari.FLOAT32_VEC3,vertex)
mesh.setParameter('vertex.position', anari.ARRAY1D, array)

array = device.newArray1D(anari.UINT32_VEC3 , index)
mesh.setParameter('primitive.index', anari.ARRAY1D, array)
mesh.commitParameters()

material = device.newMaterial('matte')
material.setParameter('color',anari.SAMPLER,sampler)
material.commitParameters()

surface = device.newSurface()
surface.setParameter('geometry', anari.GEOMETRY, mesh)
surface.setParameter('material', anari.MATERIAL, material)
surface.commitParameters()

world.setParameterArray1D('surface', anari.SURFACE, [ surface ])

light = device.newLight('directional')
light.setParameter('direction', anari.float3, ( 1., -1., 1. ) )
light.commitParameters()

array = device.newArray1D(anari.LIGHT, [light])
world.setParameter('light', anari.ARRAY1D, array)
world.commitParameters()


fb_size = (800,800)
look_from = (.5,.5,-2)
look_at = (.5,.5,.5)
look_up = (0.,1.,0.)
fovy = 40.

camera = device.newCamera('perspective')
camera.setParameter('aspect', anari.FLOAT32, fb_size[0]/fb_size[1])
camera.setParameter('position',anari.FLOAT32_VEC3, look_from)

direction = [ look_at[0] - look_from[0],
              look_at[1] - look_from[1],
              look_at[2] - look_from[2] ] 
camera.setParameter('direction',anari.float3, direction)
camera.setParameter('up',anari.float3,look_up)
camera.setParameter('fovy',anari.float,fovy*3.14/180)
camera.commitParameters()


# background gradient: use an image of 1 pixel wide and 2 pixels high
bg_values = np.array(((.9,.9,.9,1.),(.15,.25,.8,1.)),
                     dtype=np.float32).reshape((2,1,4))
bg_gradient = device.newArray2D(anari.float4, bg_values)

renderer = device.newRenderer('default')
renderer.setParameter('ambientRadiance',anari.FLOAT32, 1.)
renderer.setParameter('background', anari.ARRAY2D, bg_gradient)
if anari.has_cuda_capable_gpu():
    # actually we have denoising on the gpu, so probably need way less...
    renderer.setParameter('pixelSamples', anari.INT32, 1024)
else:
    renderer.setParameter('pixelSamples', anari.INT32, 16)
renderer.commitParameters()


frame = device.newFrame()

frame.setParameter('size', anari.uint2, fb_size)

frame.setParameter('channel.color', anari.DATA_TYPE, anari.UFIXED8_RGBA_SRGB)
frame.setParameter('renderer', anari.OBJECT, renderer)
frame.setParameter('camera', anari.OBJECT, camera)
frame.setParameter('world', anari.OBJECT, world)
frame.commitParameters()

frame.render()
fb_color = frame.get('channel.color')
pixels = np.array(fb_color)

if out_file_name == '':
    plt.imshow(pixels)
    plt.gca().invert_yaxis()
    plt.show()
else:
    im = PIL.Image.fromarray(pixels)
    im = im.transpose(PIL.Image.FLIP_TOP_BOTTOM)
    im = im.convert('RGB')
    print(f'@pynari: done. saving to {out_file_name}')
    im.save(out_file_name)





