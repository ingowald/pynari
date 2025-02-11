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
    case ANARI_INT32:          return "ANARI_INT32";
    case ANARI_INT32_VEC2:     return "ANARI_INT32_VEC2";
    case ANARI_INT32_VEC3:     return "ANARI_INT32_VEC3";
    case ANARI_INT32_VEC4:     return "ANARI_INT32_VEC4";
    default:
      throw std::runtime_error
        ("unsupported type "
         +std::to_string((int)type)
         +" in pyName::toString(ANARIDataType)");
    }
  }
  
  Object::Object(Device::SP device)
    : device(device)
  {
    device->listOfAllObjectsCreatedOnThisDevice.insert(this);
  }

  Object::~Object()
  {
    assert(this);
    release();
  }

  void Object::assertThisObjectIsValid()
  {
    assert(this->handle);
    assert(this->device);
  }
  
  void Object::commit()
  {
    assertThisObjectIsValid();
    anariCommitParameters(device->handle,this->handle);
  }

  void Object::release()
  {
    if (!handle) return;
    if (!device->handle) return;

    device->listOfAllObjectsCreatedOnThisDevice.erase(this);
    
    anari::release(device->handle,handle);
    handle = {};
    device = nullptr;
  }

  void Object::setArray_list(const char *name,
                             int type, 
                             const py::list &list)
  { 
    assertThisObjectIsValid();
    std::vector<ANARIObject> objects;
    for (auto item : list) {
      Object::SP object = item.cast<Object::SP>();
      assert(object);
      ANARIObject handle = object->handle;
      objects.push_back(handle);
    }
    anari::Array array
      = anari::newArray1D(device->handle,ANARI_OBJECT,objects.size());
      
    ANARIObject *mapped
      = (ANARIObject*)anariMapArray(device->handle,array);
    std::copy(objects.begin(),objects.end(),mapped);
    anariUnmapArray(device->handle,array);
      
    anari::setParameter(device->handle,this->handle,name,array);
  }
  
  void Object::set_object(const char *name, int type, const Object::SP &object)
  {
    assertThisObjectIsValid();
    /* TODO: do some checking if 'type' matches anariType() */
    anari::setParameter(device->handle,this->handle,
                        name,
                        object->anariType(),
                        &object->handle);
  }
    
  void Object::set_float(const char *name,
                         int type,
                         float v)
  {
    assertThisObjectIsValid();
    switch(type) {
    case ANARI_FLOAT32:
      return anari::setParameter(device->handle,handle,name,(float)v);
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)
         +" unsupported type "+to_string((anari::DataType)type));
    }
  }
    
  void Object::set_float2(const char *name,
                          int type, 
                          const std::tuple<float,float> &v)
  { 
    assertThisObjectIsValid();
    switch(type) {
    case ANARI_FLOAT32:
      return anari::setParameter(device->handle,handle,name,
                                 math::float2(std::get<0>(v),
                                              std::get<1>(v)));
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)
         +" unsupported type "+to_string((anari::DataType)type));
    }
  }
    
  void Object::set_float3(const char *name,
                          int type, 
                          const std::tuple<float,float,float> &v)
  {
    assertThisObjectIsValid();
    switch(type) {
    case ANARI_FLOAT32_VEC3:
      return anari::setParameter(device->handle,this->handle,name,
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
  {
    assertThisObjectIsValid();
    switch(type) {
    case ANARI_FLOAT32_VEC4:
      return anari::setParameter(device->handle,this->handle,name,
                                 math::float4(std::get<0>(v),
                                              std::get<1>(v),
                                              std::get<2>(v),
                                              std::get<3>(v)));
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)+" unsupported type "
         +to_string((anari::DataType)type));
    }
  }
    
  void Object::set_uint_vec(const char *name,
                            int type, 
                            const std::vector<uint> &v)
  {
    assertThisObjectIsValid();
    switch(type) {
    case ANARI_UINT32_VEC3:
      return anari::setParameter(device->handle,this->handle,name,
                                 math::uint3(v[0],v[1],v[2]));
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)
         +" unsupported type "+to_string((anari::DataType)type));
    }
  }

  void Object::set_float_vec(const char *name,
                             int type, 
                             const std::vector<float> &v)
  {
    assertThisObjectIsValid();
    switch(type) {
    case ANARI_FLOAT32_VEC3:
      return anari::setParameter(device->handle,this->handle,name,
                                 math::float3(v[0],v[1],v[2]));
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)
         +" unsupported type "+to_string((anari::DataType)type));
    }
  }

  void Object::set_string(const char *name, int type,
                          const std::string &stringValue)
  {
    assert(type == ANARI_STRING);
    anari::setParameter(device->handle,this->handle,name,stringValue.c_str());
  }

  void Object::set_uint(const char *name,
                        int type,
                        uint v)
  {
    assertThisObjectIsValid();
    switch(type) {
    case ANARI_DATA_TYPE:
      return anari::setParameter(device->handle,this->handle,name,
                                 (anari::DataType)v);
    case ANARI_INT32:
      return anari::setParameter(device->handle,this->handle,name,
                                 (int)v);
    case ANARI_UINT32:
      return anari::setParameter(device->handle,this->handle,name,
                                 (uint)v);
    case ANARI_FLOAT32:
      return anari::setParameter(device->handle,this->handle,name,
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
    assertThisObjectIsValid();
    switch(type) {
    case ANARI_UINT32_VEC2:
      return anari::setParameter(device->handle,this->handle,name,
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
    assertThisObjectIsValid();
    switch(type) {
    case ANARI_FLOAT32_VEC3:
      return anari::setParameter(device->handle,this->handle,name,
                                 math::float3((float)std::get<0>(v),
                                              (float)std::get<1>(v),
                                              (float)std::get<2>(v)));
    case ANARI_UINT32_VEC3:
      return anari::setParameter(device->handle,this->handle,name,
                                 math::uint3((uint32_t)std::get<0>(v),
                                             (uint32_t)std::get<1>(v),
                                             (uint32_t)std::get<2>(v)));
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)
         +" unsupported type "+to_string((anari::DataType)type));
    }
  }
    
  void Object::set_uint4(const char *name,
                         int type, 
                         const std::tuple<uint,uint,uint,uint> &v)
  {
    assertThisObjectIsValid();
    switch(type) {
    case ANARI_UINT32_VEC4:
      return anari::setParameter(device->handle,this->handle,name,
                                 math::uint4((uint32_t)std::get<0>(v),
                                             (uint32_t)std::get<1>(v),
                                             (uint32_t)std::get<2>(v),
                                             (uint32_t)std::get<3>(v)));
    case ANARI_FLOAT32_VEC4:
      return anari::setParameter(device->handle,this->handle,name,
                                 math::float4((float)std::get<0>(v),
                                              (float)std::get<1>(v),
                                              (float)std::get<2>(v),
                                              (float)std::get<3>(v)));
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)+" unsupported type "+to_string((anari::DataType)type));
    }
  }
    
  
}

