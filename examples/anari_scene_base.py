# // ======================================================================== //
# // Copyright 2024++ Ingo Wald                                               //
# // Copyright 2024++ Milan Jaros - IT4Innovations, VSB-TUO                   //
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

import pynari as anari
import numpy as np

class AnariSceneBase:
    def __init__(self):
        self.device = None
        self.fb_size = None
        self.renderer = None
        self.camera = None
        self.world = None
        self.frame = None

    def create_device(self):
        return anari.newDevice('barney')
        #return anari.newDevice('helide')

    def create_world(self, device):
        """Create and return an ANARI device. Must be implemented by subclass."""
        raise NotImplementedError("Subclasses must implement create_device()")
    
    def create_camera(self):
        return self.device.newCamera('perspective')

    def create_background(self): 
        # background gradient: use an image of 1 pixel wide and 2 pixels high
        bg_values = np.array(((.9,.9,.9,1.),(.15,.25,.8,1.)), dtype=np.float32).reshape((4,1,2))
        return self.device.newArray(anari.float4, bg_values)    

    def create_renderer(self):
        renderer = self.device.newRenderer('default')
        renderer.setParameter('ambientRadiance',anari.FLOAT32, .8)
        renderer.setParameter('pixelSamples', anari.INT32, 1)
        renderer.setParameter('background', anari.ARRAY, self.create_background())
        renderer.commitParameters()

        return renderer 
    
    def use_cuda_glinterop(self):
        return False  # Whether to use CUDA-OpenGL interop    
    
    def use_dearpygui_tf(self):
        return False  # Whether to use DearPyGui

    def anari_init(self,w,h):
        self.fb_size = (w,h)

        self.device = self.create_device()
        self.world = self.create_world(self.device)
        self.camera = self.create_camera()
        self.renderer = self.create_renderer()
        self.frame = self.device.newFrame()

    def anari_render(self,w,h,eye,dir,up,fovy):
        self.fb_size = (w,h)
        self.camera.setParameter('aspect', anari.FLOAT32, self.fb_size[0]/self.fb_size[1])
        self.camera.setParameter('position',anari.FLOAT32_VEC3, eye)
        self.camera.setParameter('direction',anari.float3, dir)
        self.camera.setParameter('up',anari.float3,up)
        self.camera.setParameter('fovy',anari.FLOAT32,fovy*3.14/180)
        self.camera.commitParameters()

        self.frame.setParameter('size', anari.uint2, self.fb_size)
        self.frame.setParameter('channel.color', anari.DATA_TYPE, anari.UFIXED8_VEC4)
        self.frame.setParameter('renderer', anari.OBJECT, self.renderer)
        self.frame.setParameter('camera', anari.OBJECT, self.camera)
        self.frame.setParameter('world', anari.OBJECT, self.world)
        self.frame.commitParameters()

        self.frame.render()

    def anari_fb_map(self):
        if self.use_cuda_glinterop():
            fb_color = self.frame.map('channel.colorCUDA')
        else:
            fb_color = self.frame.map('channel.color')

        return fb_color

    def anari_fb_unmap(self):
        if self.use_cuda_glinterop():
            self.frame.unmap('channel.colorCUDA')
        else:
            self.frame.unmap('channel.color')