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
                            int &nDims)
  {
    py::array_t<T> asArray = py::cast<py::array_t<T>>(buffer);
    int numScalarsInArray = 1;
    for (int i=0;i<info.ndim;i++)
      numScalarsInArray *= (int)info.shape[i];

    anari::Array handle = 0;
    if (D == 1) {
      switch (info.ndim) {
      case 1:
        handle = anari::newArray1D
          (device,anariType,numScalarsInArray);
        nDims = 1;
        break;
      case 2:
        handle = anari::newArray2D
          (device,anariType,info.shape[0],info.shape[1]);
        nDims = 2;
        break;
      case 3:
        handle = anari::newArray3D
          (device,anariType,info.shape[0],info.shape[1],info.shape[2]);
        nDims = 3;
        break;
      default:
        PING;
        PRINT(info.ndim); 
        throw std::runtime_error("array dimensionality not implemented");
      }
    } else if (info.ndim == 1) {
      /*! array over vector type but array is one-dimensional - this
          _must_ be a one-dimensional array, but number of 'actual'
          elements is num scalars divided by vector width */
      handle = anari::newArray1D(device,anariType,numScalarsInArray/D);
      nDims = 1;
    } else {
      /*! array over a (anari-)vector type - first dim is vector width */
      if (info.shape[0] != D)
        throw std::runtime_error("N-dimensional array, but shape[0] doesn't match the vector width of underlying ANARI data type (ie, a 4x3 array over a ANARI_FLOAT32_VEC4 data should have shape (4,5,3)");
      switch (info.ndim) {
      case 2:
        handle = anari::newArray1D
          (device,anariType,info.shape[1]);
        nDims = 1;
        break;
      case 3:
        handle = anari::newArray2D
          (device,anariType,info.shape[1],info.shape[2]);
      nDims = 2;
        break;
      case 4:
        handle = anari::newArray3D
          (device,anariType,info.shape[1],info.shape[2],info.shape[3]);
        nDims = 3;
        break;
      default:
        PING;
        PRINT(info.ndim);
        throw std::runtime_error("array dimensionality not implemented");
      }
    }
    void *ptr = anariMapArray(device,handle);
    auto buf = asArray.request();
    const T *elems = (T*)buf.ptr;
    ::memcpy(ptr,elems,numScalarsInArray*sizeof(T));
    anariUnmapArray(device,handle);
    return handle;
  }
  
  // template<typename T, int D>
  // anari::Array import1D(anari::Device device,
  //                       ANARIDataType anariType,
  //                       const py::buffer_info &info,
  //                       const py::buffer &buffer)
  // {
  //   py::array_t<T> asArray = py::cast<py::array_t<T>>(buffer);
  //   int numElems = asArray.shape()[0] / D;
  //   anari::Array handle = anari::newArray1D(device,anariType,numElems);
  //   void *ptr = anariMapArray(device,handle);
  //   auto buf = asArray.request();
  //   const T *elems = (T*)buf.ptr;
  //   ::memcpy(ptr,elems,numElems*sizeof(T)*D);
  //   anariUnmapArray(device,handle);
  //   return handle;
  // }

  // template<typename T, int D>
  // anari::Array import3D(anari::Device device,
  //                       ANARIDataType anariType,
  //                       const py::buffer_info &info,
  //                       const py::buffer &buffer)
  // {
  //   py::array_t<T> asArray = py::cast<py::array_t<T>>(buffer);
  //   int nx = asArray.shape()[0] / D;
  //   int ny = asArray.shape()[1];
  //   int nz = asArray.shape()[2];
  //     anari::Array handle = anari::newArray3D(device,anariType,nx,ny,nz);
  //   void *ptr = anariMapArray(device,handle);
  //   auto buf = asArray.request();
  //   const T *elems = (T*)buf.ptr;
  //   ::memcpy(ptr,elems,nx*ny*nz*sizeof(T)*D);
  //   anariUnmapArray(device,handle);
  //   return handle;
  // }

  // anari::Array importArray1D(anari::Device device,
  //                            anari::DataType type,
  //                            const py::buffer_info &info,
  //                            const py::buffer &buffer)
  // {
  //   switch (type) {
  //   case ANARI_FLOAT32:
  //     return import1D<float,1>(device,ANARI_FLOAT32,info,buffer);
  //   case ANARI_FLOAT32_VEC2:
  //     return import1D<float,2>(device,ANARI_FLOAT32_VEC2,info,buffer);
  //   case ANARI_FLOAT32_VEC3:
  //     return import1D<float,3>(device,ANARI_FLOAT32_VEC3,info,buffer);
  //   case ANARI_FLOAT32_VEC4:
  //     return import1D<float,4>(device,ANARI_FLOAT32_VEC4,info,buffer);

  //   case ANARI_UINT32:
  //     return import1D<uint32_t,1>(device,ANARI_UINT32,info,buffer);
  //   case ANARI_UINT32_VEC2:
  //     return import1D<uint32_t,2>(device,ANARI_UINT32_VEC2,info,buffer);
  //   case ANARI_UINT32_VEC3:
  //     return import1D<uint32_t,3>(device,ANARI_UINT32_VEC3,info,buffer);
  //   case ANARI_UINT32_VEC4:
  //     return import1D<uint32_t,4>(device,ANARI_UINT32_VEC4,info,buffer);
  //   default:
  //   // if (info.format == "f")
  //   //   return importArray1D<float>(info,buffer);
  //     throw std::runtime_error("un-implemented array type of "+type);
  //   }
  // }
  

  anari::Array importArray(anari::Device device,
                             anari::DataType type,
                             const py::buffer_info &info,
                             const py::buffer &buffer,
                             int &nDims)
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
    default:
    // if (info.format == "f")
    //   return importArray<float>(info,buffer);
      throw std::runtime_error("un-implemented array type of "+std::to_string(type));
    }
  }
  


  // anari::Array importArray3D(anari::Device device,
  //                            anari::DataType type,
  //                            const py::buffer_info &info,
  //                            const py::buffer &buffer)
  // {
  //   switch (type) {
  //   case ANARI_FLOAT32:
  //     return import3D<float,1>(device,ANARI_FLOAT32,info,buffer);
  //   case ANARI_FLOAT32_VEC2:
  //     return import3D<float,2>(device,ANARI_FLOAT32_VEC2,info,buffer);
  //   case ANARI_FLOAT32_VEC3:
  //     return import3D<float,3>(device,ANARI_FLOAT32_VEC3,info,buffer);
  //   case ANARI_FLOAT32_VEC4:
  //     return import3D<float,4>(device,ANARI_FLOAT32_VEC4,info,buffer);

  //   case ANARI_UINT32:
  //     return import3D<uint32_t,1>(device,ANARI_UINT32,info,buffer);
  //   case ANARI_UINT32_VEC2:
  //     return import3D<uint32_t,2>(device,ANARI_UINT32_VEC2,info,buffer);
  //   case ANARI_UINT32_VEC3:
  //     return import3D<uint32_t,3>(device,ANARI_UINT32_VEC3,info,buffer);
  //   case ANARI_UINT32_VEC4:
  //     return import3D<uint32_t,4>(device,ANARI_UINT32_VEC4,info,buffer);
  //   default:
  //   // if (info.format == "f")
  //   //   return importArray3D<float>(info,buffer);
  //     throw std::runtime_error("un-implemented array type of "+type);
  //   }
  // }
  
  Array::Array(Device::SP device,
               anari::DataType type,
               const py::buffer &buffer)
    : Object(device)
  {
    py::buffer_info info = buffer.request();
    this->handle = importArray(device->handle,type,info,buffer,nDims);
  }
  
  Array::Array(Device::SP device,
               anari::DataType type,
               const std::vector<Object::SP> &objects)
    : Object(device)
  {
    anari::Array1D array
      = anari::newArray1D(device->handle,ANARI_OBJECT,objects.size());
    
    ANARIObject *mapped
      = (ANARIObject*)anariMapArray(device->handle,array);
    for (int i=0;i<objects.size();i++)
      mapped[i] = objects[i]->handle;
    anariUnmapArray(device->handle,array);
    this->handle = array;
    nDims = 1;
  }
  
}
