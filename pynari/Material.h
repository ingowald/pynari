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

  struct Material : public Object {
    typedef std::shared_ptr<Material> SP;
    
    Material(Device::SP device,
             // anari::World model,
             const std::string type);
    virtual ~Material() = default;
    std::string toString() const override { return "py_barn::Material"; }

    // void set3f(const std::string &name, float x, float y, float z)
    // { anari::setParameter(handle,name.c_str(),math::float3(x,y,z)); }
    // void set3fv(const std::string &name, const std::tuple<float,float,float> &v)
    // { anari::setParameter(handle,name.c_str(),std::get<0>(v),std::get<1>(v),std::get<2>(v)); }
    // void set1f(const std::string &name, float &f)
    // { bnSet1f(handle,name.c_str(),f); }

    const std::string type;
    anari::Material handle = 0;
    // anari::World    model  = 0;
  };

}
