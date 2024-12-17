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

cmake_minimum_required(VERSION 3.12)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.18)
  cmake_policy(SET CMP0104 NEW)
endif()

set(CMAKE_BUILD_TYPE_INIT "Release")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#if ((NOT WIN32) AND (NOT CMAKE_BUILD_TYPE) AND (NOT CMAKE_CONFIGURATION_TYPES))
  # Set the possible values of build type for cmake-gui
#  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
#    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
#endif()

# project command is required to come after cmake_minimum_required command.
project(pynari VERSION 1.1.7 LANGUAGES C CXX)

if (NOT (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR}))
  set(PYNARI_IS_SUBPROJECT ON)
else()
  set(PYNARI_IS_SUBPROJECT OFF)
endif()

if (NOT PYNARI_IS_SUBPROJECT)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
endif()

find_package(Python3 COMPONENTS Interpreter Development)

add_subdirectory(external/pybind11 build_pybin EXCLUDE_FROM_ALL)
if (NOT (TARGET stb_image))
  add_subdirectory(external/stb_image  EXCLUDE_FROM_ALL)
endif()

add_subdirectory(pynari)


