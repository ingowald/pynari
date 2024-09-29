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

#pragma once

#include "pynari/Object.h"

namespace pynari {

  struct Model;
  
  struct Camera : public Object {
    typedef std::shared_ptr<Camera> SP;
    
    Camera(Device::SP device,
           const std::string &type);
    virtual ~Camera() = default;
    anari::Object getHandle() const override { return handle; }
    std::string toString() const override { return "py_barn::Camera<"+type+">"; }

    // void set_float(const std::string &name, float x) override { PING; PRINT(x); }
    // void set3f(const std::string &name, float x, float y, float z)
    // { bnSet3f(handle,name.c_str(),x,y,z); }
    // void set3fv(const std::string &name, const std::tuple<float,float,float> &v)
    // { bnSet3f(handle,name.c_str(),std::get<0>(v),std::get<1>(v),std::get<2>(v)); }
    // void set1f(const std::string &name, float &f)
    // { bnSet1f(handle,name.c_str(),f); }

    // void from_at_up(const std::tuple<float,float,float> &_from,
    //                 const std::tuple<float,float,float> &_at,
    //                 const std::tuple<float,float,float> &_up,
    //                 float fovy=60.f,
    //                 float aspect=1.f);
    
    
    const std::string type;
    anari::Camera handle = 0;
  };

}
