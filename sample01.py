import matplotlib.pyplot as plt
import numpy as np
import sys, getopt
import PIL

#from pynari import *
import pynari as anari

width = 1024
height = 768

cam_pos = [0.0, 0.0, 0.0]
cam_up = [0.0, 1.0, 0.0]
cam_view = [0.1, 0.0, 1.0]

vertex = np.array([
  -1.0, -1.0, 3.0,
  -1.0,  1.0, 3.0,
   1.0, -1.0, 3.0,
   0.1,  0.1, 0.3
], dtype = np.float32)

color =  np.array([
  0.9, 0.5, 0.5, 1.0,
  0.8, 0.8, 0.8, 1.0,
  0.8, 0.8, 0.8, 1.0,
  0.5, 0.9, 0.5, 1.0
], dtype = np.float32)

index = np.array([
  0, 1, 2,
  1, 2, 3
], dtype = np.uint32)

#library = anariLoadLibrary('default', status_handle)
device = anari.newDevice('default')

camera = device.newCamera('perspective')
#anariSetParameter(device, camera, 'aspect', ANARI_FLOAT32, width/height)
camera.setParameter('aspect', anari.FLOAT32, width/height)
#anariSetParameter(device, camera, 'position', ANARI_FLOAT32_VEC3, cam_pos)
camera.setParameter('position',anari.FLOAT32_VEC3, cam_pos)
#anariSetParameter(device, camera, 'direction', ANARI_FLOAT32_VEC3, cam_view)
camera.setParameter('direction',anari.float3,cam_view)
#anariSetParameter(device, camera, 'up', ANARI_FLOAT32_VEC3, cam_up)
camera.setParameter('up',anari.float3,cam_up)
#anariCommitParameters(device, camera)
camera.commitParameters()

world = device.newWorld()

mesh = device.newGeometry('triangle')

array = device.newArray(anari.FLOAT32_VEC3,vertex)
mesh.setParameter('vertex.position', anari.ARRAY, array)

array = device.newArray(anari.FLOAT32_VEC4, color)
mesh.setParameter('vertex.color', anari.ARRAY, array)

array = device.newArray(anari.UINT32_VEC3 , index)
mesh.setParameter('primitive.index', anari.ARRAY, array)
mesh.commitParameters()

material = device.newMaterial('matte')
material.commitParameters()

surface = device.newSurface()
surface.setParameter('geometry', anari.GEOMETRY, mesh)
surface.setParameter('material', anari.MATERIAL, material)
surface.commitParameters()

world.setParameterArray('surface', anari.SURFACE, [ surface ])

light = device.newLight('directional')
light.setParameter('direction', anari.float3, (0,0,1))
light.setParameter('irradiance', anari.float, 1)
light.commitParameters()

array = device.newArray(anari.LIGHT, [light])
world.setParameter('light', anari.ARRAY1D, array)

world.commitParameters()


# background gradient: use an image of 1 pixel wide and 2 pixels high
bg_values = np.array(((.9,.9,.9,1.),(.15,.25,.8,1.)), dtype=np.float32).reshape((4,1,2))
bg_gradient = device.newArray(anari.float4, bg_values)


renderer = device.newRenderer('default')
if anari.has_cuda_capable_gpu():
    renderer.setParameter('pixelSamples', anari.INT32, 128)
else:
    renderer.setParameter('pixelSamples', anari.INT32, 8)
renderer.setParameter('background', anari.ARRAY, bg_gradient)
renderer.setParameter('ambientRadiance',anari.FLOAT32, .3)
renderer.commitParameters()


frame = device.newFrame()

frame.setParameter('size', anari.uint2, [width, height])

frame.setParameter('channel.color', anari.DATA_TYPE, anari.UFIXED8_VEC4)
#frame.setParameter('channel.color', anari.DATA_TYPE, anari.FLOAT32_VEC4)
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
        printf('sample01.py [-o outfile.jpg]')
        sys.exit(0)
    elif opt == '-o':
        out_file_name = arg

if out_file_name == '':
    plt.imshow(pixels)
    plt.gca().invert_yaxis()
    plt.show()
else:
    im = PIL.Image.fromarray(pixels)
    im.convert('RGB').save(out_file_name)




