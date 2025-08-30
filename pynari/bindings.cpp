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

#define PYBIND11_DETAILED_ERROR_MESSAGES 1
#include "pynari/Context.h"
#include "pynari/World.h"
#include "pynari/Frame.h"
#include "pynari/Instance.h"
#include "pynari/Camera.h"
#include "pynari/Renderer.h"
#include "pynari/Surface.h"
#include "pynari/Material.h"
#include "pynari/Light.h"
#include "pynari/Sampler.h"
#include "pynari/Geometry.h"
#include "pynari/Group.h"
#include "pynari/Array.h"
#include "pynari/SpatialField.h"
#include "pynari/Volume.h"

PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

using namespace pynari;

namespace pynari {
  bool has_cuda_capable_gpu();
}

PYBIND11_MODULE(pynari, m) {
  // optional module docstring
  m.doc() = "barney python wrappers";

  // create one context; almost all functions are then per context
  m.def("newDevice", &createContext,
        "Creates an barney Context object",
        py::arg("libName"),
        py::arg("devName")="default");
 
  m.attr("DATA_TYPE")     = py::int_((int)ANARI_DATA_TYPE);
  m.attr("STRING")        = py::int_((int)ANARI_STRING);
  m.attr("OBJECT")        = py::int_((int)ANARI_OBJECT);
  m.attr("SURFACE")       = py::int_((int)ANARI_SURFACE);
  m.attr("GEOMETRY")      = py::int_((int)ANARI_GEOMETRY);
  m.attr("MATERIAL")      = py::int_((int)ANARI_MATERIAL);
  m.attr("LIGHT")         = py::int_((int)ANARI_LIGHT);
  m.attr("SAMPLER")       = py::int_((int)ANARI_SAMPLER);
  m.attr("RENDERER")      = py::int_((int)ANARI_RENDERER);
  m.attr("SPATIAL_FIELD") = py::int_((int)ANARI_SPATIAL_FIELD);
  m.attr("CAMERA")        = py::int_((int)ANARI_CAMERA);
  m.attr("WORLD")         = py::int_((int)ANARI_WORLD);
  m.attr("SPATIAL_FIELD") = py::int_((int)ANARI_SPATIAL_FIELD);
  m.attr("VOLUME")        = py::int_((int)ANARI_VOLUME);
  
  m.attr("ARRAY")         = py::int_((int)ANARI_ARRAY);
  m.attr("ARRAY1D")       = py::int_((int)ANARI_ARRAY1D);
  m.attr("ARRAY2D")       = py::int_((int)ANARI_ARRAY2D);
  m.attr("ARRAY3D")       = py::int_((int)ANARI_ARRAY3D);

  m.attr("FLOAT32")       = py::int_((int)ANARI_FLOAT32);
  m.attr("FLOAT32_VEC2")  = py::int_((int)ANARI_FLOAT32_VEC2);
  m.attr("FLOAT32_VEC3")  = py::int_((int)ANARI_FLOAT32_VEC3);
  m.attr("FLOAT32_VEC4")  = py::int_((int)ANARI_FLOAT32_VEC4);

  m.attr("FLOAT32_MAT3x4")  = py::int_((int)ANARI_FLOAT32_MAT3x4);
  m.attr("FLOAT32_MAT3X4")  = py::int_((int)ANARI_FLOAT32_MAT3x4);
  
  m.attr("UINT32")        = py::int_((int)ANARI_UINT32);
  m.attr("UINT32_VEC2")   = py::int_((int)ANARI_UINT32_VEC2);
  m.attr("UINT32_VEC3")   = py::int_((int)ANARI_UINT32_VEC3);
  m.attr("UINT32_VEC4")   = py::int_((int)ANARI_UINT32_VEC4);

  m.attr("INT32")         = py::int_((int)ANARI_INT32);
  m.attr("INT32_VEC2")    = py::int_((int)ANARI_INT32_VEC2);
  m.attr("INT32_VEC3")    = py::int_((int)ANARI_INT32_VEC3);
  m.attr("INT32_VEC4")    = py::int_((int)ANARI_INT32_VEC4);

  m.attr("UINT64")        = py::int_((int)ANARI_UINT64);
  m.attr("INT64")         = py::int_((int)ANARI_INT64);
  
  m.attr("UFIXED8_VEC4")  = py::int_((int)ANARI_UFIXED8_VEC4);
  m.attr("UFIXED8_RGBA_SRGB")  = py::int_((int)ANARI_UFIXED8_RGBA_SRGB);

  // iw - not anari spelling, but ...
  m.attr("FLOAT")  = py::int_((int)ANARI_FLOAT32);
  m.attr("float")  = py::int_((int)ANARI_FLOAT32);
  m.attr("float2") = py::int_((int)ANARI_FLOAT32_VEC2);
  m.attr("float3") = py::int_((int)ANARI_FLOAT32_VEC3);
  m.attr("float4") = py::int_((int)ANARI_FLOAT32_VEC4);

  m.attr("UINT")   = py::int_((int)ANARI_UINT32);
  m.attr("uint")   = py::int_((int)ANARI_UINT32);
  m.attr("uint2")  = py::int_((int)ANARI_UINT32_VEC2);
  m.attr("uint3")  = py::int_((int)ANARI_UINT32_VEC3);
  m.attr("uint4")  = py::int_((int)ANARI_UINT32_VEC4);

  m.attr("UINT8")        = py::int_((int)ANARI_UINT8);
  m.attr("UINT8_VEC2")   = py::int_((int)ANARI_UINT8_VEC2);
  m.attr("UINT8_VEC3")   = py::int_((int)ANARI_UINT8_VEC3);
  m.attr("UINT8_VEC4")   = py::int_((int)ANARI_UINT8_VEC4);

  m.attr("FLOAT32_BOX1") = py::int_((int)ANARI_FLOAT32_BOX1);
  m.attr("FLOAT32_BOX2") = py::int_((int)ANARI_FLOAT32_BOX2);
  m.attr("FLOAT32_BOX3") = py::int_((int)ANARI_FLOAT32_BOX3);
  m.attr("FLOAT32_BOX4") = py::int_((int)ANARI_FLOAT32_BOX4);
  m.attr("FLOAT32_RANGE1") = py::int_((int)ANARI_FLOAT32_BOX1);
  m.attr("FLOAT32_RANGE2") = py::int_((int)ANARI_FLOAT32_BOX2);
  m.attr("FLOAT32_RANGE3") = py::int_((int)ANARI_FLOAT32_BOX3);
  m.attr("FLOAT32_RANGE4") = py::int_((int)ANARI_FLOAT32_BOX4);
  
  
  // -------------------------------------------------------
  auto object
    = py::class_<pynari::Object,
                 std::shared_ptr<pynari::Object>>(m, "anari::Object");
  /*! set FROM a python pynari object */
  object.def("setParameter",  &pynari::Object::set_object);
  /*! set FROM a python string */
  object.def("setParameter",  &pynari::Object::set_string);
  object.def("setParameter",  &pynari::Object::set_string_notype);
  object.def("setParameterArray",    &pynari::Object::setArray_list);
  object.def("setParameterArray1D",  &pynari::Object::setArray1D_list);
  // object.def("setParameterArray2D",  &pynari::Object::setArray2D_list);
  // object.def("setParameterArray3D",  &pynari::Object::setArray3D_list);
  object.def("setParameterArray",    &pynari::Object::setArray_np);
  object.def("setParameterArray1D",  &pynari::Object::setArray1D_np);
  object.def("setParameterArray2D",  &pynari::Object::setArray2D_np);
  object.def("setParameterArray3D",  &pynari::Object::setArray3D_np);
  /*! set FROM a python float value */
  object.def("setParameter",  &pynari::Object::set_float);
  /*! set FROM a python float tuple */
  object.def("setParameter",  &pynari::Object::set_float2);
  object.def("setParameter",  &pynari::Object::set_float3);
  object.def("setParameter",  &pynari::Object::set_float4);
  object.def("setParameter",  &pynari::Object::set_float12);
  object.def("setParameter",  &pynari::Object::set_float_vec);

  object.def("setParameter",  &pynari::Object::set_ulong);
  
  object.def("setParameter",  &pynari::Object::set_uint);
  object.def("setParameter",  &pynari::Object::set_uint2);
  object.def("setParameter",  &pynari::Object::set_uint3);
  object.def("setParameter",  &pynari::Object::set_uint4);
  object.def("setParameter",  &pynari::Object::set_uint_vec);
  
  object.def("commitParameters", &pynari::Object::commit);
  object.def("release", &pynari::Object::release);
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
  auto spatialField
    = py::class_<pynari::SpatialField,pynari::Object,
                 std::shared_ptr<pynari::SpatialField>>(m, "anari::SpatialField");
  // -------------------------------------------------------
  auto volume
    = py::class_<pynari::Volume,pynari::Object,
                 std::shared_ptr<pynari::Volume>>(m, "anari::Volume");
  // -------------------------------------------------------
  auto material
    = py::class_<pynari::Material,pynari::Object,
                 std::shared_ptr<pynari::Material>>(m, "anari::Material");
  // -------------------------------------------------------
  auto light
    = py::class_<pynari::Light,pynari::Object,
                 std::shared_ptr<pynari::Light>>(m, "anari::Light");
  // -------------------------------------------------------
  auto sampler
    = py::class_<pynari::Sampler,pynari::Object,
                 std::shared_ptr<pynari::Sampler>>(m, "anari::Sampler");
  // -------------------------------------------------------
  auto geometry
    = py::class_<pynari::Geometry,pynari::Object,
                 std::shared_ptr<pynari::Geometry>>(m, "anari::Geometry");
  // -------------------------------------------------------
  auto world
    = py::class_<pynari::World,pynari::Object,
                 std::shared_ptr<pynari::World>>(m, "anari::World");
  // -------------------------------------------------------
  auto group
    = py::class_<pynari::Group,pynari::Object,
                 std::shared_ptr<pynari::Group>>(m, "anari::Group");
  // -------------------------------------------------------
  auto instance
    = py::class_<pynari::Instance,pynari::Object,
                 std::shared_ptr<pynari::Instance>>(m, "anari::Instance");
  // -------------------------------------------------------
  auto frame
    = py::class_<pynari::Frame,pynari::Object,
                 std::shared_ptr<pynari::Frame>>(m, "anari::Frame");
  frame.def("render", &pynari::Frame::render);
  frame.def("get", &pynari::Frame::get);
  frame.def("map", &pynari::Frame::map);
  frame.def("unmap", &pynari::Frame::unmap);
  frame.def("readGPU", &pynari::Frame::readGPU);
  
  // -------------------------------------------------------
  auto array
    = py::class_<pynari::Array,pynari::Object,
                 std::shared_ptr<pynari::Array>>(m, "anari::Array");
  // // -------------------------------------------------------
  auto context
    = py::class_<pynari::Context,
                 std::shared_ptr<Context>>(m, "anari::Device");
  context.def("setParameter",  &pynari::Context::set_ulong);
  context.def("commitParameters", &pynari::Context::commit);
  
  
  // // -------------------------------------------------------

  m.def("has_cuda_capable_gpu", &pynari::has_cuda_capable_gpu);

  context.def("newCamera",  &pynari::Context::newCamera);
  context.def("newGroup",   &pynari::Context::newGroup);
  context.def("newInstance",&pynari::Context::newInstance);
  context.def("newRenderer",&pynari::Context::newRenderer);
  context.def("newSurface", &pynari::Context::newSurface);
  context.def("newSpatialField", &pynari::Context::newSpatialField);
  context.def("newVolume",  &pynari::Context::newVolume);
  context.def("newMaterial",&pynari::Context::newMaterial);
  context.def("newLight",   &pynari::Context::newLight);
  context.def("newWorld",   &pynari::Context::newWorld);
  context.def("newFrame",   &pynari::Context::newFrame);
  context.def("newGeometry",&pynari::Context::newGeometry);
  context.def("newSampler", &pynari::Context::newSampler);
  
  context.def("newArray",   &pynari::Context::newArray);
  context.def("newArray1D", &pynari::Context::newArray1D);
  context.def("newArray2D", &pynari::Context::newArray2D);
  context.def("newArray3D", &pynari::Context::newArray3D);
  context.def("newArray",   &pynari::Context::newArray_objects);
  context.def("newArray1D", &pynari::Context::newArray1D_objects);
}
