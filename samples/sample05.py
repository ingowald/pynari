import matplotlib.pyplot as plt
import numpy as np
#from pynari import *
import pynari as anari
import random
import sys, getopt,PIL

fb_size = (2048, 2048)
look_from = (.3, .6, -4.)
look_at = (0., 0., 0.)
look_up = (0.,1.,0.)
fovy = 35.

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

def make_pbr(roughness, metallic):
    mat = device.newMaterial('physicallyBased')
    mat.setParameter('baseColor',anari.float3,(.2,.8,.2))
    mat.setParameter('ior',anari.FLOAT32,1.45)
    mat.setParameter('metallic',anari.FLOAT32,metallic)
    mat.setParameter('specular',anari.FLOAT32,0.)
    mat.setParameter('roughness',anari.float,roughness)
    mat.commitParameters()
    return mat
    
def create_spheres():
    grid_size = 9
    radius = .9 / grid_size
    for a in range(grid_size):
        for b in range(grid_size):
            choose_mat = random.random();
            center = ((a + .5 - grid_size/2) / (grid_size/2),
                      (b + .5 - grid_size/2) / (grid_size/2),
                      0.)
            metallic = (a+.5)/grid_size
            roughness = (b+.5)/grid_size
            add_sphere(center,radius,make_pbr(roughness*roughness,metallic))


spheres = []

device = anari.newDevice('default')

create_spheres()

world = device.newWorld()
light = device.newLight('directional')
light.setParameter('direction', anari.float3, (-.6,-1,+.5))
light.setParameter('color', anari.float3, (1,1,1))
light.setParameter('irradiance', anari.float, 1)
light.commitParameters()

array = device.newArray1D(anari.LIGHT, [light])
world.setParameter('light', anari.ARRAY1D, array)


world.setParameterArray1D('surface', anari.SURFACE, spheres )
world.commitParameters()


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
bg_values = np.array(((.9,.9,.9,1.),(.15,.25,.8,1.)), dtype=np.float32).reshape((2,1,4))
bg_gradient = device.newArray1D(anari.float4, bg_values)


renderer = device.newRenderer('default')
renderer.setParameter('ambientRadiance',anari.FLOAT32, .2)
renderer.setParameter('background', anari.ARRAY2D, bg_gradient)
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




