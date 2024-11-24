# sample that tests 'curves' geometry by generating random 3D bezier
# curves

import matplotlib.pyplot as plt
import numpy as np
#from pynari import *
import pynari as anari
import random
import sys, getopt,PIL
import math

fb_size = (1024,1024)
look_from = (-3,1.5,-2)
look_at = (.5,.5,.5)
look_up = (0.,1.,0.)
fovy = 20.

random.seed(80577)

vertex_position = []
vertex_color = []
vertex_radius = []
primitive_index = []

def add_vertex(v,c):
    global vertex_position
    global vertex_radius
    global vertex_color
    position = (v[0],v[1],v[2])
    radius   = v[3]
    color    = (c[0],c[1],c[2],0.)
    vertex_position.append(position)
    vertex_radius.append(radius)
    vertex_color.append(color)

def lerp_cp(t,p0,p1):
    x0 = p0[0]
    y0 = p0[1]
    z0 = p0[2]
    r0 = p0[3]
    x1 = p1[0]
    y1 = p1[1]
    z1 = p1[2]
    r1 = p1[3]
    r = (1-t)*r0+t*r1
    x = (1-t)*x0+t*x1
    y = (1-t)*y0+t*y1
    z = (1-t)*z0+t*z1
    return (x,y,z,r)

def eval_linear(t,c0,c1):
    r = (1-t)*c0[0]+t*c1[0]
    g = (1-t)*c0[1]+t*c1[1]
    b = (1-t)*c0[2]+t*c1[2]
    a = (1-t)*c0[3]+t*c1[3]
    return (r,g,b,a)

def eval_cubic(t,p00,p01,p02,p03):
    p10 = lerp_cp(t,p00,p01)
    p11 = lerp_cp(t,p01,p02)
    p12 = lerp_cp(t,p02,p03)

    p20 = lerp_cp(t,p10,p11)
    p21 = lerp_cp(t,p11,p12)

    p30 = lerp_cp(t,p20,p21)
    
    return p30
    
# a curve with
# four 4D control points (with x,y,z, and radius each)
# two colors (for start and end)
def add_curve(cp0, cp1, cp2, cp3, color0, color1):
    global vertex_position
    global primitive_index
    n_segs = 16
    for i in range(n_segs):
        t0 = i/n_segs
        t1 = (i+1)/n_segs
        p0 = eval_cubic(t0,cp0,cp1,cp2,cp3)
        c0 = eval_linear(t0,color0,color1)
        p1 = eval_cubic(t1,cp0,cp1,cp2,cp3)
        c1 = eval_linear(t1,color0,color1)
        primitive_index.append(len(vertex_position))
        add_vertex(p0,c0)
        add_vertex(p1,c1)
        
# create a "Lambertian" (ie, diffuse) anari matterial, using ANARI's
# 'matte' material
def make_color_mapped_lambertian(r,g,b):
    mat = device.newMaterial('matte')
    #mat.setParameter('color',anari.float3,(r,g,b))
    mat.setParameter('color',anari.STRING,'color')
    mat.commitParameters()
    return mat

def make_color_mapped_metal(albedo,fuzz):
    mat = device.newMaterial('physicallyBased')
    mat.setParameter('baseColor',anari.STRING,'color')
    mat.setParameter('ior',anari.FLOAT32,1.45)
    mat.setParameter('metallic',anari.FLOAT32,1.)
    mat.setParameter('specular',anari.FLOAT32,0.)
    mat.setParameter('roughness',anari.FLOAT32,.5)
    mat.commitParameters()
    return mat
    
def make_anari_surface(material):
    geom = device.newGeometry('curve')

    np_position = np.array(vertex_position,dtype=np.float32).flatten()
    position = device.newArray(anari.FLOAT32_VEC3,np_position)
    geom.setParameter('vertex.position',anari.ARRAY,position)
    
    np_radius = np.array(vertex_radius,dtype=np.float32).flatten()
    radius = device.newArray(anari.FLOAT32,np_radius)
    geom.setParameter('vertex.radius',anari.ARRAY,radius)

    np_color = np.array(vertex_color,dtype=np.float32).flatten()
    color = device.newArray(anari.FLOAT32_VEC4,np_color)
    geom.setParameter('vertex.color',anari.ARRAY,color)

    np_index = np.array(primitive_index,dtype=np.uint32).flatten()
    index = device.newArray(anari.UINT32,np_index)
    geom.setParameter('primitive.index',anari.ARRAY,index)

    geom.commitParameters()

    surf = device.newSurface()
    surf.setParameter('geometry', anari.GEOMETRY, geom)
    surf.setParameter('material', anari.MATERIAL, material)
    surf.commitParameters()
    
    return surf

def make_cp(v,r):
    return np.array((v[0],v[1],v[2],r),dtype=np.float32)

def random_sphere(r):
    while True:
        x = 1-2*random.random()
        y = 1-2*random.random()
        z = 1-2*random.random()
        r2 = x*x+y*y+z*z
        if r2 <= 1:
            r = r / math.sqrt(r2)
            return np.array((x*r,y*r,z*r),dtype=np.float32)
    
def create_curves():
    global vertex_position
    global vertex_radius
    global vertex_color
    global primitive_index
    surfaces = []
    for a in range(100):
        vertex_position = []
        vertex_radius = []
        vertex_color = []
        primitive_index = []
        choose_mat = random.random()
        r = random.random()
        if (choose_mat < .5):
            material = make_color_mapped_metal((1,1,1),.2+.5*r*r)
        else:
            material = make_color_mapped_lambertian(1,1,1)
        for b in range(4):
            c0 = np.array((random.random(),random.random(),random.random(),1),dtype=np.float32)
            c1 = np.array((random.random(),random.random(),random.random(),1),dtype=np.float32)
            d0 = random_sphere(.1)
            d1 = random_sphere(.1)
            d2 = random_sphere(.1)
            p0 = np.array((random.random(),random.random(),random.random()),dtype=np.float32)
            p1 = p0 + d0
            p2 = p1 + d0+d1
            p3 = p2 + d0+d1+d2
            r0 = .005 + .02*random.random()
            r1 = .005 + .02*random.random()
            r2 = .005 + .02*random.random()
            r3 = .005 + .02*random.random()
            add_curve(make_cp(p0,r0),
                      make_cp(p1,r1),
                      make_cp(p2,r2),
                      make_cp(p3,r3),c0,c1)
        surfaces.append(make_anari_surface(material))
    return surfaces




device = anari.newDevice('default')

curves = create_curves()

world = device.newWorld()
world.setParameterArray('surface', anari.SURFACE, curves )

light = device.newLight('directional')
light.setParameter('direction', anari.float3, (1,-1,1))
light.commitParameters()

array = device.newArray(anari.LIGHT, [light])
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
camera.setParameter('fovy',anari.float,fovy*3.14/180)
camera.commitParameters()


# background gradient: use an image of 1 pixel wide and 2 pixels high
bg_values = np.array(((.9,.9,.9,1.),(.15,.25,.8,1.)), dtype=np.float32).reshape((4,1,2))
bg_gradient = device.newArray(anari.float4, bg_values)




renderer = device.newRenderer('default')
renderer.setParameter('ambientRadiance',anari.FLOAT32, .2)
renderer.setParameter('background', anari.ARRAY, bg_gradient)
renderer.setParameter('pixelSamples', anari.INT32, 128)
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

pixels = np.array(fb_color)

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




