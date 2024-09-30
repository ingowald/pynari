// ======================================================================== //
// Copyright 2024-2024 Ingo Wald                                            //
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

#include "pynari/Object.h"

namespace pynari {

  Object::Object(Device::SP device)
    : device(device)
  {}
  
  std::string Object::toString() const { return "<Object>"; }

  void Object::commit() { anariCommitParameters(device->handle,getHandle()); }

  void Object::set(const std::string &name, Object::SP object)
  { object->assignTo(shared_from_this(),name); }
  //anari::setParameter(device->handle,getHandle(),object->getHandle()); }

  ANARIDataType parseType(const std::string &typeName)
  {
    if (typeName == "ANARI_UFIXED8_RGBA_SRGB")
      return ANARI_UFIXED8_RGBA_SRGB;
    if (typeName == "ANARI_FLOAT32_VEC4")
      return ANARI_FLOAT32_VEC4;
    if (typeName == "ANARI_UFIXED8_VEC4")
      return ANARI_UFIXED8_VEC4;
    throw std::runtime_error("un-implmemented anari type "+typeName);
  }
  
  void Object::set_type(const std::string &name, const std::string &typeName)
  {
    ANARIDataType type = parseType(typeName);
    anariSetParameter(device->handle,getHandle(),name.c_str(),
                      ANARI_DATA_TYPE,&type);
  }
  
  void Object::set_float(const std::string &name, float x)
  { anari::setParameter(device->handle,getHandle(),name.c_str(),x); }
    
  void Object::set_float2_tuple(const char *name,
                                const std::tuple<float,float> &v)
  { anari::setParameter(device->handle,getHandle(),name,make_float2(v)); }

  void Object::set_float2_list(const char *name,
                               const std::vector<float> &v)
  { anari::setParameter(device->handle,getHandle(),name,make_float2(v)); }
  
  void Object::set_float3_tuple(const char *name,
                                const std::tuple<float,float,float> &v)
  { anari::setParameter(device->handle,getHandle(),name,make_float3(v)); }

  void Object::set_float3_list(const char *name,
                               const std::vector<float> &v)
  { anari::setParameter(device->handle,getHandle(),name,make_float3(v)); }
  
  void Object::set_float4_tuple(const char *name,
                                const std::tuple<float,float,float,float> &v)
  { anari::setParameter(device->handle,getHandle(),name,make_float4(v)); }

  void Object::set_float4_list(const char *name,
                               const std::vector<float> &v)
  { anari::setParameter(device->handle,getHandle(),name,make_float4(v)); }
  

  void Object::set_uint(const std::string &name, uint x)
  { anari::setParameter(device->handle,getHandle(),name.c_str(),x); }
    
  void Object::set_uint2_tuple(const char *name,
                                const std::tuple<uint,uint> &v)
  { anari::setParameter(device->handle,getHandle(),name,make_uint2(v)); }

  void Object::set_uint2_list(const char *name,
                               const std::vector<uint> &v)
  { anari::setParameter(device->handle,getHandle(),name,make_uint2(v)); }
  
  void Object::set_uint3_tuple(const char *name,
                                const std::tuple<uint,uint,uint> &v)
  { anari::setParameter(device->handle,getHandle(),name,make_uint3(v)); }

  void Object::set_uint3_list(const char *name,
                               const std::vector<uint> &v)
  { anari::setParameter(device->handle,getHandle(),name,make_uint3(v)); }
  
  void Object::set_uint4_tuple(const char *name,
                                const std::tuple<uint,uint,uint,uint> &v)
  { anari::setParameter(device->handle,getHandle(),name,make_uint4(v)); }

  void Object::set_uint4_list(const char *name,
                               const std::vector<uint> &v)
  { anari::setParameter(device->handle,getHandle(),name,make_uint4(v)); }
  

  void Object::set_list(const std::string &name,
                        const py::list &list)
  {
    std::vector<ANARIObject> objects;
    for (auto item : list) {
      Object::SP object = item.cast<Object::SP>();
      assert(object);
      ANARIObject handle = object->getHandle();
      anariRetain(device->handle,handle);
      objects.push_back(handle);
    }
    anari::Array array = anari::newArray1D(device->handle,ANARI_OBJECT,objects.size());
    void *ptr = anariMapArray(device->handle,array);
    memcpy(ptr,objects.data(),objects.size()*sizeof(objects[0]));
    anariUnmapArray(device->handle,array);


    anari::setParameter(device->handle,getHandle(),name.c_str(),array);
    
    // OWLGroup handle = anariNewArray1D(device->handle,
    //                                               geoms.size(),
    //                                               geoms.data());
    // assert(handle);
    // return std::make_shared<Group>(handle);
    // PING;
  }
  
    
  
  // void Object::set_float3_numpy(const char *name,
  //                               const py::buffer &buffer)
  // { anari::setParameter(device->handle,getHandle(),name,make_float3(buffer)); }
  
}

