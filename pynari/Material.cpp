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

#include "pynari/Material.h"

namespace pynari {

  Material::Material(Device::SP device,
                     const std::string &type)
    : Object(device),
      type(type)
  {
    handle = anari::newObject<anari::Material>(device->handle,type.c_str());
    std::cout << "#pynari: have new material "
              << (int*)this << ":" << (int*)handle << std::endl;
  }
  
  Material::~Material()
  {
    std::cout << "#pynari: RELEASING material "
              << (int*)this << ":" << (int*)handle << std::endl;
    anariRelease(device->handle,handle);
    handle = {};
  }
}
