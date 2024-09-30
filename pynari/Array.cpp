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

#include "pynari/Array.h"
#include <cstdlib>

namespace pynari {

  template<typename T, int D>
  anari::Array import1D(anari::Device device,
                        ANARIDataType anariType,
                        const py::buffer_info &info,
                        const py::buffer &buffer)
  {
    py::array_t<T> asArray = py::cast<py::array_t<T>>(buffer);
    int numElems = asArray.shape()[0] / D;
    anari::Array handle = anari::newArray1D(device,anariType,numElems);
    void *ptr = anariMapArray(device,handle);
    auto buf = asArray.request();
    const T *elems = (T*)buf.ptr;
    ::memcpy(ptr,elems,numElems*sizeof(T)*D);
    anariUnmapArray(device,handle);
    return handle;
  }

  void Array::assignTo(Object::SP object,
                       anari::DataType intendedType,
                       const std::string &name) 
  {
    switch(nDims) {
    case 1:
      anariSetParameter(device->handle,object->getHandle(),
                        name.c_str(),ANARI_ARRAY1D,&this->handle);
      return;
    default:
      throw std::runtime_error("Array::assignTo not implemented for nDims="
                               +std::to_string(nDims)); 
    }
  }
  
  anari::Array importArray1D(anari::Device device,
                             anari::DataType type,
                             const py::buffer_info &info,
                             const py::buffer &buffer)
  {
    switch (type) {
    case ANARI_FLOAT32:
      return import1D<float,1>(device,ANARI_FLOAT32,info,buffer);
    case ANARI_FLOAT32_VEC2:
      return import1D<float,2>(device,ANARI_FLOAT32_VEC2,info,buffer);
    case ANARI_FLOAT32_VEC3:
      return import1D<float,3>(device,ANARI_FLOAT32_VEC3,info,buffer);
    case ANARI_FLOAT32_VEC4:
      return import1D<float,4>(device,ANARI_FLOAT32_VEC4,info,buffer);

    case ANARI_UINT32:
      return import1D<uint32_t,1>(device,ANARI_UINT32,info,buffer);
    case ANARI_UINT32_VEC2:
      return import1D<uint32_t,2>(device,ANARI_UINT32_VEC2,info,buffer);
    case ANARI_UINT32_VEC3:
      return import1D<uint32_t,3>(device,ANARI_UINT32_VEC3,info,buffer);
    case ANARI_UINT32_VEC4:
      return import1D<uint32_t,4>(device,ANARI_UINT32_VEC4,info,buffer);
    default:
    // if (info.format == "f")
    //   return importArray1D<float>(info,buffer);
      throw std::runtime_error("un-implemented array type of "+type);
    }
  }
  
  Array::Array(Device::SP device, anari::DataType type, const py::buffer &buffer)
    : Object(device)
  {
    // handle = importArray(device->handle,buffer)
    py::buffer_info info = buffer.request();
    // int numElems = asArray.shape()[0] / N;
    switch(info.ndim) {
    case 1:
      nDims = 1;
      handle = importArray1D(device->handle,type,info,buffer);
      // throw std::runtime_error("array1d not implemented");
      break;
    default:
      throw std::runtime_error("un-supported array dimensionality for anari::Array");
    }
  }
  
  Array::Array(Device::SP device,
               anari::DataType type,
               const std::vector<Object::SP> &objects)
    : Object(device)
  {
    anari::Array array
      = anari::newArray1D(device->handle,ANARI_OBJECT,objects.size());
    
    ANARIObject *mapped
      = (ANARIObject*)anariMapArray(device->handle,array);
    for (int i=0;i<objects.size();i++)
      mapped[i] = objects[i]->getHandle();
    // std::copy(objects.begin(),objects.end(),mapped);
    anariUnmapArray(device->handle,array);
    this->handle = array;
    nDims = 1;
  }
  
}
