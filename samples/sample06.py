# sample06 - this sample demonstrates exists to show/test MPI-parallel
# ANARI rendering. For this sample to make sense you need to run it
# with MPI, and with a "Data Parallel ANARI" capable ANARI device such
# as PTC or barney. You can still run it without MPI, but should
# simply see a NxNxN grid of same-color cubes; when run with multiple
# MPI ranks (and a mpi-capable ANARI) you should see the cubes having
# different colors, with the color of each cube indicating which rank
# generated that data.

import matplotlib.pyplot as plt
import numpy as np
import os
#from pynari import *
import pynari as anari
import random
import sys, getopt,PIL
from mpi4py import MPI

fb_size = (1024,1024)
look_from = (-.7,2.,-4.)
look_at = (0., 0., 0.)
look_up = (0.,1.,0.)
fovy = 40.
grid_size = 7
use_floatFB = False#True

out_file_name = ''
args = sys.argv[1:]
opts, args = getopt.getopt(args,"ho:f",["help","output=","float4"])
for opt,arg in opts:
    if opt == '-h':
        print('mpirun -n <numRanks> sample06.py [-o outfile.jpg]')
        print('note: make sure to use a ANARI_LIBRARY that supports data parallel rendering')
        sys.exit(0)
    elif opt == '-f':
        use_floatFB = True
    elif opt == '-o':
        out_file_name = arg

random.seed(80577)
surfaces = []


def make_material(color_idx):
    random.seed(0x12345+color_idx)
    red   = random.uniform(.05,.95)
    green = random.uniform(.05,.95)
    blue  = random.uniform(.05,.95)
    red = .2+.8*red*red
    green = .2+.8*green*green
    blue = .2+.8*blue*blue
    mat = device.newMaterial('physicallyBased')
    mat.setParameter('baseColor',anari.float3,(red,green,blue))
    mat.setParameter('ior',anari.FLOAT32,1.45)
    mat.setParameter('metallic',anari.FLOAT32,0.5)
    mat.setParameter('specular',anari.FLOAT32,0.)
    mat.setParameter('roughness',anari.FLOAT32,0.3)
    mat.commitParameters()
    return mat
    
def add_cube(mpi_rank,mpi_size,ix,iy,iz):
    cube_idx = ix + grid_size*iy + grid_size*grid_size*iz
    random.seed(290374+cube_idx)
    rank_of_cube = random.randrange(mpi_size)
    if mpi_rank != rank_of_cube:
        return

    geom = device.newGeometry('triangle')
    vertices = []
    gap = .1
    for iiz in range(2):
        for iiy in range(2):
            for iix in range(2):
                # generate vertex in [0,N]^3 space
                x = ix + gap + iix*(1-2*gap)
                y = iy + gap + iiy*(1-2*gap)
                z = iz + gap + iiz*(1-2*gap)
                # convert to [-1,+1]^3 space
                x = -1 + 2*(x/grid_size)
                y = -1 + 2*(y/grid_size)
                z = -1 + 2*(z/grid_size)
                # store vertex
                vertices.append((x,y,z))
     
    _vertices = device.newArray(anari.FLOAT32_VEC3,np.array(vertices,dtype=np.float32).flatten())
    indices = [
        ( 0,1,3 ), ( 0,3,2 ),
        ( 5,7,6 ), ( 5,6,4 ),
        ( 0,4,5 ), ( 0,5,1 ),
        ( 2,3,7 ), ( 2,7,6 ),
        ( 1,5,7 ), ( 1,7,3 ),
        ( 4,0,2 ), ( 4,2,6 )
    ]
    _indices = device.newArray(anari.UINT32_VEC3,
                               np.array(indices,dtype=np.uint32).flatten())

    geom.setParameter('vertex.position',anari.ARRAY,_vertices)
    geom.setParameter('primitive.index',anari.ARRAY,_indices)
    geom.commitParameters()

    material = make_material(mpi_rank)
    surf = device.newSurface()
    surf.setParameter('geometry', anari.GEOMETRY, geom)
    surf.setParameter('material', anari.MATERIAL, material)
    surf.commitParameters()
    
    surfaces.append(surf)

def create_surfaces(mpi_rank,mpi_size):
    for iz in range(grid_size):
        for iy in range(grid_size):
            for ix in range(grid_size):
                add_cube(mpi_rank,mpi_size,ix,iy,iz)

anariDeviceToUse = os.getenv('ANARI_LIBRARY')
if anariDeviceToUse == None:
    anariDeviceToUse = 'barney_mpi'
device = anari.newDevice(anariDeviceToUse,'default')

name = MPI.COMM_WORLD.Get_name()
addr = MPI._addressof(MPI.COMM_WORLD)
device.setParameter('pointer_to_mpi_communicator', anari.UINT64, addr);
device.commitParameters()
mpi_rank, mpi_size = (MPI.COMM_WORLD.Get_rank(), MPI.COMM_WORLD.Get_size())
create_surfaces(mpi_rank,mpi_size)

light = device.newLight('directional')
#look_from = (-.7,2.,-4.)
light.setParameter('direction', anari.float3, ( -1., -1., 1. ) )
light.setParameter('radiance', anari.float, 4.)
light.commitParameters()
light_array = device.newArray(anari.LIGHT, [light])
    

world = device.newWorld()
if True:
    world.setParameterArray('surface', anari.SURFACE, surfaces )
    world.setParameterArray('light', anari.LIGHT, [ light ])
else:
    # this _should_ work,but currently ignores the lights
    rootGroup = device.newGroup(surfaces)
    rootGroup.commitParameters()
    inst = device.newInstance('transform')
    inst.setParameter('group',anari.OBJECT,rootGroup)
    inst.setParameterArray('light', anari.LIGHT, [ light ])
    inst.setParameter('transform',anari.FLOAT32_MAT3X4,
                      [
                       1,0,0,
                       0,1,0,
                       0,0,1,
                       0,0,0
                      ]
                      )
    inst.commitParameters()
    world.setParameterArray('instance', anari.OBJECT, [ inst ])

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



# background gradient: use an image of 1 pixel wide and 2 pixels high
bg_values = np.array(((.9,.9,.9,1.),(.15,.25,.8,1.)), dtype=np.float32).reshape((2,1,4))
bg_gradient = device.newArray(anari.float4, bg_values)

renderer = device.newRenderer('default')
renderer.setParameter('ambientRadiance',anari.FLOAT32, .2)
renderer.setParameter('background', anari.ARRAY, bg_gradient)
renderer.setParameter('pixelSamples', anari.INT32, 16)
renderer.commitParameters()


frame = device.newFrame()

frame.setParameter('size', anari.uint2, fb_size)

if use_floatFB:
    frame.setParameter('channel.color', anari.DATA_TYPE, anari.FLOAT32_VEC4)
else:
    frame.setParameter('channel.color', anari.DATA_TYPE, anari.UFIXED8_RGBA_SRGB)
frame.setParameter('renderer', anari.OBJECT, renderer)
frame.setParameter('camera', anari.OBJECT, camera)
frame.setParameter('world', anari.OBJECT, world)

frame.commitParameters()

frame.render()
fb_color = frame.get('channel.color')

pixels = np.array(fb_color)

if MPI.COMM_WORLD.Get_rank() == 0:
    if use_floatFB:
        width = fb_size[0]
        height = fb_size[1]
        pixels = pixels.reshape(width*height*4)
        r = pixels[0::4]
        g = pixels[1::4]
        b = pixels[2::4]
        pixels = np.empty(width*height*3,dtype=np.float32)
        pixels[0::3] = r
        pixels[1::3] = g
        pixels[2::3] = b
        pixels = pixels.reshape((width,height,3))
    if out_file_name == '':
        plt.imshow(pixels)
        plt.gca().invert_yaxis()
        plt.show()
    else:
        im = PIL.Image.fromarray(pixels)
        im = im.transpose(PIL.Image.FLIP_TOP_BOTTOM)
        im = im.convert('RGB')
        im.save(out_file_name)
MPI.Finalize()




