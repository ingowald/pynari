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

#include "pynari/Device.h"
#include "pynari/Object.h"

namespace pynari {

  void Device::release()
  {
    PING; PRINT(handle);
    if (!handle)
      /* was already force-relased before, it's only the python object
         that's till live -> don't do anything */
      return;

    // make sure to release all objects _before_ the device itself
    // gets released
    std::set<Object *> copyOfCurrentObjects
      = listOfAllObjectsCreatedOnThisDevice;
    PING;
    for (Object *obj : copyOfCurrentObjects)
      obj->release();

    // and finally, release the device itself
    anari::release(this->handle,this->handle);
    handle = nullptr;
  }
  
}
