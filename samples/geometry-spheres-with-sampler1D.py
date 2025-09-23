import pynari as anari
import random
import math
import numpy as np
import matplotlib.pyplot as plt
import sys, getopt
import PIL
import os

executable_path = os.path.abspath(__file__)
executable_directory = os.path.dirname(executable_path)


width  = 1024
height = 1024

cam_at = [0,0,0]
cam_dir=(+.2,-.4,1.)
cam_dist_scale =  1500
cam_pos = [cam_at[0]-cam_dist_scale*cam_dir[0],
           cam_at[1]-cam_dist_scale*cam_dir[1],
           cam_at[2]-cam_dist_scale*cam_dir[2]]
cam_up = [0.0, 1.0, 0.0]

device = anari.newDevice('default')

base_file_name = "/dlaf-1m"
#base_file_name = "/dlaf"

camera = device.newCamera('perspective')
camera.setParameter('aspect', anari.FLOAT32, width/height)
camera.setParameter('position',anari.FLOAT32_VEC3, cam_pos)
camera.setParameter('direction',anari.float3,cam_dir)
camera.setParameter('up',anari.float3,cam_up)
camera.commitParameters()

world = device.newWorld()

cmap_data = np.fromfile(executable_directory+base_file_name+".cmap.binary.float3",dtype=np.float32)
cmap = device.newSampler('image1D')
cmap.setParameterArray1D('image',anari.float3,cmap_data) 
cmap.setParameter('inAttribute','attribute0')
cmap.commitParameters()

points_data = np.fromfile(executable_directory+base_file_name+".points.binary.float3",dtype=np.float32)
scalar_data = np.fromfile(executable_directory+base_file_name+".scalars.binary.float",dtype=np.float32)

print('creating sphere geom')
geom = device.newGeometry('sphere')
geom.setParameterArray1D('vertex.position',anari.float3,points_data)
geom.setParameterArray1D('vertex.attribute0',anari.float,scalar_data)
geom.setParameter('radius',anari.float,4.)
geom.commitParameters()
                 
material = device.newMaterial('matte')
#material.setParameter('color','color')
material.setParameter('color',anari.SAMPLER,cmap)
material.commitParameters()

surf = device.newSurface()
surf.setParameter('material',anari.MATERIAL, material)
surf.setParameter('geometry',anari.GEOMETRY, geom)
surf.commitParameters()


world.setParameterArray1D('surface', anari.SURFACE, [ surf ])

hdri_res = 512

def checkerboard(y,x,d):
    return 3.*((y//16+x//16) % 2)

rad = np.fromfunction(checkerboard, (hdri_res,2*hdri_res,3), dtype=np.float32)
radiance_array=device.newArray2D(anari.float3,rad.reshape((hdri_res,2*hdri_res,3)))
hdri=device.newLight('hdri')
hdri.setParameter('radiance',anari.ARRAY2D,radiance_array)
hdri.setParameter('up',anari.float3,(0,1,0))
hdri.commitParameters()

light = device.newLight('directional')
light.setParameter('direction', anari.float3, (0,0,1))
light.setParameter('irradiance', anari.float, 1)
light.commitParameters()

array = device.newArray1D(anari.LIGHT, [light,hdri])
world.setParameter('light', anari.ARRAY1D, array)

world.commitParameters()


# background gradient: use an image of 1 pixel wide and 2 pixels high
bg_values = np.array(((.9,.9,.9,1.),(.15,.25,.8,1.)), dtype=np.float32).reshape((2,1,4))
bg_gradient = device.newArray2D(anari.float4, bg_values)


renderer = device.newRenderer('default')
if anari.has_cuda_capable_gpu():
    renderer.setParameter('pixelSamples', anari.INT32, 128)
else:
    renderer.setParameter('pixelSamples', anari.INT32, 8)
renderer.setParameter('background', anari.ARRAY2D, bg_gradient)
renderer.setParameter('ambientRadiance',anari.FLOAT32, 1.)
renderer.commitParameters()


frame = device.newFrame()

frame.setParameter('size', anari.uint2, [width, height])

frame.setParameter('channel.color', anari.DATA_TYPE, anari.UFIXED8_RGBA_SRGB)
frame.setParameter('renderer', anari.OBJECT, renderer)
frame.setParameter('camera', anari.OBJECT, camera)
frame.setParameter('world', anari.OBJECT, world)
frame.commitParameters()

frame.render()
fb_color = frame.get('channel.color')

pixels = np.array(fb_color)

out_file_name = 'output.png'
args = sys.argv[1:]
opts, args = getopt.getopt(args,"ho:",["help","output="])
for opt,arg in opts:
    if opt == '-h':
        printf('run.py [-o outfile.jpg]')
        sys.exit(0)
    elif opt == '-o':
        out_file_name = arg

im = PIL.Image.fromarray(pixels)
im = im.transpose(PIL.Image.FLIP_TOP_BOTTOM)
print(f'@pynari: done. saving to {out_file_name}')
im.convert('RGB').save(out_file_name)

