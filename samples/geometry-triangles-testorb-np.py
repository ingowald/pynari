import matplotlib.pyplot as plt
import numpy as np
import sys, getopt
import PIL
import pynari as anari
import sys
import os

executable_path = os.path.abspath(__file__)
executable_directory = os.path.dirname(executable_path)


width = 1024
height = 768

cam_at = [0,.14,0]
cam_dir=(+.2,-.4,1.)
cam_scale =  .4
cam_pos = [cam_at[0]-cam_scale*cam_dir[0],
           cam_at[1]-cam_scale*cam_dir[1],
           cam_at[2]-cam_scale*cam_dir[2]]
cam_up = [0.0, 1.0, 0.0]

device = anari.newDevice('default')

def makeMaterial_metallic(baseColor, roughness):
    mat = device.newMaterial('physicallyBased')
    mat.setParameter('baseColor',anari.float3,baseColor)
    mat.setParameter('metallic',anari.float,.5)
    mat.setParameter('roughness',anari.float,roughness)
    mat.setParameter('clearcoat',anari.float,1.)
    mat.commitParameters()
    return mat
    
def makeMaterial_matte(color):
    mat = device.newMaterial('matte')
    mat.setParameter('color',anari.float3,color)
    mat.commitParameters()
    return mat
    
def createMesh(file_name, material):
    full_file_name = executable_directory+"/"+file_name
    mesh_data = np.load(full_file_name)
    vertex = mesh_data['vertex']
    uv = mesh_data['uv']
    normal = mesh_data['normal']
    geom = device.newGeometry('triangle')
    print("vertices :"+str(vertex))
    geom.setParameterArray1D('vertex.position',anari.float3,vertex)
    geom.setParameterArray1D('vertex.normal',anari.float3,normal)
    geom.commitParameters()
    surf = device.newSurface()
    surf.setParameter('material',anari.MATERIAL, material)
    surf.setParameter('geometry',anari.GEOMETRY, geom)
    surf.commitParameters()
    return surf

camera = device.newCamera('perspective')
camera.setParameter('aspect', anari.FLOAT32, width/height)
camera.setParameter('position',anari.FLOAT32_VEC3, cam_pos)
camera.setParameter('direction',anari.float3,cam_dir)
camera.setParameter('up',anari.float3,cam_up)
camera.commitParameters()

world = device.newWorld()

#orb_material = makeMaterial_metallic((.5,.5,.9),.02)
orb_material = makeMaterial_metallic((0,.11,.32),0.)
equation_material = makeMaterial_metallic([.77,.75,0],0.)
#floor_material = makeMaterial_matte((.2,.2,.2))
floor_material = makeMaterial_metallic((.2,.2,.2),0.)

base = createMesh('testorb-base.npz',orb_material)
inner_sphere = createMesh('testorb-inner-sphere.npz',orb_material)
outer_sphere = createMesh('testorb-outer-sphere.npz',orb_material)
floor = createMesh('testorb-floor.npz',floor_material)
equation = createMesh('testorb-equation.npz',equation_material)
                          
world.setParameterArray1D('surface', anari.SURFACE,
                        [ base, inner_sphere, outer_sphere, equation, floor ])


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

out_file_name = 'testOrb.png'
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
    im = im.transpose(PIL.Image.FLIP_TOP_BOTTOM)
    print(f'@pynari: done. saving to {out_file_name}')
    im.convert('RGB').save(out_file_name)

