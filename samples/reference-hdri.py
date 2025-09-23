# test scene for checking proper orientation for hdri maps

import matplotlib.pyplot as plt
import numpy as np
#from pynari import *
import pynari as anari
import random
import sys, getopt,PIL
import math

fb_size = (1024, 1024)
num_paths_per_pixel = 128
look_from = (.01, .01, -5.)
#look_from = (.01, -5, .01)
look_at = (0., 0., 0.)
look_up = (0.,1.,0.)
fovy = 60.

random.seed(80577)

def add_sphere(pos, radius, material):
    geom = device.newGeometry('sphere')
    array = device.newArray1D(anari.FLOAT32_VEC3,np.array(pos,dtype=np.float32))
    geom.setParameter('vertex.position',anari.ARRAY1D,array)
    geom.setParameter('radius',anari.FLOAT32,radius)
    geom.commitParameters()

    surf = device.newSurface()
    surf.setParameter('geometry', anari.GEOMETRY, geom)
    surf.setParameter('material', anari.MATERIAL, material)
    surf.commitParameters()
    spheres.append(surf)

def make_metal(albedo,fuzz):
    mat = device.newMaterial('physicallyBased')
    mat.setParameter('baseColor',anari.float3,(albedo[0],albedo[1],albedo[2]))
    mat.setParameter('ior',anari.FLOAT32,1.45)
    mat.setParameter('metallic',anari.FLOAT32,1.)
    mat.setParameter('specular',anari.FLOAT32,0.)
    mat.setParameter('roughness',anari.FLOAT32,0.02)
    mat.commitParameters()
    return mat
    
def create_spheres():
    add_sphere((0.,0.,0.),1.,make_metal((1.,1.,1.),0))

spheres = []

device = anari.newDevice('default')

create_spheres()


envmap_texels = []
nx = 512
ny = 256
for y in range(0,ny):
    for x in range(0,nx):
        if (x % 16) == 1 and (y % 16) == 1:
            envmap_texels.append(1)
            envmap_texels.append(1)
            envmap_texels.append(1)
        else:
            fx = (x+.5)/nx
            fy = (y+.5)/ny
            envmap_texels.append(fx)
            envmap_texels.append(0)
            envmap_texels.append(fy)

envmap_np = np.array(envmap_texels, dtype=np.float32).reshape((ny,nx,3))
envmap_array = device.newArray2D(anari.float3, envmap_np)
light = device.newLight("hdri")
light.setParameter('radiance',anari.ARRAY2D,envmap_array)
light.setParameter("up",anari.float3,(0.,1.,0.))
light.setParameter("direction",anari.float3,(0.,0.,1.))
light.commitParameters()



world = device.newWorld()
world.setParameterArray1D('surface', anari.SURFACE, spheres )
array = device.newArray1D(anari.LIGHT, [light])
world.setParameter('light', anari.ARRAY1D, array)

world.commitParameters()

camera = device.newCamera('perspective')
camera.setParameter('aspect', anari.FLOAT32, fb_size[0]/fb_size[1])
camera.setParameter('position',anari.FLOAT32_VEC3, look_from)
direction = [ look_at[0] - look_from[0],
              look_at[1] - look_from[1],
              look_at[2] - look_from[2] ] 
camera.setParameter('direction',anari.float3, direction)
camera.setParameter('up',anari.float3,look_up)
camera.setParameter('fovy',anari.FLOAT32,fovy*3.14/180)
camera.commitParameters()



renderer = device.newRenderer('default')
renderer.setParameter('ambientRadiance',anari.FLOAT32, 1.)
renderer.setParameter('pixelSamples', anari.INT32, num_paths_per_pixel)
renderer.commitParameters()

frame = device.newFrame()

frame.setParameter('size', anari.uint2, fb_size)

frame.setParameter('channel.color', anari.DATA_TYPE, anari.UFIXED8_VEC4)
frame.setParameter('renderer', anari.OBJECT, renderer)
frame.setParameter('camera', anari.OBJECT, camera)
frame.setParameter('world', anari.OBJECT, world)
frame.commitParameters()

frame.render()
fb_color = frame.get('channel.color')

pixels = np.array(fb_color)#.reshape([height, width, 4])

out_file_name = ''
args = sys.argv[1:]
opts, args = getopt.getopt(args,"ho:",["help","output="])
for opt,arg in opts:
    if opt == '-h':
        printf('sample02.py [-o outfile.jpg]')
        sys.exit(0)
    elif opt == '-o':
        out_file_name = arg

if out_file_name == '':
    plt.imshow(pixels)
    plt.gca().invert_yaxis()
    plt.show()
else:
    im = PIL.Image.fromarray(pixels)
    im = im.transpose(PIL.Image.FLIP_TOP_BOTTOM)
    im = im.convert('RGB')
    im.save(out_file_name)




