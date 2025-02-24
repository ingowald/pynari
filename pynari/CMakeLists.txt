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

#find_package(anari)

set(CMAKE_CXX_STANDARD 20)

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
  dummy.cu
  )
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
target_include_directories(pynari PUBLIC ${PROJECT_SOURCE_DIR})
set_target_properties(pynari PROPERTIES
  PYTHON_MODULE_NAME "pynari"
  )
target_link_libraries(pynari PUBLIC
  anari::anari
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

#include(CheckLanguage)
#check_language(CUDA)
#if (CMAKE_CUDA_COMPILER)
#  enable_language(CUDA)
  target_compile_definitions(pynari PRIVATE -DPYNARI_HAVE_CUDA=1)
  set_target_properties(pynari PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    POSITION_INDEPENDENT_CODE ON
    CUDA_RESOLVE_DEVICE_SYMBOLS ON
    CUDA_USE_STATIC_CUDA_RUNTIME ON
    BUILD_WITH_INSTALL_RPATH ON
    )
#endif()

message("PYNARI_BUILD_FOR_PIP ${PYNARI_BUILD_FOR_PIP}")
if (PYNARI_BUILD_FOR_PIP)
  message("building for pip...")
  if (APPLE)
    set_target_properties(pynari PROPERTIES INSTALL_RPATH "@loader_path")
  else()
    set_target_properties(pynari PROPERTIES INSTALL_RPATH "$ORIGIN")
  endif()
  find_package(barney)
  target_link_libraries(pynari
    PUBLIC
    barney::anari_library_barney
    anari::helium
    anari::anari
    )
  install(
    IMPORTED_RUNTIME_ARTIFACTS
    pynari
    barney::barney
    barney::anari_library_barney
    anari::anari
    
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/pynari
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/pynari
#    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/pynari
    )
  install(
    TARGETS pynari
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/pynari
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/pynari
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/pynari
    )
  install(
  # note(iw): the reason we store this as 'becomes.__init__.py'
  # in the source tree is because we want this file to be
  # 'recognized' by python _only_ when installed through a pip
  # package. If it is called __init__.py in the source
  # tree, python will try to call it even if we run on a
  # local build that's not a pip package, which will
  # fail.
    FILES becomes.__init__.py
    RENAME __init__.py
    # where to put it - we want this in <install>/pynari
    # $(install_dir)/<packagename(=pynari)>/__init__.py
    DESTINATION ${CMAKE_INSTALL_PREFIX}/pynari
    )
else()
  install(
    TARGETS pynari
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()
