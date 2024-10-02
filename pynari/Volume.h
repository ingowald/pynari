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

  struct Volume : public Object {
    typedef std::shared_ptr<Volume> SP;
    
    Volume(Device::SP device,
                 const std::string &type);
    virtual ~Volume() = default;
    std::string toString() const override
    { return "pynari::Volume<"+type+">"; }
    
    ANARIDataType anariType() const override { return ANARI_VOLUME; }

    const std::string type;
  };

  inline Volume::Volume(Device::SP device,
                 const std::string &type)
    : Object(device),
      type(type)
  {
    handle = anari::newObject<anari::Volume>(device->handle,type.c_str());
  }

}
