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
  struct Array;
  
  /*! wrapper object for an ANARIDevice - note that the 'device'
      created by the python app is actually a 'Context' - not this
      device: the context is what a pynari app talks to (through
      python), but it may go out of scope even while other obejcts are
      still alive. This 'Device' class is what stay alive until every
      other object (including said context) has gone out of scope. */
  struct Device : public std::enable_shared_from_this<Device> {
    typedef std::shared_ptr<Device> SP;
    
    Device(anari::Device handle);
    virtual ~Device();

    /*! this gets called if - and only if - the python app calls
        anariDevice.release(). If so this will go over all the pynari
        object still alive at this moment, and force-release their
        anari handles (the pynari wrapper objects themselves are
        refcoutned by python and may thus stay alive for a while
        longer) */
    void releaseFromApp();

    std::shared_ptr<Array> newArray(int type, const py::buffer &buffer);
    std::shared_ptr<Array> newArray1D(int type, const py::buffer &buffer);
    std::shared_ptr<Array> newArray2D(int type, const py::buffer &buffer);
    std::shared_ptr<Array> newArray3D(int type, const py::buffer &buffer);
    
    
    std::set<Object*> listOfAllObjectsCreatedOnThisDevice;

    /*! if enabled (via env-var PYNARI_WARN_MISSING_RELEASES) this
        will print warning messages when objects to out of scope
        without having been properly released */
    const bool warnMissingReleases;
    anari::Device handle = 0;
  };

}
