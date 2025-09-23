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

#include "pynari/Group.h"
#include "pynari/Context.h"
#include "pynari/Surface.h"
#include "pynari/Volume.h"

namespace pynari {
  
  Group::Group(Device::SP device,
               const py::list &list)
    : Object(device)
  {
    handle = anari::newObject<anari::Group>(device->handle);
    std::vector<anari::Object> surfaces;
    std::vector<anari::Object> volumes;
    for (auto item : list) {
      Object::SP object = item.cast<Object::SP>();
      assert(object);
      
      Surface::SP surface = item.cast<Surface::SP>();
      if (surface) { surfaces.push_back(surface->handle); continue; }
      
      Volume::SP volume = item.cast<Volume::SP>();
      if (volume) { volumes.push_back(volume->handle); continue; }
    }

    if (!surfaces.empty())
      anari::setParameterArray1D(device->handle, handle, "surface",
                                 surfaces.data(),surfaces.size());
    if (!volumes.empty())
      anari::setParameterArray1D(device->handle, handle, "volume",
                                 volumes.data(),volumes.size());
    anari::commitParameters(device->handle,handle);
  }

  Group::~Group()
  {
    std::cout << "#pynari: RELEASING group "
              << (int*)this << ":" << (int*)handle << std::endl;
    anariRelease(device->handle,handle);
    handle = {};
  }
  
}

