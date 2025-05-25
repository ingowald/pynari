import numpy as np
#from pynari import *
import pynari as anari
import random
import sys, getopt, PIL

from PIL import Image

fb_size = (1600,800)
look_from = (5.,-10.,4.)
look_at = (5., 0.5, 2.5)
look_from = (7.36, 4.95, 5.43)
fovy = 40.
look_at = (7.41, 0.20, 2.65)
look_up = (0.,0.,1.)
fovy = 50.
out_file_name = 'unstructured-vertexCentric.jpg'

print('@pynari: -------------------------------------------------------')
print('@pynari: unstructured data, vertex centric scalars')
print('@pynari: -------------------------------------------------------')

device = anari.newDevice('default')

vertex_position = np.array([
    [0, 0, 0],
    [1, 0, 0],
    [0, 1, 0],
    [2, 0, 0],
    [3, 0, 0],
    [3, 1, 0],
    [2, 1, 0],
    [4, 0, 0],
    [5, 0, 0],
    [4, 1, 0],
    [4, 0, 1],
    [6, 0, 0],
    [7, 0, 0],
    [7, 1, 0],
    [6, 1, 0],
    [6.5, 0.5, 1],
    [8, 0, 0],
    [9, 0, 0],
    [8, 0, 1],
    [8, 1, 0],
    [9, 1, 0],
    [8, 1, 1],
    [10, 0, 0],
    [11, 0, 0],
    [11, 1, 0],
    [10, 1, 0],
    [10, 0, 1],
    [11, 0, 1],
    [11, 1, 1],
    [10, 1, 1],
    [0, 0, 4],
    [1, 0, 4.5],
    [0, 1, 5],
    [2, 0, 4],
    [3, 0, 4.33],
    [3, 1, 4.66],
    [2, 1, 5],
    [4, 0, 4],
    [5, 0, 4],
    [4, 1, 4],
    [4, 0, 5],
    [6, 0, 4],
    [7, 0, 3],
    [7, 1, 4],
    [6, 1, 3],
    [6.5, 0.5, 5],
    [7.13337, 0, 3.50105],
    [8.86821, 0, 3.50381],
    [7.99681, 0, 4.99999],
    [7.50036, 1, 4.86623],
    [7.5045, 1, 3.13139],
    [8.99999, 1, 4.00398],
    [10.7068, 0, 4.70739],
    [10.708, 0, 3.29374],
    [9.2943, 0, 3.29149],
    [9.29093, 0, 4.70513],
    [10.708, 1, 3.29374],
    [9.2943, 1, 3.29149],
    [9.29093, 1, 4.70513],
    [10.7046, 1, 4.70964],
    ],dtype=np.float32)
                    
index = np.array([
    ## tris
    #0,1,2
    #30,31,32,
    ## quads
    #3, 4, 5, 6, 
    #33, 34, 35, 36,
    
    #tets
    7, 8, 9, 10, 
    37, 38, 39, 40, 
    #, pyrs
    11, 12, 13, 14, 15, 
    41, 42, 43, 44, 45,
    #, wedges
    16, 17, 18, 19, 20, 21, 
    46, 47, 48, 49, 50, 51,
    #, hexes
    22, 23, 24, 25, 26, 27, 28, 29, 
    52, 53, 54, 55, 56, 57, 58, 59,
    ],dtype=np.uint32)

cell_index = np.array([
    0,4,8,
    13,18,
    24,30,
    38
                      ],dtype=np.uint32)
# vtk style numbering:
elt_tet=10
elt_hex=12
elt_wedge=13
elt_pyr=14
cell_type = np.array([elt_tet,elt_tet,#elt_tet,elt_tet,
                          elt_pyr,elt_pyr,
                          elt_wedge,elt_wedge,
                          elt_hex,elt_hex],dtype=np.uint8)
# for per vertex
vertex_data = np.array([
    0, 0.5, 1, 0, 0.333333, 0.666667, 1, 0, 0.333333, 0.666667, 1, 0, 0.25, 0.5, 0.75, 1, 0, 0.2, 0.4, 0.6, 0.8, 1, 0, 0.142857, 0.285714, 0.428571, 0.571429, 0.714286, 0.857143, 1, 0, 0.5, 1, 0, 0.333333, 0.666667, 1, 0, 0.333333, 0.666667, 1, 0, 0.25, 0.5, 0.75, 1, 0, 0.2, 0.4, 0.6, 0.8, 1, 0, 0.142857, 0.285714, 0.428571, 0.571429, 0.714286, 0.857143, 1],dtype=np.float32)

args = sys.argv[1:]
opts, args = getopt.getopt(args,"ho:",["help","output="])
for opt,arg in opts:
    if opt == '-h':
        printf('unstructured-vertexCentric-uint32.py [-o outfile.jpg]')
        printf("# should render an image of some data-per-vertex unstructured cells")
        sys.exit(0)
    elif opt == '-o':
        out_file_name = arg

spatial_field = device.newSpatialField('unstructured')
spatial_field.setParameterArray('cell.type',anari.UINT8,cell_type)
spatial_field.setParameterArray('cell.index',anari.UINT32,cell_index)
spatial_field.setParameterArray('index',anari.UINT32,index)
spatial_field.setParameterArray('vertex.position',anari.FLOAT32_VEC3,vertex_position)
spatial_field.setParameterArray('vertex.data',anari.FLOAT32,vertex_data)
spatial_field.commitParameters()

xf = np.array([0, 0, 1, 1,
               0, 1, 1, 1,
               0, 1, 0, 1,
               1, 1, 0, 1,
               1, 0, 0, 1,
               1, 0, 1, 1,
               0, 0, 1, 1,
               ],dtype=np.float32)
# for cell-centered data, make sure we see all the prims
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
#bg_values = np.array(((.9,.9,.9,1.),(.15,.25,.8,1.)),
#                     dtype=np.float32).reshape((2,1,4))
#bg_gradient = device.newArray(anari.float4, bg_values)


renderer = device.newRenderer('default')
renderer.setParameter('ambientRadiance',anari.FLOAT32, .5)
#renderer.setParameter('background', anari.ARRAY, bg_gradient)
if anari.has_cuda_capable_gpu():
    # actually we have denoising on the gpu, so probably need way less...
    renderer.setParameter('pixelSamples', anari.INT32, 1024)
else:
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
pixels = np.array(fb_color)

im = Image.fromarray(pixels)
im = im.transpose(PIL.Image.FLIP_TOP_BOTTOM)
im = im.convert('RGB')
print(f'@pynari: done. saving to {out_file_name}')
im.save(out_file_name)





