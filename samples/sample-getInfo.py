import matplotlib.pyplot as plt
import numpy as np
import sys, getopt
import PIL

#from pynari import *
import pynari as anari

print('@pynari: -------------------------------------------------------')
print('@pynari: running getInfo.py sample" ')
print('@pynari: -------------------------------------------------------')
device = anari.newDevice('default')

supported_renderer = device.getObjectSubtypes(anari.RENDERER)
print("device supports these types of 'anari.RENDERER': "+str(supported_renderer))
supported_light = device.getObjectSubtypes(anari.LIGHT)
print("device supports these types of 'anari.LIGHT': "+str(supported_light))


renderer_info = device.getObjectInfo(anari.RENDERER,'default')
#renderer_params = device.getObjectParameterInfo(anari.RENDERER,'default')
print("renderer info: "+str(renderer_info))
print("renderer understands these parameters:")
for param in renderer_info['parameters']:
    info = device.getParameterInfo(anari.RENDERER,'default',param['name'],param['type'])
    print(f' {param} : {info}')
