
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
camera.setParameter_float3('direction',cam_view)
#anariSetParameter(device, camera, 'direction', ANARI_FLOAT32_VEC3, cam_view)
camera.setParameter_float3('up',cam_up)
#anariSetParameter(device, camera, 'up', ANARI_FLOAT32_VEC3, cam_up)
#anariCommitParameters(device, camera)
camera.commitParameters

