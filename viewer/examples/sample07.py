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


# sample that tests 'curves' geometry by generating random 3D bezier
# curves

import numpy as np
import pynari as anari
import random
import math

from .anari_scene_base import AnariSceneBase

class AnariScene(AnariSceneBase):
    def __init__(self):
        super().__init__()

    def create_world(self, device):
        """Create and populate the scene with objects."""

        random.seed(80577)

        # radius to use for emitted curves:
        curve_radius = .02
        curve_radius = .04


        # the linear line segments representing the curves, in python arrays
        curves_vertex_position = []
        curves_vertex_radius = []
        curves_primitive_index = []

        spheres_vertex_position = []
        spheres_vertex_radius = []

        def lerp_cp(t,p0,p1):
            x0 = p0[0]
            y0 = p0[1]
            z0 = p0[2]
            x1 = p1[0]
            y1 = p1[1]
            z1 = p1[2]
            x = (1-t)*x0+t*x1
            y = (1-t)*y0+t*y1
            z = (1-t)*z0+t*z1
            return (x,y,z)

        def eval_cubic(t,p00,p01,p02,p03):
            p10 = lerp_cp(t,p00,p01)
            p11 = lerp_cp(t,p01,p02)
            p12 = lerp_cp(t,p02,p03)

            p20 = lerp_cp(t,p10,p11)
            p21 = lerp_cp(t,p11,p12)

            p30 = lerp_cp(t,p20,p21)
            
            return p30
            
        def add_vertex(v):
            position = (v[0],v[1],v[2])
            radius   = curve_radius
            curves_vertex_position.append(position)
            curves_vertex_radius.append(radius)

        def random_sphere(r):
            while True:
                x = 1-2*random.random()
                y = 1-2*random.random()
                z = 1-2*random.random()
                r2 = x*x+y*y+z*z
                if r2 <= 1:
                    r = r / math.sqrt(r2)
                    return np.array((x*r,y*r,z*r),dtype=np.float32)
            
            
        # a curve with
        # four 4D control points (with x,y,z, and radius each)
        # two colors (for start and end)
        def emit_tubes(cp0, cp1, cp2, cp3):
            n_segs = 8
            for i in range(n_segs):
                t0 = i/n_segs
                t1 = (i+1)/n_segs
                p0 = eval_cubic(t0,cp0,cp1,cp2,cp3)
                p1 = eval_cubic(t1,cp0,cp1,cp2,cp3)
                curves_primitive_index.append(len(curves_vertex_position))
                add_vertex(p0)
                add_vertex(p1)

        def emit_spheres(cp0,cp1,cp2,cp3):
            n_spheres = random.randint(14,28)
            for i in range(n_spheres):
                point_on_curve = eval_cubic(random.uniform(0.,1.),cp0,cp1,cp2,cp3)
                offset = random_sphere(curve_radius*random.uniform(.2,.14))
                r = random.uniform(.3,.8)*curve_radius
                spheres_vertex_position.append(point_on_curve+offset)
                spheres_vertex_radius.append(r);

        def create_curves():
            curves = []
            for a in range(30):
                P = np.array((random.random(),random.random(),random.random()),dtype=np.float32)
                delta_length = .1
                delta = np.array([0,0,0],dtype=np.float32)
                #random_sphere(delta_length)
                for b in range(2):
                    # first point:
                    nodes = [ P ]
                    for b in range(3):
                        # three more points, totals three bezier segments
                        if b > 0:
                            delta = delta + random_sphere(delta_length)
                        P = P + delta;
                        nodes.append(P)
                    curves.append(nodes)
            return curves




        # create a "Lambertian" (ie, diffuse) anari matterial, using ANARI's
        # 'matte' material
        def make_spheres_material_ignore():
            mat = device.newMaterial('matte')
            mat.setParameter('color',anari.float3,(.9,.1,.1))
            mat.commitParameters()
            return mat

        def make_spheres_material():
            mat = device.newMaterial('physicallyBased')
            mat.setParameter('baseColor',anari.float3,(.9,.1,.1))
            mat.setParameter('metallic',anari.FLOAT32,.7)
            mat.setParameter('roughness',anari.FLOAT32,.05)
            mat.commitParameters()
            return mat

        def make_curves_material():
            mat = device.newMaterial('physicallyBased')
            #mat.setParameter('baseColor',anari.float4,(.5,.5,.5,.2))
            mat.setParameter('baseColor',anari.float3,(.5,.5,.5))
            mat.setParameter('ior',anari.FLOAT32,1.)
            mat.setParameter('metallic',anari.FLOAT32,0.)
            mat.setParameter('opacity',anari.FLOAT32,0.1)
            mat.commitParameters()
            return mat
            
        def make_anari_curves():
            geom = device.newGeometry('curve')

            np_position = np.array(curves_vertex_position,dtype=np.float32).flatten()
            position = device.newArray(anari.FLOAT32_VEC3,np_position)
            geom.setParameter('vertex.position',anari.ARRAY,position)
            
            np_radius = np.array(curves_vertex_radius,dtype=np.float32).flatten()
            radius = device.newArray(anari.FLOAT32,np_radius)
            geom.setParameter('vertex.radius',anari.ARRAY,radius)

            np_index = np.array(curves_primitive_index,dtype=np.uint32).flatten()
            index = device.newArray(anari.UINT32,np_index)
            geom.setParameter('primitive.index',anari.ARRAY,index)

            geom.commitParameters()

            surf = device.newSurface()
            surf.setParameter('geometry', anari.GEOMETRY, geom)
            surf.setParameter('material', anari.MATERIAL, make_curves_material())
            surf.commitParameters()
            
            return surf

        def make_anari_spheres():
            geom = device.newGeometry('sphere')

            np_position = np.array(spheres_vertex_position,dtype=np.float32).flatten()
            position = device.newArray(anari.FLOAT32_VEC3,np_position)
            geom.setParameter('vertex.position',anari.ARRAY,position)
            
            np_radius = np.array(spheres_vertex_radius,dtype=np.float32).flatten()
            radius = device.newArray(anari.FLOAT32,np_radius)
            geom.setParameter('vertex.radius',anari.ARRAY,radius)

            geom.commitParameters()

            surf = device.newSurface()
            surf.setParameter('geometry', anari.GEOMETRY, geom)
            surf.setParameter('material', anari.MATERIAL, make_spheres_material())
            surf.commitParameters()
            
            return surf


        curves = create_curves()
        for curve in curves:
            emit_tubes(curve[0],curve[1],curve[2],curve[3])
            emit_spheres(curve[0],curve[1],curve[2],curve[3])


        world = device.newWorld()
        spheres_surf = make_anari_spheres()
        curves_surf = make_anari_curves()
        surfaces = device.newArray( anari.SURFACE, [ spheres_surf, curves_surf ])
        world.setParameter('surface', anari.ARRAY, surfaces )

        light = device.newLight('directional')
        light.setParameter('direction', anari.float3, (1,-1,1))
        light.setParameter('irradiance', anari.float, 10)
        light.setParameter('color', anari.float3, (1,1,1))
        light.commitParameters()

        array = device.newArray(anari.LIGHT, [light])
        world.setParameter('light', anari.ARRAY1D, array)

        world.commitParameters()

        return world
