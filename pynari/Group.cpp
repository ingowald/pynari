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

#include "pynari/Model.h"
#include "pynari/Group.h"
#include "pynari/Geometry.h"

namespace pynari {

  Group::Group(Device::SP device,
               const py::list &list)
    : Object(device)
  {
    handle = anari::newObject<anari::Group>(device->handle);
    // std::vector<BNGeom> geoms;
    // std::vector<BNVolume> volumes;
    // for (auto item : list) {
    //   Geometry::SP geom = item.cast<Geometry::SP>();
    //   if (geom) {
    //     geoms.push_back(geom->handle);
    //   }
    // }
    // handle = bnGroupCreate(model->handle,slot,
    //                        geoms.data(),geoms.size(),
    //                        volumes.data(),volumes.size());
    // bnGroupBuild(handle);
    PING;
  }

  
}
