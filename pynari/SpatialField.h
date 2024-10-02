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

  struct SpatialField : public Object {
    typedef std::shared_ptr<SpatialField> SP;
    
    SpatialField(Device::SP device,
                 const std::string &type);
    virtual ~SpatialField() = default;
    std::string toString() const override
    { return "pynari::SpatialField<"+type+">"; }
    
    void assignTo(Object::SP object,
                  anari::DataType intendedType,
                  const std::string &name) override
    {
      anariSetParameter(device->handle,object->getHandle(),
                        name.c_str(),ANARI_SPATIAL_FIELD,&this->handle);
    }
    
    anari::Object getHandle() const override { return handle; }

    anari::SpatialField handle = 0;
    const std::string type;
  };

}
