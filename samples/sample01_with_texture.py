# simple feature-example that shows how to add a texture/image sampler to a
# material. Note the changes to sample01:
#
# - adding a 'vertex.attribute0' array with the texture coordinates
# - creating a Array2D of float4 texture values
# - creating a "image2D" sampler object with those texels
# - assigning this sampler object to the 'color' value of the material
#
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
#   0.1,  0.1, 0.3
   1.0,  1.0, 3.0
], dtype = np.float32)

tex_coords = np.array([
   0.0, 0.0, 
   0.0, 1.0, 
   1.0, 0.0, 
   1.0, 1.0, 
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

print('@pynari: -------------------------------------------------------')
print('@pynari: running sample01_with_textures - the original ANARI ')
print('@pynari: "first light" sample adapted to show a simple')
print('@pynari: checkerboard texture on a flat quad')
print('@pynari: -------------------------------------------------------')
device = anari.newDevice('default')

camera = device.newCamera('perspective')
camera.setParameter('aspect', anari.FLOAT32, width/height)
camera.setParameter('position',anari.FLOAT32_VEC3, cam_pos)
camera.setParameter('direction',anari.float3,cam_view)
camera.setParameter('up',anari.float3,cam_up)
camera.commitParameters()

world = device.newWorld()

mesh = device.newGeometry('triangle')

array = device.newArray1D(anari.FLOAT32_VEC3,vertex)
mesh.setParameter('vertex.position', anari.ARRAY1D, array)

array = device.newArray1D(anari.FLOAT32_VEC4, color)
mesh.setParameter('vertex.color', anari.ARRAY1D, array)

array = device.newArray1D(anari.UINT32_VEC3 , index)
mesh.setParameter('primitive.index', anari.ARRAY1D, array)

array = device.newArray1D(anari.FLOAT32_VEC2 , tex_coords)
mesh.setParameter('vertex.attribute0', anari.ARRAY1D, array)

mesh.commitParameters()

# create the texture
texels =  np.array([
    1, 0, 0, 1,
    0, 0, 1, 1,
    1, 0, 0, 1,
    0, 0, 1, 1,

    0, 0, 1, 1,
    1, 0, 0, 1,
    0, 0, 1, 1,
    1, 0, 0, 1,
    
    1, 0, 0, 1,
    0, 0, 1, 1,
    1, 0, 0, 1,
    0, 0, 1, 1,

    0, 0, 1, 1,
    1, 0, 0, 1,
    0, 0, 1, 1,
    1, 0, 0, 1,
], dtype = np.float32)
texels = texels.reshape((
    4,#height
    4,#width
    4#channels
                        ))
texel_array = device.newArray2D(anari.FLOAT32_VEC4 , texels)
sampler = device.newSampler('image2D')
sampler.setParameter("wrapMode",anari.STRING,'mirrorRepeat')
sampler.setParameter("filter",anari.STRING,'nearest')
sampler.setParameter("image",anari.ARRAY2D,texel_array)
sampler.commitParameters()

#material = device.newMaterial('physicallyBased')
#material.setParameter('baseColor', anari.SAMPLER, sampler)
material = device.newMaterial('matte')
material.setParameter('color', anari.SAMPLER, sampler)
material.commitParameters()

surface = device.newSurface()
surface.setParameter('geometry', anari.GEOMETRY, mesh)
surface.setParameter('material', anari.MATERIAL, material)
surface.commitParameters()

world.setParameterArray1D('surface', anari.SURFACE, [ surface ])

light = device.newLight('directional')
light.setParameter('direction', anari.float3, (0,0,1))
light.setParameter('irradiance', anari.float, 1)
light.commitParameters()

array = device.newArray1D(anari.LIGHT, [light])
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
renderer.setParameter('ambientRadiance',anari.FLOAT32, .3)
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
    print(f'@pynari: done. saving to {out_file_name}')
    im.convert('RGB').save(out_file_name)




