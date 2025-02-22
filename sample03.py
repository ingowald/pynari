import matplotlib.pyplot as plt
import numpy as np
#from pynari import *
import pynari as anari
import random
import sys, getopt,PIL

fb_size = (1600,800)
look_from = (-3.,2.,-2.)
look_at = (0., 0., 0.)
look_up = (0.,1.,0.)
fovy = 40.

#volume_dims = (512,512,512)
#volume_fileName = 'magnetic_512_volume.raw'

device = anari.newDevice('default')

def get_volume_2():
    cell_values = [ 0, 1, 0, 0, 0, 0, 0, 0 ]
    volume_dims = (2,2,2)
    return cell_values, volume_dims
    
def get_volume_3():
    cell_values = [ 1, 0, 1,
                    0, 0, 0,
                    1, 0, 1,
                    #
                    0, 0, 0,
                    0, 1, 0,
                    0, 0, 0,
                    #
                    1, 1, 1,
                    1, 0, 1,
                    1, 1, 1
                   ]
    volume_dims = (3,3,3)
    return cell_values, volume_dims
    
def get_volume_4():
    cell_values = [ 0, 0, 0, 0,
                    0, 0, 0, 0, 
                    0, 0, 0, 0, 
                    0, 0, 0, 0,
                    #
                    0, 0, 0, 0, 
                    0, 1, 1, 0, 
                    0, 1, 1, 0, 
                    0, 0, 0, 0,
                    #
                    0, 0, 0, 0, 
                    0, 1, 1, 0, 
                    0, 1, 1, 0, 
                    0, 0, 0, 0,
                    #
                    0, 0, 0, 0, 
                    0, 0, 0, 0, 
                    0, 0, 0, 0, 
                    0, 0, 0, 0, 
                   ]
    volume_dims = (4,4,4)
    return cell_values, volume_dims

def get_volume_5():
    cell_values = [ 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 
                0, 0, 0, 0, 0,  
                0, 0, 0, 0, 0, 
                0, 0, 0, 0, 0,
                #
                0, 0, 0, 0, 0, 
                0, 1, 1, 1, 0, 
                0, 1, 1, 1, 0, 
                0, 1, 1, 1, 0, 
                0, 0, 0, 0, 0,
                #
                0, 0, 0, 0, 0, 
                0, 1, 1, 1, 0, 
                0, 1, 1, 1, 0, 
                0, 1, 1, 1, 0, 
                0, 0, 0, 0, 0,
                #
                0, 0, 0, 0, 0, 
                0, 1, 1, 1, 0, 
                0, 1, 1, 1, 0, 
                0, 1, 1, 1, 0, 
                0, 0, 0, 0, 0,
                #
                0, 0, 0, 0, 0, 
                0, 0, 0, 0, 0, 
                0, 0, 0, 0, 0, 
                0, 0, 0, 0, 0, 
                0, 0, 0, 0, 0, 
               ]
    volume_dims = (5,5,5)
    return cell_values, volume_dims

cell_values, volume_dims = get_volume_4()

out_file_name = ''
args = sys.argv[1:]
opts, args = getopt.getopt(args,"hmo:",["magnetic","help","output="])
for opt,arg in opts:
    if opt == '-h':
        printf('sample03.py [-o outfile.jpg]')
        sys.exit(0)
    elif opt == "-m":
        volume_dims = (512,512,512)
        cell_values = np.fromfile('magnetic-512-volume.raw', dtype=np.float32)
        cell_values = np.array(cell_values).reshape(volume_dims)
        look_from = (-.5,2.,1.)
        look_at = (0., 0., 0.)
        look_up = (0.,0.,1.)
        fovy = 60
    elif opt == '-o':
        out_file_name = arg



cell_array = np.array(cell_values,dtype=np.float32).reshape(volume_dims)
structured_data = device.newArray(anari.float,cell_array)

cellSize = (2/(volume_dims[0]-1),2/(volume_dims[1]-1),2/(volume_dims[2]-1))
spatial_field = device.newSpatialField('structuredRegular')
spatial_field.setParameter('origin',anari.float3,(-1,-1,-1))
spatial_field.setParameter('spacing',anari.float3,cellSize)
spatial_field.setParameter('data',anari.ARRAY3D,structured_data)
spatial_field.commitParameters()

xf = np.array([0, 0, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0,
               0, 0, 1, 0,
               0, 0, 1, 1,
               0, 1, 0, 1,
               0, 1, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0
               ],dtype=np.float32)
xf_array = device.newArray(anari.float4,xf)

volume = device.newVolume('transferFunction1D')
volume.setParameter('color',anari.ARRAY,xf_array)
volume.setParameter('value',anari.SPATIAL_FIELD,spatial_field)
volume.setParameter('unitDistance',anari.FLOAT32,10.)
volume.commitParameters()
                                                    
world = device.newWorld()
#world.setParameterArray('surface', anari.SURFACE, spheres )
world.setParameterArray('volume', anari.VOLUME, [ volume ] )
light = device.newLight('directional')
light.setParameter('direction', anari.float3, ( 1., -1., -1. ) )
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
bg_values = np.array(((.9,.9,.9,1.),(.15,.25,.8,1.)),
                     dtype=np.float32).reshape((4,1,2))
bg_gradient = device.newArray(anari.float4, bg_values)


renderer = device.newRenderer('default')
renderer.setParameter('ambientRadiance',anari.FLOAT32, 1.)
renderer.setParameter('background', anari.ARRAY, bg_gradient)
if anari.has_cuda_capable_gpu():
    # actually we have denoising on the gpu, so probably need way less...
    renderer.setParameter('pixelSamples', anari.INT32, 1024)
else:
    renderer.setParameter('pixelSamples', anari.INT32, 16)
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

if out_file_name == '':
    plt.imshow(pixels)
    plt.gca().invert_yaxis()
    plt.show()
else:
    im = PIL.Image.fromarray(pixels)
    im = im.transpose(PIL.Image.FLIP_TOP_BOTTOM)
    im = im.convert('RGB')
    im.save(out_file_name)





