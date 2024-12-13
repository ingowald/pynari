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

#include "pynari/common.h"
#include <set>

namespace pynari {
  struct Object;

  /*! python-wrapper object for an ANARIDevice - not that in pynari
      all functionality of a device lives in the pynari::Context
      object; this only handles the lifetime (so all objects created
      from this device can hold a std::shared_ptr to it and thus make
      sure the device doesn't die before the obejcts */
  struct Device {
    typedef std::shared_ptr<Device> SP;
    Device(anari::Device handle) : handle(handle)
    {}
    virtual ~Device() {}

    /*! force-releases this device, and all objects created from
        it. This will force-release all respective objects on the
        anari side even if some of the python wrapper objects are
        still around. It is of course no longer valid to use any of
        these python wrapper objects any more after their owning
        device has been deleted, but at least it gives he python user
        an easy way of deleting all anari objects (in that gvien
        device) without having to track and destroy all the obejcts he
        has created */
    void release();

    std::set<Object*> listOfAllObjectsCreatedOnThisDevice;
    
    anari::Device handle = 0;
  };

}
