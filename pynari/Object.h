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

  struct Object : public std::enable_shared_from_this<Object> {
    Object(Device::SP device);
    
    virtual ~Object() = default;
    virtual std::string toString() const { return "<Object>"; }

    virtual anari::Object getHandle() const = 0;
    
    void commit();

    void set_float(const std::string &name,
                   float f);
    void set_float3_tuple(const char *name,
                          const std::tuple<float,float,float> &v);
    void set_float3_list(const char *name,
                         const std::vector<float> &v);
    void set_float3_numpy(const char *name,
                          const py::buffer &buffer);

    const Device::SP device;
  };
  
}
