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
  anari::Array importArrayT(anari::Device device,
                            ANARIDataType anariType,
                            const py::buffer_info &info,
                            const py::buffer &buffer,
                            uint64_t const nDims)
  {
    py::array_t<T> asArray = py::cast<py::array_t<T>>(buffer);
    uint64_t numScalarsInArray = 1;
    for (int i=0;i<info.ndim;i++) {
      numScalarsInArray *= (int)info.shape[i];
    }
    anari::Array handle = 0;
    if (nDims == 1) {
      handle = anari::newArray1D(device,anariType,numScalarsInArray/D);
    } else if (nDims == 2) {
      if (info.ndim == 2 && D == 1)
        handle = anari::newArray2D(device,anariType,
                                   info.shape[1],
                                   info.shape[0]);
      else if (info.ndim == 3 && info.shape[2] == D)
        handle = anari::newArray2D(device,anariType,
                                   info.shape[1],
                                   info.shape[0]);
      else
        throw std::runtime_error("cannot create array of this dim and shape!?");
    } else if (nDims == 3) {
      if (info.ndim == 3 && D == 1)
        // this is an array of scalars
        handle = anari::newArray3D(device,anariType,
                                   info.shape[2],
                                   info.shape[1],
                                   info.shape[0]);
      else if (info.ndim == 4 && info.shape[3] == D)
        handle = anari::newArray3D(device,anariType,
                                   info.shape[2],
                                   info.shape[1],
                                   info.shape[0]);
      else
        throw std::runtime_error("cannot create array of this dim and shape!?");
    } else {
      throw std::runtime_error("invalid array dimensionality");
    }
    void *ptr = anariMapArray(device,handle);
    auto buf = asArray.request();
    const T *elems = (T*)buf.ptr;
    ::memcpy(ptr,elems,numScalarsInArray*sizeof(T));
    anariUnmapArray(device,handle);
    return handle;
  }
  
  anari::Array importArray(anari::Device device,
                           anari::DataType type,
                           const py::buffer_info &info,
                           const py::buffer &buffer,
                           int const nDims)
  {
    switch (type) {
    case ANARI_FLOAT32:
      return importArrayT<float,1>(device,ANARI_FLOAT32,info,buffer,nDims);
    case ANARI_FLOAT32_VEC2:
      return importArrayT<float,2>(device,ANARI_FLOAT32_VEC2,info,buffer,nDims);
    case ANARI_FLOAT32_VEC3:
      return importArrayT<float,3>(device,ANARI_FLOAT32_VEC3,info,buffer,nDims);
    case ANARI_FLOAT32_VEC4:
      return importArrayT<float,4>(device,ANARI_FLOAT32_VEC4,info,buffer,nDims);
      
    case ANARI_UINT32:
      return importArrayT<uint32_t,1>(device,ANARI_UINT32,info,buffer,nDims);
    case ANARI_UINT32_VEC2:
      return importArrayT<uint32_t,2>(device,ANARI_UINT32_VEC2,info,buffer,nDims);
    case ANARI_UINT32_VEC3:
      return importArrayT<uint32_t,3>(device,ANARI_UINT32_VEC3,info,buffer,nDims);
    case ANARI_UINT32_VEC4:
      return importArrayT<uint32_t,4>(device,ANARI_UINT32_VEC4,info,buffer,nDims);

    case ANARI_UINT8:
      return importArrayT<uint8_t,1>(device,ANARI_UINT8,info,buffer,nDims);
    case ANARI_UINT8_VEC2:
      return importArrayT<uint8_t,2>(device,ANARI_UINT8_VEC2,info,buffer,nDims);
    case ANARI_UINT8_VEC3:
      return importArrayT<uint8_t,3>(device,ANARI_UINT8_VEC3,info,buffer,nDims);
    case ANARI_UINT8_VEC4:
      return importArrayT<uint8_t,4>(device,ANARI_UINT8_VEC4,info,buffer,nDims);

    case ANARI_INT32:
      return importArrayT<uint32_t,1>(device,ANARI_INT32,info,buffer,nDims);
    case ANARI_INT32_VEC2:
      return importArrayT<uint32_t,2>(device,ANARI_INT32_VEC2,info,buffer,nDims);
    case ANARI_INT32_VEC3:
      return importArrayT<uint32_t,3>(device,ANARI_INT32_VEC3,info,buffer,nDims);
    case ANARI_INT32_VEC4:
      return importArrayT<uint32_t,4>(device,ANARI_INT32_VEC4,info,buffer,nDims);
    default:
      throw std::runtime_error("un-implemented array type of "+std::to_string(type));
    }
  }
  
  Array::Array(Device::SP device,
               int dims,
               anari::DataType type,
               const py::buffer &buffer)
    : Object(device),
      nDims(dims),
      elementType(type),
      numObjects(0)
  {
    py::buffer_info info = buffer.request();
    this->handle = importArray(device->handle,type,info,buffer,nDims);
    PYNARI_TRACK_LEAKS(std::cout << "@pynari: created DATA-array"
                       << std::endl);
  }
  
  Array::Array(Device::SP device,
               anari::DataType type,
               const std::vector<Object::SP> &objects)
    : Object(device),
      elementType(type),
      numObjects(objects.size())
  {
    nDims = 1;
    anari::Array1D array
      = anari::newArray1D(device->handle,
# if 1
                          type,
# else
                          ANARI_OBJECT,
# endif
                          objects.size());
    this->handle = array;
    // do we need to release here?
    assertThisObjectIsValid();
    std::vector<ANARIObject> anariObjects;
    for (auto object : objects) {
      assert(object);
      ANARIObject handle = object->handle;
      anariObjects.push_back(handle);
    }
      
    ANARIObject *mapped
      = (ANARIObject*)anariMapArray(device->handle,array);
    std::copy(anariObjects.begin(),anariObjects.end(),mapped);
    anariUnmapArray(device->handle,array);
  }

  Array::~Array()
  {
    PYNARI_TRACK_LEAKS(std::cout << "#pynari: RELEASING array "
                       << (int*)this << ":" << (int*)handle << std::endl);
    anariRelease(device->handle,handle);
    handle = {};
  }
}
