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

#include "pynari/Surface.h"

namespace pynari {

  Surface::Surface(Device::SP device)
    : Object(device)
  {
    handle = anari::newObject<anari::Surface>(device->handle);
    std::cout << "#pynari: have new surface "
              << (int*)this << ":" << (int*)handle << std::endl;
  }
  
  Surface::~Surface()
  {
    std::cout << "#pynari: RELEASING surface "
              << (int*)this << ":" << (int*)handle << std::endl;
    anariRelease(device->handle,handle);
    handle = {};
  }
}
