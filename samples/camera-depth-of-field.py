import matplotlib.pyplot as plt
import numpy as np
#from pynari import *
import pynari as anari
import random
import sys, getopt,PIL
import math

if anari.has_cuda_capable_gpu():
   print('@pynari: detected cuda-capable GPU; using higher res and sample count')
   fb_size = (1600,800)
   num_paths_per_pixel = 32
else:   
   print('@pynari: no CUDA-capable GPU detected, reducing sample count')
   fb_size = (800,400)
   num_paths_per_pixel = 4
look_from = (10., 1.5, 1.)
look_at = (0., 0., 0.)
look_up = (0.,1.,0.)
fovy = 70.
apertureRadius = 2.4
sphereRadius = .25
# focusDistance is computed from look_from to look_at, so look-at should be in
# perfect focus
sphereJitter=.05

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

def length(v):
    return math.sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2])

# create a "Lambertian" (ie, diffuse) anari matterial, using ANARI's
# 'matte' material
def make_lambertian(r,g,b):
    mat = device.newMaterial('matte')
    mat.setParameter('color',anari.float3,(r,g,b))
    mat.commitParameters()
    return mat

def make_dielectric(ior):
    mat = device.newMaterial('physicallyBased')
    mat.setParameter('baseColor',anari.float3,(1.,1.,1.))
    mat.setParameter('transmission',anari.FLOAT32,1.)
    mat.setParameter('specular',anari.FLOAT32,0.)
    mat.setParameter('metallic',anari.FLOAT32,0.)
    mat.setParameter('ior',anari.FLOAT32,ior)
    mat.commitParameters()
    return mat

def make_metal(albedo,fuzz):
    mat = device.newMaterial('physicallyBased')
    mat.setParameter('baseColor',anari.float3,(albedo[0],albedo[1],albedo[2]))
    mat.setParameter('ior',anari.FLOAT32,1.45)
    mat.setParameter('metallic',anari.FLOAT32,1.)
    mat.setParameter('specular',anari.FLOAT32,0.)
    mat.setParameter('roughness',anari.FLOAT32,0.2)
    mat.commitParameters()
    return mat
    
def create_spheres():
    for a in range(-4,5):
        for b in range(-4,5):
            for c in range(-4,5):
                choose_mat = random.random();
                center = (
                    a + sphereJitter*random.random(),
                    b + sphereJitter*random.random(),
                    c + sphereJitter*random.random());
                if choose_mat < 0.8:
                    x = random.random()
                    y = random.random()
                    z = random.random()
                    add_sphere(center,sphereRadius,make_lambertian(x*x,y*y,z*z))
                elif choose_mat < .95:
                    x = random.random()
                    y = random.random()
                    z = random.random()
                    x = .5*(1+x)
                    y = .5*(1+y)
                    z = .5*(1+z)
                    add_sphere(center,sphereRadius,make_metal((x,y,z),.2))
                else:
                    add_sphere(center,sphereRadius,make_dielectric(1.5))


spheres = []

device = anari.newDevice('default')

create_spheres()

world = device.newWorld()
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
camera.setParameter('fovy',anari.FLOAT32,fovy*3.14/180)
camera.setParameter('focusDistance',anari.FLOAT32,length(direction))
camera.setParameter('apertureRadius',anari.FLOAT32,apertureRadius)
camera.commitParameters()



# background gradient: use an image of 1 pixel wide and 2 pixels high
bg_values = np.array(((.9,.9,.9,1.),(.15,.25,.8,1.)), dtype=np.float32).reshape((2,1,4))
bg_gradient = device.newArray2D(anari.float4, bg_values)

renderer = device.newRenderer('default')
renderer.setParameter('ambientRadiance',anari.FLOAT32, .5)
renderer.setParameter('pixelSamples', anari.INT32, num_paths_per_pixel)
renderer.setParameter('background', anari.ARRAY2D, bg_gradient)
renderer.commitParameters()


frame = device.newFrame()

frame.setParameter('size', anari.uint2, fb_size)

frame.setParameter('channel.color', anari.DATA_TYPE, anari.UFIXED8_RGBA_SRGB)
frame.setParameter('renderer', anari.RENDERER, renderer)
frame.setParameter('camera', anari.CAMERA, camera)
frame.setParameter('world', anari.WORLD, world)
frame.commitParameters()

frame.render()
fb_color = frame.get('channel.color')

pixels = np.array(fb_color)#.reshape([height, width, 4])

out_file_name = 'camera-dof.png'
args = sys.argv[1:]
opts, args = getopt.getopt(args,"ho:",["help","output="])
for opt,arg in opts:
    if opt == '-h':
        printf('camera-depth-of-field.py [-o outfile.jpg]')
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




