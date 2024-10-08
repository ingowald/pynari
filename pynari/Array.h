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

  struct Array : public Object {
    typedef std::shared_ptr<Array> SP;
    
    Array(Device::SP device, anari::DataType type,
          const py::buffer &buffer);
    Array(Device::SP device, anari::DataType type,
          const std::vector<Object::SP> &list);
    virtual ~Array() = default;
    std::string toString() const override { return "pynari::Array"; }

    ANARIDataType anariType() const override
    {
      switch (nDims) {
      case 1: return ANARI_ARRAY1D;
      case 2: return ANARI_ARRAY2D;
      case 3: return ANARI_ARRAY3D;
      default: throw std::runtime_error("array type not implemented");
      }
    };
    
    /*! number of DIMENSIONS of this array, NOT the 'size' */
    int          nDims  = -1;
  };

}
