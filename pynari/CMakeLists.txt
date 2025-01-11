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

if (NOT (TARGET anari))
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

include(CheckLanguage)
check_language(CUDA)
if (CMAKE_CUDA_COMPILER)
  enable_language(CUDA)
  set_source_files_properties(
    Context.cpp 
    PROPERTIES 
    LANGUAGE CUDA
    )
  target_compile_definitions(pynari PRIVATE __PYNARI_HAVE_CUDA__=1)
  set_target_properties(pynari PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    POSITION_INDEPENDENT_CODE ON
    CUDA_RESOLVE_DEVICE_SYMBOLS ON
    CUDA_USE_STATIC_CUDA_RUNTIME ON
    )
endif()

if (PYNARI_BAKED_BACKENDS)
  string(REPLACE ";" "@" PYNARI_BAKED_BACKENDS_LIST "${PYNARI_BAKED_BACKENDS}")
  target_compile_definitions(pynari
    PRIVATE
    -DPYNARI_BAKED_BACKENDS=1
    -DPYNARI_BAKED_BACKENDS_LIST="${PYNARI_BAKED_BACKENDS_LIST}"
    )
#  if (APPLE)
#    set_target_properties(pynari PROPERTIES INSTALL_RPATH "@loader_path")
#  else()
#    set_target_properties(pynari PROPERTIES INSTALL_RPATH "$ORIGIN/pynari")
#    #set_target_properties(pynari PROPERTIES INSTALL_RPATH "$ORIGIN")
#  endif()
  message("*************************************************************************************************")
  message("*************************************************************************************************")
  message("*************************************************************************************************")
  message("*************************************************************************************************")
  foreach (backend ${PYNARI_BAKED_BACKENDS})
    message("finding pynari_baked_${backend}")
    find_library(BAKED_LIB
      "pynari_baked_${backend}"
      PATHS ${CMAKE_INSTALL_PREFIX}/pynari
      REQUIRED
      )
    message("pynari: linking pynari to baked ${backend} lib ${BAKED_LIB}")
    target_link_libraries(pynari
      PUBLIC
      ${BAKED_LIB}
      )
  endforeach()
endif()
message("*************************************************************************************************")
  message("*************************************************************************************************")
  message("*************************************************************************************************")
  message("*************************************************************************************************")
  
if (TARGET anari::anari)
  target_link_libraries(pynari
    PRIVATE
    anari::anari_static#::anari
    )
else()
  target_link_libraries(pynari
    PRIVATE
    anari_static#::anari
    )
endif()



#install(TARGETS pynari
#  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/pynari
#  )
#set_target_properties(pynari PROPERTIES INSTALL_RPATH "$ORIGIN")


