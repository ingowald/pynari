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

find_package(anari 0.10.0 COMPONENTS code_gen)
pybind11_add_module(pynari
  Context.cpp
  Object.h
  Material.h
  Material.cpp
  Model.h
  Model.cpp
  Group.h
  Group.cpp
  Geometry.h
  Geometry.cpp
  Camera.h
  Camera.cpp
  FrameBuffer.h
  FrameBuffer.cpp
  bindings.cpp
  )
target_include_directories(pynari PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(pynari
  PRIVATE
  Python::Python
  anari::helium
  #barney
  stb_image
  )



