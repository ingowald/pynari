#!/usr/bin/python3

# simple test case in which a device creates (and then immediately forgets) a
# triangle geometry.


import pynari as anari

device = anari.newDevice('default')
for i in range(3):
    print('py: creating new geometry')
    geom = device.newGeometry('triangle')


 


