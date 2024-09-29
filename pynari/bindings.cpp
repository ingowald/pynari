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
#include "pynari/Model.h"
#include "pynari/Camera.h"
#include "pynari/Geometry.h"
#include "pynari/Group.h"

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
  object.def("setParameter_float",  &pynari::Object::set_float);
  object.def("setParameter_float3", &pynari::Object::set_float3_tuple);
  object.def("setParameter_float3", &pynari::Object::set_float3_numpy);
  object.def("setParameter_float3", &pynari::Object::set_float3_list);
  object.def("commitParameters", &pynari::Object::commit);
  // -------------------------------------------------------
  auto camera
    = py::class_<pynari::Camera,pynari::Object,
                 std::shared_ptr<pynari::Camera>>(m, "anari::Camera");
  // camera.def("set_float3", &pynari::Camera::set3f);
  // camera.def("set_float3", &pynari::Camera::set3fv);
  // camera.def("set_float",  &pynari::Camera::set_float);
  // camera.def("set_float3", &pynari::Camera::set_float3_tuple);
  // camera.def("set_float3", &pynari::Camera::set_float3_numpy);
  // camera.def("set_float3", &pynari::Camera::set_float3_list);
  // camera.def("from_at_up", &pynari::Camera::from_at_up);
  
  // // -------------------------------------------------------
  auto context
    = py::class_<pynari::Context,
                 std::shared_ptr<Context>>(m, "pynari::Device");
  
  // context.def(py::init<>());
  // context.def("newDevice", &pynari::Context::create);
  context.def("newCamera", &pynari::Context::newCamera);
   // context.def("create_frame_buffer", &pynari::Context::fbCreate);
   // context.def("destroy",&pynari::Context::destroy);
}
