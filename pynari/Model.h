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

#pragma once

#include "pynari/Object.h"

namespace pynari {

  struct Material;
  struct Geometry;
  struct Group;
  struct Camera;
  struct FrameBuffer;
  struct Data;
  
  struct Model : public Object {
    typedef std::shared_ptr<Model> SP;
    
    Model(Device::SP device);
    virtual ~Model() = default;
    std::string toString() const override { return "pynari::Model"; }
    anari::Object getHandle() const override { return handle; }
    
    // std::shared_ptr<Material> createMaterial(const std::string &type);
    // // std::shared_ptr<Geometry> createSphere(float x, float y, float z,
    // //                                        float r,
    // //                                        const std::shared_ptr<Material> &material);
    // // std::shared_ptr<Geometry> createSphere(const py::tuple &pos,//std::array<float,3> &pos,
    // std::shared_ptr<Geometry>
    // createSphere(const std::tuple<float,float,float> &pos,
    //              float r,
    //              const std::shared_ptr<Material> &material);

    // std::shared_ptr<Geometry>
    // createGeometry(const std::string &type,
    //                const std::shared_ptr<Material> &material);
    
    // std::shared_ptr<Group>
    // createGroup(const py::list &list);
    
    // std::shared_ptr<Data>
    // createData(const std::string &type, const py::list &list);
    
    // void 
    // setInstances(const py::list &list);
    
    // void render(const std::shared_ptr<Camera> &camera,
    //             const std::shared_ptr<FrameBuffer> &fb);
    
    anari::World handle = 0;
  };

}
