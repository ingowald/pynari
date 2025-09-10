#!/usr/bin/python3

# simple test case to create a device and immediately drop it right after, to
# allow backend to check if device gets released.

import pynari as anari

print('creating pynari device')
device = anari.newDevice('default')
print('python releasing pynari device')
device = None


