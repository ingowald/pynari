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
#if PYNARI_HAVE_CUDA
# include <cuda_runtime.h>
#endif

namespace pynari {

  Frame::Frame(Device::SP device)
    : Object(device)
  {
    handle = anari::newObject<anari::Frame>(device->handle);
    anari::setParameter(device->handle, handle, "channel.color",
                        (anari::DataType)ANARI_UFIXED8_RGBA_SRGB);
  }

  void Frame::render()
  {
    PING; PRINT(handle);
    anariRenderFrame(device->handle, (ANARIFrame)handle);
    anariFrameReady(device->handle, (ANARIFrame)handle, ANARI_WAIT);
  }

  uint64_t Frame::map(const std::string &channel)
  {
    ANARIDataType pixelType;
    uint32_t width, height;
    const void *ptr = anariMapFrame(device->handle, (ANARIFrame)handle,
                                    channel.c_str(), &width, &height, &pixelType);
    return (uint64_t)ptr;
  }
  
  void Frame::unmap(const std::string &channel)
  {
    anariUnmapFrame(device->handle, (ANARIFrame)handle, channel.c_str());
  }

  void Frame::readGPU(uint64_t devicePtr, const std::string &channel)
  {
#if PYNARI_HAVE_CUDA
    size_t numBytes = 0;
    void *destPtr = (void *)devicePtr;
    uint32_t width, height;
    ANARIDataType pixelType;
    const void *srcPtr
      = anariMapFrame(device->handle,(ANARIFrame)this->handle,
                      channel.c_str(),
                      &width,&height,&pixelType);

    if (pixelType == ANARI_UFIXED8_VEC4 ||
        pixelType == ANARI_UFIXED8_RGBA_SRGB)
      numBytes = width*height*sizeof(uint32_t);
    else
      throw std::runtime_error
        ("pynari::FRame::readGPU currently only supporting frame buffers "
         "of format "
         "'ANARI_UFIXED8_VEC4', or "
         "'ANARI_UFIXED8_RGBA_SRGB'");
    
    cudaMemcpy(destPtr,srcPtr,numBytes,cudaMemcpyDefault);
    anariUnmapFrame(device->handle,(ANARIFrame)handle,channel.c_str());
#else
    throw std::runtime_error("pnari::Frame::readGPU() requires building with CUDA support");
#endif
  }
  
  py::object Frame::get(const std::string &channelName)
  {
    if (channelName == "channel.color") {
      uint32_t width, height;
      ANARIDataType pixelType;

      const void *mapped
        = anariMapFrame(device->handle,(ANARIFrame)this->handle,
                        "channel.color",
                        &width,&height,&pixelType);
      py::object frame;
      if (pixelType == ANARI_FLOAT32_VEC4) {
        frame
          = py::array_t<float>(
                               /* numpy shape */
                               {(int)height,(int)width,4},
                               /*! C strides */
                               {width*4*sizeof(float),
                                4*sizeof(float),
                                sizeof(float)},
                               (float*)mapped);
      }
      else if (pixelType == ANARI_UFIXED8_VEC4
               ||
               pixelType == ANARI_UFIXED8_RGBA_SRGB) {
        frame
          = py::array_t<uint8_t>(
                                  /* numpy shape */
                                  {(int)height,(int)width,4},
                                  /*! C strides */
                                  {width*4*sizeof(uint8_t),
                                   4*sizeof(uint8_t),
                                   sizeof(uint8_t)},
                                  (uint8_t*)mapped);
      }
      else {
        throw std::runtime_error("currently only supporting frame buffers "
                                 "of format 'ANARI_FLOAT32_VEC4', "
                                 "'ANARI_UFIXED8_VEC4', or "
                                 "'ANARI_UFIXED8_RGBA_SRGB'");
      }

      anariUnmapFrame(device->handle,(ANARIFrame)handle,"channel.color");
      return frame;
    }

    throw std::runtime_error("unsupported channel name '"+channelName
                             +"' in pynari::Frame::get()");
  }

}
