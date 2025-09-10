#!/usr/bin/python3

# simple test case in which a device creates and releases a world with a two
# surfaces.


import pynari as anari
import gc

device = anari.newDevice('default')
frame = device.newFrame()
renderer = device.newRenderer('default')
renderer.commitParameters()
camera = device.newCamera('perspective')
camera.commitParameters()

for i in range(4):
    print('==================================================================')
    print('py: creating new material')
    mat = device.newMaterial('physicallyBased')
    mat.commitParameters()
    print('py: creating new geometry')
    geom1 = device.newGeometry('triangle')
    geom1.commitParameters()
    geom2 = device.newGeometry('triangle')
    geom2.commitParameters()

    surf1 = device.newSurface()
    surf1.setParameter('geometry', anari.GEOMETRY, geom1)
    surf1.setParameter('material', anari.MATERIAL, mat)
    surf1.commitParameters()
    surf2 = device.newSurface()
    surf2.setParameter('geometry', anari.GEOMETRY, geom1)
    surf2.setParameter('material', anari.MATERIAL, mat)
    surf2.commitParameters()

    world = device.newWorld()
    world.setParameterArray1D('surface', anari.SURFACE, [ surf1, surf2 ])
    world.commitParameters()

    frame.setParameter('renderer', anari.OBJECT, renderer)
    frame.setParameter('camera', anari.OBJECT, camera)
    frame.setParameter('world', anari.OBJECT, world)
    frame.commitParameters()
    print("$$$$$$$$$$$ RENDER()")
    frame.render()
    print("$$$$$$$$$$$ clear world")
    world = None
    print("$$$$$$$$$$$ COLLECT()")
    gc.collect()
    


