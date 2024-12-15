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
#include "pynari/Data.h"
#include "pynari/World.h"
#include "pynari/Frame.h"
#include "pynari/Camera.h"
#include "pynari/Renderer.h"
#include "pynari/Surface.h"
#include "pynari/Material.h"
#include "pynari/Light.h"
#include "pynari/Geometry.h"
#include "pynari/Group.h"
#include "pynari/Array.h"
#include "pynari/SpatialField.h"
#include "pynari/Volume.h"

PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

using namespace pynari;

PYBIND11_MODULE(PYNARI_MODULE_NAME, m) {
  // optional module docstring
  m.doc() = "barney python wrappers";

  // create one context; almost all functions are then per context
  m.def("newDevice", &createContext,
        "Creates an barney Context object");
 
  m.attr("DATA_TYPE")    = py::int_((int)ANARI_DATA_TYPE);
  m.attr("STRING")       = py::int_((int)ANARI_STRING);
  m.attr("OBJECT")       = py::int_((int)ANARI_OBJECT);
  m.attr("SURFACE")      = py::int_((int)ANARI_SURFACE);
  m.attr("GEOMETRY")     = py::int_((int)ANARI_GEOMETRY);
  m.attr("MATERIAL")     = py::int_((int)ANARI_MATERIAL);
  m.attr("LIGHT")        = py::int_((int)ANARI_LIGHT);
  m.attr("RENDERER")     = py::int_((int)ANARI_RENDERER);
  m.attr("SPATIAL_FIELD") = py::int_((int)ANARI_SPATIAL_FIELD);
  m.attr("CAMERA")       = py::int_((int)ANARI_CAMERA);
  m.attr("WORLD")        = py::int_((int)ANARI_WORLD);
  m.attr("SPATIAL_FIELD")= py::int_((int)ANARI_SPATIAL_FIELD);
  m.attr("VOLUME")       = py::int_((int)ANARI_VOLUME);
  
  m.attr("ARRAY")        = py::int_((int)ANARI_ARRAY);
  m.attr("ARRAY1D")      = py::int_((int)ANARI_ARRAY1D);
  m.attr("ARRAY2D")      = py::int_((int)ANARI_ARRAY2D);
  m.attr("ARRAY3D")      = py::int_((int)ANARI_ARRAY3D);

  m.attr("FLOAT32")      = py::int_((int)ANARI_FLOAT32);
  m.attr("FLOAT32_VEC2") = py::int_((int)ANARI_FLOAT32_VEC2);
  m.attr("FLOAT32_VEC3") = py::int_((int)ANARI_FLOAT32_VEC3);
  m.attr("FLOAT32_VEC4") = py::int_((int)ANARI_FLOAT32_VEC4);

  m.attr("UINT32")       = py::int_((int)ANARI_UINT32);
  m.attr("UINT32_VEC2")  = py::int_((int)ANARI_UINT32_VEC2);
  m.attr("UINT32_VEC3")  = py::int_((int)ANARI_UINT32_VEC3);
  m.attr("UINT32_VEC4")  = py::int_((int)ANARI_UINT32_VEC4);

  m.attr("INT32")       = py::int_((int)ANARI_INT32);
  m.attr("INT32_VEC2")  = py::int_((int)ANARI_INT32_VEC2);
  m.attr("INT32_VEC3")  = py::int_((int)ANARI_INT32_VEC3);
  m.attr("INT32_VEC4")  = py::int_((int)ANARI_INT32_VEC4);

  m.attr("UFIXED8_VEC4")  = py::int_((int)ANARI_UFIXED8_VEC4);

  // iw - not anari spelling, but ...
  m.attr("FLOAT")        = py::int_((int)ANARI_FLOAT32);
  m.attr("float")        = py::int_((int)ANARI_FLOAT32);
  m.attr("float2")       = py::int_((int)ANARI_FLOAT32_VEC2);
  m.attr("float3")       = py::int_((int)ANARI_FLOAT32_VEC3);
  m.attr("float4")       = py::int_((int)ANARI_FLOAT32_VEC4);

  m.attr("UINT")         = py::int_((int)ANARI_UINT32);
  m.attr("uint")         = py::int_((int)ANARI_UINT32);
  m.attr("uint2")        = py::int_((int)ANARI_UINT32_VEC2);
  m.attr("uint3")        = py::int_((int)ANARI_UINT32_VEC3);
  m.attr("uint4")        = py::int_((int)ANARI_UINT32_VEC4);

  
  // -------------------------------------------------------
  auto object
    = py::class_<pynari::Object,
                 std::shared_ptr<pynari::Object>>(m, "anari::Object");
  /*! set FROM a python pynari object */
  object.def("setParameter",  &pynari::Object::set_object);
  /*! set FROM a python string */
  object.def("setParameter",  &pynari::Object::set_string);
  object.def("setParameterArray",  &pynari::Object::setArray_list);
  /*! set FROM a python float value */
  object.def("setParameter",  &pynari::Object::set_float);
  /*! set FROM a python float tuple */
  object.def("setParameter",  &pynari::Object::set_float2);
  object.def("setParameter",  &pynari::Object::set_float3);
  object.def("setParameter",  &pynari::Object::set_float4);
  object.def("setParameter",  &pynari::Object::set_float_vec);

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
  frame.def("get", &pynari::Frame::get);
  
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
  context.def("newSpatialField", &pynari::Context::newSpatialField);
  context.def("newVolume", &pynari::Context::newVolume);
  context.def("newMaterial", &pynari::Context::newMaterial);
  context.def("newLight", &pynari::Context::newLight);
  context.def("newWorld", &pynari::Context::newWorld);
  context.def("newFrame", &pynari::Context::newFrame);
  context.def("newGeometry", &pynari::Context::newGeometry);
  context.def("newArray", &pynari::Context::newArray);
  context.def("newArray", &pynari::Context::newArray_objects);
  context.def("newArray", &pynari::Context::newArray);
  context.def("newArray", &pynari::Context::newArray_objects);
}
