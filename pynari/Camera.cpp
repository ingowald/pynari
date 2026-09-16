// ======================================================================== //
// Copyright 2024-2026 Ingo Wald                                            //
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

#include "pynari/Camera.h"

namespace pynari {

  Camera::Camera(Device::SP device,
                 const std::string &type)
    : Object(device),
      type(type)
  {
    handle = anari::newObject<anari::Camera>(device->handle,type.c_str());
  }

  Camera::~Camera()
  {
    /* do NOT anariRelease() our handle here, for two reasons: First,
       this should have been released already in 'releaseFromApp()',
       when the app called pynari_object.release() (as it _should_
       have done). Second, even if the app 'forgot' to do that we want
       this to be detected and handled cleanly by device and parent
       object:: code, so let's not interfere with this here. */
    if (handle)
      // this should always be called in the specific object's constructor, not here.
      fallbackDestructAndWarn(toString());
  }

  std::string Camera::toString() const
  { return "pynari::Camera<"+type+">"; }
  
}


