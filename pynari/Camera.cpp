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

#include "pynari/Camera.h"
#include "pynari/Model.h"

namespace pynari {

  Camera::Camera(Device::SP device,
                 const std::string &type)
    : Object(device),
      type(type)
  {
    handle = anari::newObject<anari::Camera>(device->handle,type.c_str());
  }

  // void Camera::from_at_up(const std::tuple<float,float,float> &_from,
  //                         const std::tuple<float,float,float> &_at,
  //                         const std::tuple<float,float,float> &_up,
  //                         float fovy,
  //                         float aspect)
  // {
  //   vec3f from = get(_from);
  //   vec3f at = get(_at);
  //   vec3f up = get(_up);
  //   vec3f dir = normalize(at - from);
  //   bnSet3fc(handle,"position",(const float3&)from);
  //   bnSet3fc(handle,"direction",(const float3&)dir);
  //   bnSet3fc(handle,"up",(const float3&)up);
  //   bnSet1f(handle,"aspect",aspect);
  //   bnSet1f(handle,"fovy",fovy);
  //   bnCommit(handle);
  // };
  
}
