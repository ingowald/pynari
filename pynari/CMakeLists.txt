# ======================================================================== #
# Copyright 2024-2024 Ingo Wald                                            #
#                                                                          #
# Licensed under the Apache License, Version 2.0 (the "License");          #
# you may not use this file except in compliance with the License.         #
# You may obtain a copy of the License at                                  #
#                                                                          #
#     http://www.apache.org/licenses/LICENSE-2.0                           #
#                                                                          #
# Unless required by applicable law or agreed to in writing, software      #
# distributed under the License is distributed on an "AS IS" BASIS,        #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. #
# See the License for the specific language governing permissions and      #
# limitations under the License.                                           #
# ======================================================================== #

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION pybind11::thin_lto)

if (NOT (TARGET anari::helium_static))
  find_package(anari 0.12.0 COMPONENTS code_gen)
endif()

pybind11_add_module(pynari
  # 'thin' objects that only handle the reference without any other
  # operations on it:
  Material.h
  Light.h
  World.h
  Volume.h
  Group.h
  Geometry.h
  Camera.h
  Renderer.h
  Surface.h
  SpatialField.h

  # more complicted objects that require additional functions
  Context.h
  Context.cpp
  Object.h
  Object.cpp
  Device.h
  Device.cpp
  Array.h
  Array.cpp
  Frame.h
  Frame.cpp
  
  # the actual pybind11 bindings file
  bindings.cpp
  )
target_include_directories(pynari PUBLIC ${PROJECT_SOURCE_DIR})
set_target_properties(pynari PROPERTIES
  PYTHON_MODULE_NAME "pynari"
  )
if (CMAKE_CUDA_ARCHITECTURES)
  set_target_properties(pynari PROPERTIES
    CUDA_ARCHITECTURES ${CMAKE_CUDA_ARCHITECTURES}
    )
else()
  set_target_properties(pynari PROPERTIES
    CUDA_ARCHITECTURES all-major
    )
endif()

set_target_properties(pynari PROPERTIES
  CUDA_SEPARABLE_COMPILATION ON
  POSITION_INDEPENDENT_CODE ON
  CUDA_RESOLVE_DEVICE_SYMBOLS ON
  CUDA_USE_STATIC_CUDA_RUNTIME ON
  )


if (PYNARI_BAKED_BACKENDS)
  # this path should only ever get used by a dedicated wheel-building
  # reposity that builds a version of pynari with barney (and all
  # other libraries) staticlally hard-linked into it, so as not to
  # depend on any dynamically linked libraries that pip/auditwheel do
  # not exactly like.
  target_compile_definitions(pynari
    PRIVATE
    -DPYNARI_BAKED_BACKENDS=1
    )
  foreach (backend ${PYNARI_BAKED_BACKENDS})
    target_link_libraries(pynari
      PRIVATE 
      anari_library_${backend}_static
      )
    target_compile_definitions(pynari
      PRIVATE
      -DPYNARI_HAVE_${backend}=1
      )
  endforeach()
else()
  target_link_libraries(pynari
    PRIVATE
    anari::anari
    )
endif()

if (NOT PYNARI_IS_SUBPROJECT)
  install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

