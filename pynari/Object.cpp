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

  std::string to_string(anari::DataType type)
  {
    switch(type) {
    case ANARI_DATA_TYPE:      return "ANARI_DATA_TYPE";
    case ANARI_STRING:         return "ANARI_STRING";
    case ANARI_OBJECT:         return "ANARI_OBJECT";
    case ANARI_ARRAY:          return "ANARI_ARRAY";
    case ANARI_ARRAY1D:        return "ANARI_ARRAY1D";
    case ANARI_ARRAY2D:        return "ANARI_ARRAY2D";
    case ANARI_ARRAY3D:        return "ANARI_ARRAY3D";
    case ANARI_WORLD:          return "ANARI_WORLD";
    case ANARI_RENDERER:       return "ANARI_RENDERER";
    case ANARI_MATERIAL:       return "ANARI_MATERIAL";
    case ANARI_GEOMETRY:       return "ANARI_GEOMETRY";
    case ANARI_SURFACE:        return "ANARI_SURFACE";
    case ANARI_FLOAT32:        return "ANARI_FLOAT32";
    case ANARI_FLOAT32_VEC2:   return "ANARI_FLOAT32_VEC2";
    case ANARI_FLOAT32_VEC3:   return "ANARI_FLOAT32_VEC3";
    case ANARI_FLOAT32_VEC4:   return "ANARI_FLOAT32_VEC4";
    case ANARI_UINT32:         return "ANARI_UINT32";
    case ANARI_UINT32_VEC2:    return "ANARI_UINT32_VEC2";
    case ANARI_UINT32_VEC3:    return "ANARI_UINT32_VEC3";
    case ANARI_UINT32_VEC4:    return "ANARI_UINT32_VEC4";
    default:
      throw std::runtime_error
        ("unsupported type "
         +std::to_string((int)type)
         +" in pyName::toString(ANARIDataType)");
    }
  }
  
  Object::Object(Device::SP device)
    : device(device)
  {}
  
  void Object::assignTo(Object::SP object,
                        anari::DataType intendedType,
                        const std::string &name)
  { throw std::runtime_error("Object::assignTo not implemented for object "
                             +object->toString()); };
  std::string Object::toString() const { return "<Object>"; }

  void Object::commit() { anariCommitParameters(device->handle,getHandle()); }

  void Object::setArray_list(const char *name,
                             int type, 
                             const py::list &list)
  { 
    std::vector<ANARIObject> objects;
    for (auto item : list) {
      Object::SP object = item.cast<Object::SP>();
      assert(object);
      ANARIObject handle = object->getHandle();
      objects.push_back(handle);
    }
    anari::Array array
      = anari::newArray1D(device->handle,ANARI_OBJECT,objects.size());
      
    ANARIObject *mapped
      = (ANARIObject*)anariMapArray(device->handle,array);
    std::copy(objects.begin(),objects.end(),mapped);
    anariUnmapArray(device->handle,array);
      
    anari::setParameter(device->handle,getHandle(),name,array);
  }
  
  void Object::set(const char *name, int type, const Object::SP &object)
  {
    object->assignTo(shared_from_this(),type,name);
  }
    
  void Object::set_float(const char *name,
                         int type,
                         float v)
  {
    switch(type) {
    case ANARI_FLOAT32:
      return anari::setParameter(device->handle,getHandle(),name,(float)v);
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)
         +" unsupported type "+to_string((anari::DataType)type));
    }
  }
    
  void Object::set_float2(const char *name,
                          int type, 
                          const std::tuple<float,float> &v)
  { PING; }
    
  void Object::set_float3(const char *name,
                          int type, 
                          const std::tuple<float,float,float> &v)
  {
    switch(type) {
    case ANARI_FLOAT32_VEC3:
      return anari::setParameter(device->handle,getHandle(),name,
                                 math::float3(std::get<0>(v),
                                              std::get<1>(v),
                                              std::get<2>(v)));
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)
         +" unsupported type "+to_string((anari::DataType)type));
    }
  }
    
  void Object::set_float4(const char *name,
                          int type, 
                          const std::tuple<float,float,float,float> &v)
  { switch(type) {
    case ANARI_FLOAT32_VEC4:
      return anari::setParameter(device->handle,getHandle(),name,
                                 math::float4(std::get<0>(v),
                                              std::get<1>(v),
                                              std::get<2>(v),
                                              std::get<3>(v)));
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)+" unsupported type "+to_string((anari::DataType)type));
    }
  }
    
  void Object::set_float_vec(const char *name,
                             int type, 
                             const std::vector<float> &v)
  { PING; }

  void Object::set_uint(const char *name,
                        int type,
                        uint v)
  {
    switch(type) {
    case ANARI_DATA_TYPE:
      return anari::setParameter(device->handle,getHandle(),name,
                                 (anari::DataType)v);
    case ANARI_UINT32:
      return anari::setParameter(device->handle,getHandle(),name,
                                 (uint)v);
    case ANARI_FLOAT32:
      return anari::setParameter(device->handle,getHandle(),name,
                                 (float)v);
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)
         +" unsupported type "+to_string((anari::DataType)type));
    }
  }
    
  void Object::set_uint2(const char *name,
                         int type, 
                         const std::tuple<uint,uint> &v)
  {
    switch(type) {
    case ANARI_UINT32_VEC2:
      return anari::setParameter(device->handle,getHandle(),name,
                                 math::uint2(std::get<0>(v),
                                             std::get<1>(v)));
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)
         +" unsupported type "+to_string((anari::DataType)type));
    }
  }
    
  void Object::set_uint3(const char *name,
                         int type, 
                         const std::tuple<uint,uint,uint> &v)
  { 
    switch(type) {
    case ANARI_FLOAT32_VEC3:
      return anari::setParameter(device->handle,getHandle(),name,
                                 math::float3(std::get<0>(v),
                                              std::get<1>(v),
                                              std::get<2>(v)));
    case ANARI_UINT32_VEC3:
      return anari::setParameter(device->handle,getHandle(),name,
                                 math::uint3(std::get<0>(v),
                                             std::get<1>(v),
                                             std::get<2>(v)));
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)
         +" unsupported type "+to_string((anari::DataType)type));
    }
  }
    
  void Object::set_uint4(const char *name,
                         int type, 
                         const std::tuple<uint,uint,uint,uint> &v)
  { switch(type) {
    case ANARI_UINT32_VEC4:
      return anari::setParameter(device->handle,getHandle(),name,
                                 math::uint4(std::get<0>(v),
                                             std::get<1>(v),
                                             std::get<2>(v),
                                             std::get<3>(v)));
    case ANARI_FLOAT32_VEC4:
      return anari::setParameter(device->handle,getHandle(),name,
                                 math::float4(std::get<0>(v),
                                              std::get<1>(v),
                                              std::get<2>(v),
                                              std::get<3>(v)));
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)+" unsupported type "+to_string((anari::DataType)type));
    }
  }
    
  void Object::set_uint_vec(const char *name,
                            int type, 
                            const std::vector<uint> &v)
  { PING; }

  
}

