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

#pragma once

#include "pynari/common.h"
#include <set>
#include <memory>

#define PYNARI_TRACK_LEAKS(a) /* nothing */

namespace pynari {
  struct Object;
  struct Context;
  
  /*! python-wrapper object for an ANARIDevice - not that in pynari
      all functionality of a device lives in the pynari::Context
      object; this only handles the lifetime (so all objects created
      from this device can hold a std::shared_ptr to it and thus make
      sure the device doesn't die before the obejcts */
  struct Device : public std::enable_shared_from_this<Device> {
    typedef std::shared_ptr<Device> SP;
    
    Device(anari::Device handle, Context *context);
    virtual ~Device();

    /*! this gets called if - and only if - the python app calls
        anariDevice.release(). If so this will go over all the pynari
        object still alive at this moment, and force-release their
        anari handles (the pynari wrapper objects themselves are
        refcoutned by python and may thus stay alive for a while
        longer) */
    void releaseFromApp();

    std::set<Object*> listOfAllObjectsCreatedOnThisDevice;
    
    anari::Device handle = 0;
    Context *const context;
  };

}
