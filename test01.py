
import matplotlib.pyplot as plt
import numpy as np
#from pynari import *
import pynari as anari

width = 1024
height = 768

cam_pos = [0.0, 0.0, 0.0]
cam_up = [0.0, 1.0, 0.0]
cam_view = [0.1, 0.0, 1.0]

bg_color = [1,1,1,1]

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
camera.setParameter_float('aspect', width/height)
#anariSetParameter(device, camera, 'position', ANARI_FLOAT32_VEC3, cam_pos)
camera.setParameter_float3('position',cam_pos)
#anariSetParameter(device, camera, 'direction', ANARI_FLOAT32_VEC3, cam_view)
camera.setParameter_float3('direction',cam_view)
#anariSetParameter(device, camera, 'up', ANARI_FLOAT32_VEC3, cam_up)
camera.setParameter_float3('up',cam_up)
#anariCommitParameters(device, camera)
camera.commitParameters()

world = device.newWorld()

mesh = device.newGeometry('triangle')

array = device.newArray('float3',vertex)
mesh.setParameter('vertex.position', array)

array = device.newArray('float4', color)
mesh.setParameter('vertex.color', array)

array = device.newArray('uint3' , index)
mesh.setParameter('primitive.index', array)
mesh.commitParameters()

material = device.newMaterial('matte')
material.commitParameters()

surface = device.newSurface()
surface.setParameter('geometry', mesh)
surface.setParameter('material', material)
surface.commitParameters()

world.setParameter('surface', [ surface ])

#light = anariNewLight(device, 'directional')
#anariCommitParameters(device, light)

#lights = ffi.new('ANARILight[]', [light])
#array = anariNewArray1D(device, lights, ANARI_LIGHT, 1)
#anariSetParameter(device, world, 'light', ANARI_ARRAY1D, array)

world.commitParameters()


renderer = device.newRenderer('default')
renderer.setParameter_float4('background', bg_color)
renderer.setParameter_float('ambientRadiance',1.)
renderer.commitParameters()



frame = device.newFrame()

frame.setParameter_uint2('size', [width, height])

#frame.setParameter_type('channel.color', 'ANARI_UFIXED8_VEC4')
#frame.setParameter_type('channel.color', 'ANARI_UFIXED8_RGBA_SRGB')
frame.setParameter_type('channel.color', 'ANARI_FLOAT32_VEC4')
frame.setParameter('renderer', renderer)
frame.setParameter('camera', camera)
frame.setParameter('world', world)
frame.commitParameters()

frame.render()
fb_color = frame.color()

pixels = np.array(fb_color).reshape([height, width, 4])
plt.imshow(pixels)
plt.show()



