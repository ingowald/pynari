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

        print('@pynari: -------------------------------------------------------')
        print('@pynari: running sample02 - the first real barney-based sample ')
        print('@pynari: that renders a pynari-version of Pete Shirley\'s famous ')
        print('@pynari: RTOW (ray tracing in one week-end) scene.')
        print('@pynari: -------------------------------------------------------')
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

        # create a "Lambertian" (ie, diffuse) anari matterial, using ANARI's
        # 'matte' material
        def make_lambertian(r,g,b):
            mat = device.newMaterial('matte')
            mat.setParameter('color',anari.float3,(r,g,b))
            mat.commitParameters()
            return mat

        def make_dielectric(ior):
            mat = device.newMaterial('physicallyBased')
            mat.setParameter('baseColor',anari.float3,(1.,1.,1.))
            mat.setParameter('transmission',anari.FLOAT32,1.)
            mat.setParameter('specular',anari.FLOAT32,0.)
            mat.setParameter('metallic',anari.FLOAT32,0.)
            mat.setParameter('ior',anari.FLOAT32,ior)
            mat.commitParameters()
            return mat

        def make_metal(albedo,fuzz):
            mat = device.newMaterial('physicallyBased')
            mat.setParameter('baseColor',anari.float3,(albedo[0],albedo[1],albedo[2]))
            mat.setParameter('ior',anari.FLOAT32,1.45)
            mat.setParameter('metallic',anari.FLOAT32,1.)
            mat.setParameter('specular',anari.FLOAT32,0.)
            #mat.setParameter('roughness',anari.FLOAT32,0.2)
            mat.setParameter('roughness',anari.FLOAT32,fuzz)
            mat.commitParameters()
            return mat
            
        def create_spheres():
            add_sphere((0.,-1000.,-1),1000.,make_lambertian(.5,.5,.5))
            add_sphere((0., 1., 0.), 1., make_dielectric(1.5))
            add_sphere((-4.,1.,0.),1.,make_lambertian(.4,.2,.1))
            add_sphere((4.,1.,0.),1.,make_metal((.7,.6,.5),0.))
            for a in range(-11,12):
                for b in range(-11,12):
                    choose_mat = random.random();
                    center = (a + random.random(), 0.2, b + random.random());
                    if choose_mat < 0.8:
                        x = random.random()
                        y = random.random()
                        z = random.random()
                        add_sphere(center,.2,make_lambertian(x*x,y*y,z*z))
                    elif choose_mat < .95:
                        x = random.random()
                        y = random.random()
                        z = random.random()
                        x = .5*(1+x)
                        y = .5*(1+y)
                        z = .5*(1+z)
                        add_sphere(center,.2,make_metal((x,y,z),.2))
                    else:
                        add_sphere(center,.2,make_dielectric(1.5))


        spheres = []

        create_spheres()

        world = device.newWorld()
        world.setParameterArray('surface', anari.SURFACE, spheres )
        world.commitParameters()

        return world
