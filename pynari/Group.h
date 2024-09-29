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

#include "pynari/common.h"
#include "pynari/Geometry.h"

namespace pynari {

  struct Material;
  struct Geometry;
  
  struct Group : public Object {
    typedef std::shared_ptr<Group> SP;
    
    Group(Device::SP device,
          const py::list &list);
    virtual ~Group() = default;
    std::string toString() const override { return "py_barn::Group"; }
    anari::Object getHandle() const override { return handle; }
    
    anari::Group handle = 0;
  };

}
