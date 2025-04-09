# // ======================================================================== //
# // Copyright 2024++ Ingo Wald                                               //
# //                                                                          //
# // Licensed under the Apache License, Version 2.0 (the "License");          //
# // you may not use this file except in compliance with the License.         //
# // You may obtain a copy of the License at                                  //
# //                                                                          //
# //     http://www.apache.org/licenses/LICENSE-2.0                           //
# //                                                                          //
# // Unless required by applicable law or agreed to in writing, software      //
# // distributed under the License is distributed on an "AS IS" BASIS,        //
# // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
# // See the License for the specific language governing permissions and      //
# // limitations under the License.                                           //
# // ======================================================================== //

import numpy as np

import pynari as anari

from .anari_scene_base import AnariSceneBase

class AnariScene(AnariSceneBase):
    def __init__(self):
        super().__init__()

    def create_world(self, device):
        """Create and populate the scene with objects."""

        vertex = np.array([
        -1.0, -1.0, 3.0,
        -1.0,  1.0, 3.0,
        1.0, -1.0, 3.0,
        0.1,  0.1, 0.3
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
        print('@pynari: running sample01 - the original ANARI "first light" ')
        print('@pynari: sample that just renders a simple pair of triangles')
        print('@pynari: -------------------------------------------------------')        

        world = device.newWorld()

        mesh = device.newGeometry('triangle')

        array = device.newArray(anari.FLOAT32_VEC3,vertex)
        mesh.setParameter('vertex.position', anari.ARRAY, array)

        array = device.newArray(anari.FLOAT32_VEC4, color)
        mesh.setParameter('vertex.color', anari.ARRAY, array)

        array = device.newArray(anari.UINT32_VEC3 , index)
        mesh.setParameter('primitive.index', anari.ARRAY, array)
        mesh.commitParameters()

        material = device.newMaterial('matte')
        material.commitParameters()

        surface = device.newSurface()
        surface.setParameter('geometry', anari.GEOMETRY, mesh)
        surface.setParameter('material', anari.MATERIAL, material)
        surface.commitParameters()

        world.setParameterArray('surface', anari.SURFACE, [ surface ])

        light = device.newLight('directional')
        light.setParameter('direction', anari.float3, (0,0,1))
        light.setParameter('irradiance', anari.float, 1)
        light.commitParameters()

        array = device.newArray(anari.LIGHT, [light])
        world.setParameter('light', anari.ARRAY1D, array)

        world.commitParameters()

        return world


