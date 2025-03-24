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
    ANARIDataType anariType() const override { return ANARI_FRAME; }

    /*! trigger rendering a frame; unlike native anari that not only
        starts the frame, it also waits for it to finish */
    void render();
    uint64_t map(const std::string &channel);
    void unmap(const std::string &channel);
    
    /*! read a given frame buffer channel, and return it in a
        np::array of proper dimensions */
    py::object get(const std::string &channelName);
    
  };

}
