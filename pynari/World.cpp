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

#include "pynari/World.h"

namespace pynari {

  World::World(Device::SP device)
    : Object(device)
  {
    handle = anari::newObject<anari::World>(device->handle);
    PYNARI_TRACK_LEAKS(std::cout << "#pynari: have new world "
                       << (int*)this << ":" << (int*)handle << std::endl);
  }
  
  World::~World()
  {
    PYNARI_TRACK_LEAKS(std::cout << "#pynari: RELEASING world "
                       << (int*)this << ":" << (int*)handle << std::endl);
    anariRelease(device->handle,handle);
    handle = {};
  }

  const std::tuple<float, float, float, float, float, float> World::getBounds()
  {
      anari::math::float3 bounds[2] = { {0.f, 0.f, 0.f}, {1.f, 1.f, 1.f} };

      if (!anariGetProperty(device->handle,
          handle,
          "bounds",
          ANARI_FLOAT32_BOX3,
          &bounds[0],
          sizeof(bounds),
          ANARI_WAIT)) {
          printf("WARNING: bounds not returned by the device! Using unit cube.\n");
      }

	  return std::make_tuple(bounds[0].x, bounds[0].y, bounds[0].z,
		  bounds[1].x, bounds[1].y, bounds[1].z);
  }
}
