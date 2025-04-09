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
import random

from .anari_scene_base import AnariSceneBase

class AnariScene(AnariSceneBase):
    def __init__(self):
        super().__init__()

    def create_world(self, device):
        """Create and populate the scene with objects."""

        random.seed(80577)

        def add_sphere(pos, radius, material):
            geom = device.newGeometry('sphere')
            array = device.newArray(anari.FLOAT32_VEC3,np.array(pos,dtype=np.float32))
            geom.setParameter('vertex.position',anari.ARRAY,array)
            geom.setParameter('radius',anari.FLOAT32,radius)
            geom.commitParameters()

            surf = device.newSurface()
            surf.setParameter('geometry', anari.GEOMETRY, geom)
            surf.setParameter('material', anari.MATERIAL, material)
            surf.commitParameters()
            
            spheres.append(surf)

        def make_pbr(roughness, metallic):
            mat = device.newMaterial('physicallyBased')
            mat.setParameter('baseColor',anari.float3,(.2,.8,.2))
            mat.setParameter('ior',anari.FLOAT32,1.45)
            mat.setParameter('metallic',anari.FLOAT32,metallic)
            mat.setParameter('specular',anari.FLOAT32,0.)
            mat.setParameter('roughness',anari.float,roughness)
            mat.commitParameters()
            return mat
            
        def create_spheres():
            grid_size = 9
            radius = .9 / grid_size
            for a in range(grid_size):
                for b in range(grid_size):
                    choose_mat = random.random();
                    center = ((a + .5 - grid_size/2) / (grid_size/2),
                            (b + .5 - grid_size/2) / (grid_size/2),
                            0.)
                    metallic = (a+.5)/grid_size
                    roughness = (b+.5)/grid_size
                    add_sphere(center,radius,make_pbr(roughness*roughness,metallic))


        spheres = []

        create_spheres()

        world = device.newWorld()
        light = device.newLight('directional')
        light.setParameter('direction', anari.float3, (-.6,-1,+.5))
        light.setParameter('color', anari.float3, (1,1,1))
        light.setParameter('irradiance', anari.float, 1)
        light.commitParameters()

        array = device.newArray(anari.LIGHT, [light])
        world.setParameter('light', anari.ARRAY1D, array)


        world.setParameterArray('surface', anari.SURFACE, spheres )
        world.commitParameters()

        return world