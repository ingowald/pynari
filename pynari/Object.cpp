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

#include "pynari/Object.h"

namespace pynari {

  Object::Object(Device::SP device)
    : device(device)
  {}
  
  std::string Object::toString() const { return "<Object>"; }

  void Object::commit() { anariCommitParameters(device->handle,getHandle()); }

  void Object::set_float(const std::string &name, float x)
  { anari::setParameter(device->handle,getHandle(),name.c_str(),x); }
    
  void Object::set_float3_tuple(const char *name,
                                const std::tuple<float,float,float> &v)
  { PING; }

  void Object::set_float3_list(const char *name,
                               const std::vector<float> &v)
  { PING; }
    
  void Object::set_float3_numpy(const char *name,
                                const py::buffer &buffer)
  { PING; }
  
}

