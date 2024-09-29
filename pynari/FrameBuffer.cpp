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

#include "pynari/FrameBuffer.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

namespace pynari {

  FrameBuffer::FrameBuffer(Device::SP device)
    : Object(device)
  {
    handle = anari::newObject<anari::Frame>(device->handle);
  }

  void FrameBuffer::resize(math::int2 size)
  {
    this->size = size;
    color.resize(size.x*size.y);
    depth.resize(size.x*size.y);
    // bnFrameBufferResize(handle,size.x,size.y,
    //                     color.data(),depth.data());
  }
    
  void FrameBuffer::save(const std::string &outFileName)
  {
    const uint32_t *fb
      = (const uint32_t*)color.data();
    
    stbi_flip_vertically_on_write(true);
    stbi_write_png(outFileName.c_str(),size.x,size.y,4,
                   fb,size.x*sizeof(uint32_t));
  }

}
