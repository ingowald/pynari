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

#include "pynari/Model.h"
#include "pynari/Material.h"
#include "pynari/Geometry.h"
#include "pynari/Data.h"
#include "pynari/Group.h"
#include "pynari/Camera.h"
#include "pynari/FrameBuffer.h"

namespace pynari {

  Model::Model(Device::SP device)
    : Object(device)
  {
    handle = anari::newObject<anari::World>(device->handle);
  }

  // std::shared_ptr<Material>
  // Model::createMaterial(const std::string &which)
  // {
  //   Material::SP material = std::make_shared<Material>(nari,handle,which);
  //   return material;
  // }

  // std::shared_ptr<Geometry>
  // Model::createSphere(const std::tuple<float,float,float> &_pos,
  //                     float radius,
  //                     const std::shared_ptr<Material> &material)
  // {
  //   vec3f pos = get(_pos);
  //   std::shared_ptr<Geometry> geom
  //     = std::make_shared<Geometry>(this,0,"spheres",material);
    
  //   // .......................................................
  //   BNData origins = bnDataCreate(handle,0,BN_FLOAT3,1,&pos);
  //   bnSetData(geom->handle,"origins",origins);
  //   bnSet1f  (geom->handle,"radius",radius);
  //   bnCommit(geom->handle);
  //   // .......................................................
  //   return geom;
  // }

  // std::shared_ptr<Group> Model::createGroup(const py::list &list)
  // {
  //   return std::make_shared<Group>(this,0,list);
  // }

  // std::shared_ptr<Geometry>
  // Model::createGeometry(const std::string &type,
  //                       const std::shared_ptr<Material> &material)
  // {
  //   std::shared_ptr<Geometry> geom
  //     = std::make_shared<Geometry>(this,0,type,material);
  //   return geom;
  // }

  // template<typename T, int D>
  // std::shared_ptr<Data> createFromCopyable(Model *model,
  //                                          BNDataType dataType,
  //                                          const py::list &list)
  // {
  // }

  // std::shared_ptr<Data>
  // Model::createData(const std::string &type, const py::list &list)
  // {
  //   if (type == "float2")
  //     return createFromCopyable<float,2>(this,BN_FLOAT2,list);
  //   if (type == "float3")
  //     return createFromCopyable<float,3>(this,BN_FLOAT3,list);
  //   throw std::runtime_error("unknown data type '"+type+"' in py_barn.model.create_data()")
  // }
    
  
  // void 
  // Model::setInstances(const py::list &list)
  // {
  //   std::vector<BNGroup> groups;
  //   std::vector<affine3f> transforms;
  //   for (auto item : list) {
  //     Group::SP group = item.cast<Group::SP>();
  //     if (group) {
  //       groups.push_back(group->handle);
  //       transforms.push_back(affine3f());
  //     } else
  //       throw std::runtime_error("un-recognized element in group/isntance list");
  //   }
  //   bnSetInstances(handle,0,
  //                  groups.data(),
  //                  (BNTransform*)transforms.data(),
  //                  groups.size());
  //   bnBuild(handle,0);
  // }

  // void Model::render(const std::shared_ptr<Camera> &camera,
  //                    const std::shared_ptr<FrameBuffer> &fb)
  // {
  //   bnRender(handle,camera->handle,fb->handle,1024);
  // }
  
}
