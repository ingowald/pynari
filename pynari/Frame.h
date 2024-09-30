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

  struct Material;
  struct Geometry;
  struct Group;
  struct Camera;
  struct FrameBuffer;
  struct Data;
  
  struct Frame : public Object {
    typedef std::shared_ptr<Frame> SP;
    
    Frame(Device::SP device);
    virtual ~Frame() = default;
    std::string toString() const override { return "pynari::Frame"; }
    anari::Object getHandle() const override { return handle; }

    void render();
    py::object color();

    anari::Frame handle = 0;
  };

}
