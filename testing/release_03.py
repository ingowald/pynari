#!/usr/bin/python3

# simple test case in which a device creates (and then immediately forgets) a
# triangle geometry.


import pynari as anari
import gc

device = anari.newDevice('default')
frame = device.newFrame()
for i in range(3):
    print('==================================================================')
    print('py: creating new material')
    mat = device.newMaterial('physicallyBased')
    mat.commitParameters()
    print('py: creating new geometry')
    geom = device.newGeometry('triangle')
    geom.commitParameters()
    
    surface = device.newSurface()
    surface.setParameter('geometry', anari.GEOMETRY, geom)
    surface.setParameter('material', anari.MATERIAL, mat)
    surface.commitParameters()

    print('py: explicitly python-releasing surface')
    surface = None
    frame.render()
    


