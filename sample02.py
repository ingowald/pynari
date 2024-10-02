import matplotlib.pyplot as plt
import numpy as np
#from pynari import *
import pynari as anari
import random
import sys, getopt,PIL

fb_size = (1600,800)
look_from = (13., 2., 3.)
look_at = (0., 0., 0.)
look_up = (0.,1.,0.)
fovy = 20.

def add_sphere(pos, radius, material):
    geom = device.newGeometry('sphere')
    array = device.newArray(anari.FLOAT32_VEC3,np.array(pos,dtype=np.float32))
    geom.setParameter('vertex.position',anari.ARRAY,array)
    geom.setParameter('radius',anari.FLOAT32,radius)
    geom.commitParameters()

    surf = device.newSurface()
    surf.setParameter('geometry', anari.GEOMETRY, geom)
    surf.setParameter('material', anari.MATERIAL, material)
    surf.commitParameters()
    
    spheres.append(surf)

    
def make_lambertian(r,g,b):
    mat = device.newMaterial('matte')
    mat.setParameter('color',anari.float3,(r,g,b))
    mat.commitParameters()
    return mat

def make_dielectric(ior):
    mat = device.newMaterial('physicallyBased')
    mat.setParameter('baseColor',anari.float3,(1.,1.,1.))
    mat.setParameter('transmission',anari.float,1.)
    mat.setParameter('specular',anari.float,0.)
    mat.setParameter('metallic',anari.float,0.)
    mat.setParameter('ior',anari.float,ior)
    mat.commitParameters()
    return mat


def make_metal(albedo,fuzz):
    mat = device.newMaterial('physicallyBased')
    mat.setParameter('baseColor',anari.float3,(albedo[0],albedo[1],albedo[2]))
    mat.setParameter('ior',anari.FLOAT32,1.45)
    mat.setParameter('metallic',anari.FLOAT32,1.)
    mat.setParameter('specular',anari.FLOAT32,0.)
    mat.setParameter('roughness',anari.float,0.)
    mat.commitParameters()
    return mat
    
def create_spheres():
    add_sphere((0., 1., 0.), 1., make_dielectric(1.5))
    add_sphere((0.,-1000.,-1),1000.,make_lambertian(.5,.5,.5))
    add_sphere((-4.,1.,0.),1.,make_lambertian(.4,.2,.1))
    add_sphere((4.,1.,0.),1.,make_metal((.7,.6,.5),0.))
    for a in range(-11,12):
        for b in range(-11,12):
            choose_mat = random.random();
            center = (a + random.random(), 0.2, b + random.random());
            if choose_mat < 0.8:
                x = random.random()
                y = random.random()
                z = random.random()
                add_sphere(center,.2,make_lambertian(x*x,y*y,z*z))
            elif choose_mat < .95:
                x = random.random()
                y = random.random()
                z = random.random()
                x = .5*(1+x)
                y = .5*(1+y)
                z = .5*(1+z)
                add_sphere(center,.2,make_metal((x,y,z),.2))
            else:
                add_sphere(center,.2,make_dielectric(1.5))


spheres = []

device = anari.newDevice('default')

create_spheres()

world = device.newWorld()
world.setParameterArray('surface', anari.SURFACE, spheres )
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



renderer = device.newRenderer('default')
renderer.setParameter('ambientRadiance',anari.FLOAT32, 10.)
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
    plt.show()
else:
    im = PIL.Image.fromarray(pixels)
    im = im.transpose(PIL.Image.FLIP_TOP_BOTTOM)
    im = im.convert('RGB')
    im.save(out_file_name)




