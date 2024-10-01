import matplotlib.pyplot as plt
import numpy as np
#from pynari import *
import pynari as anari
import random

fb_size = (1600,800)
look_from = (3.,1.,2.)
look_at = (0., 0., 0.)
look_up = (0.,1.,0.)
fovy = 20.

#volume_dims = (512,512,512)
#volume_fileName = 'magnetic_512_volume.raw'

device = anari.newDevice('default')


cell_values = [ 0, 1, 0, 0, 0, 0, 0, 0 ]
volume_dims = (2,2,2)
cell_array = np.array(cell_values,dtype=np.float32).reshape(volume_dims)
structured_data = device.newArray(anari.float,cell_array)

cellSize = (2/volume_dims[0],2/volume_dims[1],2/volume_dims[2])
field = device.newSpatialField('structuredRegular')
field.setParameter('origin',anari.float3,(-1,-1,-1))
field.setParameter('spacing',anari.float3,cellSize)
field.setParameter('data',anari.ARRAY3D,structured_data)
field.commitParameters()

xf = np.array([ 0, 0, 0, 0,
                1, 0, 1, 1,
                0, 1, 0, 1,
                0, 0, 0, 0 ],dtype=np.float32)
xf_array = device.newArray(anari.float4,xf)

volume = device.newVolume('transferFunction1D')
volume.setParameter('color',anari.ARRAY,xf_array)
volume.commitParameters()
                                                    
world = device.newWorld()
#world.setParameterArray('surface', anari.SURFACE, spheres )
world.setParameterArray('volume', anari.VOLUME, [ volume ] )
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

frame.setParameter('channel.color', anari.DATA_TYPE, anari.FLOAT32_VEC4)
frame.setParameter('renderer', anari.OBJECT, renderer)
frame.setParameter('camera', anari.OBJECT, camera)
frame.setParameter('world', anari.OBJECT, world)
frame.commitParameters()

frame.render()
fb_color = frame.get('channel.color')

pixels = fb_color
np.array(fb_color).reshape([fb_size[0], fb_size[1], 4])
plt.imshow(pixels)
plt.gca().invert_yaxis()
plt.show()




