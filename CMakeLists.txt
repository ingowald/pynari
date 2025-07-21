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

set(PYNARI_VERSION_MAJOR 1)
set(PYNARI_VERSION_MINOR 2)
set(PYNARI_VERSION_PATCH 9)

cmake_minimum_required(VERSION 3.12)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.18)
  cmake_policy(SET CMP0104 NEW)
endif()

set(CMAKE_BUILD_TYPE_INIT "Release")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# project command is required to come after cmake_minimum_required command.
set(PYNARI_VERSION ${PYNARI_VERSION_MAJOR}.${PYNARI_VERSION_MINOR}.${PYNARI_VERSION_PATCH})
project(pynari VERSION ${PYNARI_VERSION} LANGUAGES C CXX)

set(PYNARI_HAVE_CUDA OFF)
include(CheckLanguage)
check_language(CUDA)
if (CMAKE_CUDA_COMPILER)
  enable_language(CUDA)
  set(PYNARI_HAVE_CUDA ON)
  message("found CUDA: ${CMAKE_CUDA_COMPILER}")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

find_package(anari 0.15.0 COMPONENTS code_gen)

set(PYBIND11_FINDPYTHON ON)
add_subdirectory(external/pybind11 build_pybin EXCLUDE_FROM_ALL)
if (NOT (TARGET stb_image))
  add_subdirectory(external/stb_image  EXCLUDE_FROM_ALL)
endif()

add_subdirectory(pynari)


