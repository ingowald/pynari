#!/usr/bin/python3

# simple test case to create a device and immediately drop it right after, to
# allow backend to check if device gets released. device will not create
# anything, and entire device does.

import pynari as anari

for i in range(100):
    print('creating pynari device')
    device = anari.newDevice('default')
    print('python releasing pynari device')
    device = None
 


