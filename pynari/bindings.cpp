// ======================================================================== //
// Copyright 2024 Ingo Wald                                                 //
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

#include "pynari/Context.h"
#include "pynari/Data.h"
#include "pynari/World.h"
#include "pynari/Frame.h"
#include "pynari/Camera.h"
#include "pynari/Renderer.h"
#include "pynari/Surface.h"
#include "pynari/Material.h"
#include "pynari/Geometry.h"
#include "pynari/Group.h"
#include "pynari/Array.h"
 
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

using namespace pynari;

PYBIND11_MODULE(pynari, m) {
  // optional module docstring
  m.doc() = "barney python wrappers";

  // create one context; almost all functions are then per context
  m.def("newDevice", &createContext,
        "Creates an barney Context object");
  // m.def("newDevice", &pynari::newDevice,
  //       "Creates an barney Context object");

  // // define add function
  // m.def("save_png_rgba8", &pynari::save_png_rgba8,
  //       "Saves a OWLBuffer of rgba8 values to a given file, in PNG format");
  
  // // -------------------------------------------------------
  // auto module
  //   = py::class_<pynari::Module,
  //                std::shared_ptr<Module>>(m, "Module");

  
  // // -------------------------------------------------------
  // auto group
  //   = py::class_<pynari::Group,
  //                std::shared_ptr<Group>>(m, "Group");
  
  // // -------------------------------------------------------
  // auto model
  //   = py::class_<pynari::Model,
  //                std::shared_ptr<Model>>(m, "Model");
  // model.def("create_material", &pynari::Model::createMaterial);
  // model.def("create_sphere",   &pynari::Model::createSphere);
  // model.def("create_geometry", &pynari::Model::createGeometry);
  // model.def("create_group",    &pynari::Model::createGroup);
  // model.def("set_instances",   &pynari::Model::setInstances);
  // model.def("render",          &pynari::Model::render);
  // model.def("create_data",     &pynari::Context::createData);
  // // model.def("test", &pynari::Model::test);
  
  // // -------------------------------------------------------
  // auto data
  //   = py::class_<pynari::Data,
  //                std::shared_ptr<Data>>(m, "Data");
  // // -------------------------------------------------------
  // auto material
  //   = py::class_<pynari::Material,
  //                std::shared_ptr<Material>>(m, "Material");
  // material.def("set_float3", &pynari::Material::set3f);
  // material.def("set_float3", &pynari::Material::set3fv);
  // material.def("set_float",  &pynari::Material::set1f);
  
  // // -------------------------------------------------------
  // auto frameBuffer
  //   = py::class_<pynari::FrameBuffer,
  //                std::shared_ptr<FrameBuffer>>(m, "FrameBuffer");
  // frameBuffer.def("save", &pynari::FrameBuffer::save);
  
  
  // // -------------------------------------------------------
  // auto geometry
  //   = py::class_<pynari::Geometry,
  //                std::shared_ptr<Geometry>>(m, "Geometry");
  // geometry.def("commit",   &pynari::Geometry::commit);
  // geometry.def("set_data", &pynari::Geometry::setData);

  // -------------------------------------------------------
  auto object
    = py::class_<pynari::Object,
                 std::shared_ptr<pynari::Object>>(m, "anari::Object");
  object.def("setParameter",  &pynari::Object::set);
  object.def("setParameter",  &pynari::Object::set_list);
  object.def("setParameter_type",  &pynari::Object::set_type);
  object.def("setParameter_float",  &pynari::Object::set_float);
  object.def("setParameter_float2", &pynari::Object::set_float2_tuple);
  object.def("setParameter_float2", &pynari::Object::set_float2_list);
  object.def("setParameter_float3", &pynari::Object::set_float3_tuple);
  object.def("setParameter_float3", &pynari::Object::set_float3_list);
  object.def("setParameter_float4", &pynari::Object::set_float4_tuple);
  object.def("setParameter_float4", &pynari::Object::set_float4_list);
  object.def("setParameter_uint",  &pynari::Object::set_uint);
  object.def("setParameter_uint2", &pynari::Object::set_uint2_tuple);
  object.def("setParameter_uint2", &pynari::Object::set_uint2_list);
  object.def("setParameter_uint3", &pynari::Object::set_uint3_tuple);
  object.def("setParameter_uint3", &pynari::Object::set_uint3_list);
  object.def("setParameter_uint4", &pynari::Object::set_uint4_tuple);
  object.def("setParameter_uint4", &pynari::Object::set_uint4_list);
  object.def("commitParameters", &pynari::Object::commit);
  // -------------------------------------------------------
  auto camera
    = py::class_<pynari::Camera,pynari::Object,
                 std::shared_ptr<pynari::Camera>>(m, "anari::Camera");
  // -------------------------------------------------------
  auto renderer
    = py::class_<pynari::Renderer,pynari::Object,
                 std::shared_ptr<pynari::Renderer>>(m, "anari::Renderer");
  // -------------------------------------------------------
  auto surface
    = py::class_<pynari::Surface,pynari::Object,
                 std::shared_ptr<pynari::Surface>>(m, "anari::Surface");
  // -------------------------------------------------------
  auto material
    = py::class_<pynari::Material,pynari::Object,
                 std::shared_ptr<pynari::Material>>(m, "anari::Material");
  // -------------------------------------------------------
  auto geometry
    = py::class_<pynari::Geometry,pynari::Object,
                 std::shared_ptr<pynari::Geometry>>(m, "anari::Geometry");
  // -------------------------------------------------------
  auto world
    = py::class_<pynari::World,pynari::Object,
                 std::shared_ptr<pynari::World>>(m, "anari::World");
  // -------------------------------------------------------
  auto frame
    = py::class_<pynari::Frame,pynari::Object,
                 std::shared_ptr<pynari::Frame>>(m, "anari::Frame");
  frame.def("render", &pynari::Frame::render);
  
  // -------------------------------------------------------
  auto array
    = py::class_<pynari::Array,pynari::Object,
                 std::shared_ptr<pynari::Array>>(m, "anari::Array");
  // // -------------------------------------------------------
  auto context
    = py::class_<pynari::Context,
                 std::shared_ptr<Context>>(m, "anari::Device");
  // // -------------------------------------------------------
  
  context.def("newCamera", &pynari::Context::newCamera);
  context.def("newRenderer", &pynari::Context::newRenderer);
  context.def("newSurface", &pynari::Context::newSurface);
  context.def("newMaterial", &pynari::Context::newMaterial);
  context.def("newWorld", &pynari::Context::newWorld);
  context.def("newFrame", &pynari::Context::newFrame);
  context.def("newGeometry", &pynari::Context::newGeometry);
  context.def("newArray", &pynari::Context::newArray);
}
