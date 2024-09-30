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

#pragma once

#include "pynari/Device.h"

namespace pynari {

  std::string to_string(anari::DataType type);
  
  struct Object : public std::enable_shared_from_this<Object> {
    typedef std::shared_ptr<Object> SP;
    
    Object(Device::SP device);
    
    virtual ~Object() = default;
    virtual std::string toString() const;

    virtual anari::Object getHandle() const = 0;

    virtual void assignTo(Object::SP object,
                          anari::DataType intendedType,
                          const std::string &name);
    
    void commit();

    void set(const char *name, int type, const Object::SP &object)
    {
      object->assignTo(shared_from_this(),type,name);
    }
    
    void set_type(const char *name, int type, int value)
    { PING; }

    // void set_list(const char *name,
    //               int type, 
    //               const py::list &list)
    // { 
    //   switch(type) {
    //   case ANARI_FLOAT32:
    //     return setScalar<float>(name,type,list);
    //   default:
    //     throw std::runtime_error
    //       ("unsupported type "+to_string((anari::DataType)type));
    //   }
    // }
    
    void setArray_list(const char *name,
                       int type, 
                       const py::list &list);
    
    void set_float(const char *name,
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
    
    void set_float2(const char *name,
                    int type, 
                    const std::tuple<float,float> &v)
    { PING; }
    
    void set_float3(const char *name,
                    int type, 
                    const std::tuple<float,float,float> &v)
    { switch(type) {
      case ANARI_FLOAT32_VEC3:
        return anari::setParameter(device->handle,getHandle(),name,
                                   math::float3(std::get<0>(v),
                                                std::get<1>(v),
                                                std::get<2>(v)));
      default:
        throw std::runtime_error
          (std::string(__PRETTY_FUNCTION__)+" unsupported type "+to_string((anari::DataType)type));
      }
    }
    
    void set_float4(const char *name,
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
    
    void set_float_vec(const char *name,
                    int type, 
                    const std::vector<float> &v)
    { PING; }

    void set_uint(const char *name,
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
    
    void set_uint2(const char *name,
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
    
    void set_uint3(const char *name,
                    int type, 
                    const std::tuple<uint,uint,uint> &v)
    { PING; }
    
    void set_uint4(const char *name,
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
    
    void set_uint_vec(const char *name,
                    int type, 
                    const std::vector<uint> &v)
    { PING; }

    // void set_uint(const char *name,
    //               int type, 
    //                uint f);
    // void set_uint2_tuple(const char *name,int type, 
    //                       const std::tuple<uint,uint> &v);
    // void set_uint2_list(const char *name,int type, 
    //                      const std::vector<uint> &v);
    // void set_uint3_tuple(const char *name,int type, 
    //                       const std::tuple<uint,uint,uint> &v);
    // void set_uint3_list(const char *name,int type, 
    //                      const std::vector<uint> &v);
    // void set_uint4_tuple(const char *name,int type, 
    //                       const std::tuple<uint,uint,uint,uint> &v);
    // void set_uint4_list(const char *name,int type, 
    //                      const std::vector<uint> &v);

    // void set_float2_tuple(const char *name,int type, 
    //                       const std::tuple<float,float> &v);
    // void set_float2_list(const char *name,int type, 
    //                      const std::vector<float> &v);
    // void set_float3_tuple(const char *name, int type, 
    //                       const std::tuple<float,float,float> &v);
    // void set_float3_list(const char *name,int type, 
    //                      const std::vector<float> &v);
    // void set_float4_tuple(const char *name,int type, 
    //                       const std::tuple<float,float,float,float> &v);
    // void set_float4_list(const char *name,int type, 
    //                      const std::vector<float> &v);
    // void set_float3_numpy(const char *name,
    //                       const py::buffer &buffer);

    const Device::SP device;
  };
  
}
