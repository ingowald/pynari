// ======================================================================== //
// Copyright 2024++ Ingo Wald                                               //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "pynari/Frame.h"

namespace pynari {

  Frame::Frame(Device::SP device)
    : Object(device)
  {
    handle = anari::newObject<anari::Frame>(device->handle);
  }

  void Frame::render()
  {
    anariRenderFrame(device->handle,handle);
    anariFrameReady(device->handle, handle, ANARI_WAIT);
  }
 
  py::object Frame::color()
  {
    uint32_t width, height;
    ANARIDataType pixelType;

    const void *mapped
      = anariMapFrame(device->handle,handle,
                      "channel.color",
                      &width,&height,&pixelType);
    py::object frame;
    if (pixelType == ANARI_FLOAT32_VEC4) {
      frame
        = py::array_t<float>(
                             {(int)height,(int)width,4}, // shape
                             // {(int)width,(int)height,4}, // shape
                             {width*4*sizeof(float),
                              4*sizeof(float),
                              sizeof(float)}, // C-style contiguous strides for double
                             (float*)mapped);
    }
    else
      throw std::runtime_error("currently only supporting frame buffers "
                               "of format 'ANARI_FLOAT32_VEC4");
                               
    anariUnmapFrame(device->handle,handle,"color");
    return frame;
  }

}
