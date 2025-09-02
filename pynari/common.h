// ======================================================================== //
// Copyright 2024-2024 Ingo Wald                                            //
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

#pragma once

# include <anari/anari_cpp.hpp>
# include "anari/anari_cpp/ext/linalg.h"
// #include <owl/common/math/vec.h>
// #include <owl/common/math/AffineSpace.h>
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <array>
#include <stdint.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <iostream>
#include <cstring>
#include <cstdlib>

namespace pynari {
  using uint = uint32_t;
  
  namespace py = pybind11;
  using namespace anari;
  
  /*! convert pybind tuple to owl vector type (w/ size sanity check) */
  inline math::float2 make_float2(const std::vector<float> &vec)
  {
    if (vec.size() != 2)
      throw std::runtime_error("the tuple passed didn't have the"
                               " expected number of elements (3)");
    return math::float2(vec[0],vec[1]);
  }

  /*! convert pybind tuple to owl vector type (w/ size sanity check) */
  inline math::float3 make_float3(const std::vector<float> &vec)
  {
    if (vec.size() != 3)
      throw std::runtime_error("the tuple passed didn't have the"
                               " expected number of elements (3)");
    return math::float3(vec[0],vec[1],vec[2]);
  }

  /*! convert pybind tuple to owl vector type (w/ size sanity check) */
  inline math::float4 make_float4(const std::vector<float> &vec)
  {
    if (vec.size() != 4)
      throw std::runtime_error("the tuple passed didn't have the"
                               " expected number of elements (3)");
    return math::float4(vec[0],vec[1],vec[2],vec[3]);
  }

  inline math::float2 make_float2(const std::tuple<float,float> &v)
  { return math::float2(std::get<0>(v),std::get<1>(v)); }
  inline math::float3 make_float3(const std::tuple<float,float,float> &v)
  { return math::float3(std::get<0>(v),std::get<1>(v),std::get<2>(v)); }
  inline math::float4 make_float4(const std::tuple<float,float,float,float> &v)
  { return math::float4(std::get<0>(v),std::get<1>(v),std::get<2>(v),std::get<3>(v)); }
  



  /*! convert pybind tuple to owl vector type (w/ size sanity check) */
  inline math::uint2 make_uint2(const std::vector<uint> &vec)
  {
    if (vec.size() != 2)
      throw std::runtime_error("the tuple passed didn't have the"
                               " expected number of elements (3)");
    return math::uint2(vec[0],vec[1]);
  }

  /*! convert pybind tuple to owl vector type (w/ size sanity check) */
  inline math::uint3 make_uint3(const std::vector<uint> &vec)
  {
    if (vec.size() != 3)
      throw std::runtime_error("the tuple passed didn't have the"
                               " expected number of elements (3)");
    return math::uint3(vec[0],vec[1],vec[2]);
  }

  /*! convert pybind tuple to owl vector type (w/ size sanity check) */
  inline math::uint4 make_uint4(const std::vector<uint> &vec)
  {
    if (vec.size() != 4)
      throw std::runtime_error("the tuple passed didn't have the"
                               " expected number of elements (3)");
    return math::uint4(vec[0],vec[1],vec[2],vec[3]);
  }

  inline math::uint2 make_uint2(const std::tuple<uint,uint> &v)
  { return math::uint2(std::get<0>(v),std::get<1>(v)); }
  inline math::uint3 make_uint3(const std::tuple<uint,uint,uint> &v)
  { return math::uint3(std::get<0>(v),std::get<1>(v),std::get<2>(v)); }
  inline math::uint4 make_uint4(const std::tuple<uint,uint,uint,uint> &v)
  { return math::uint4(std::get<0>(v),std::get<1>(v),std::get<2>(v),std::get<3>(v)); }
  
  
#if defined(_MSC_VER)
//&& !defined(__PRETTY_FUNCTION__)
#  define __PRETTY_FUNCTION__ __FUNCTION__
#endif


#ifndef PRINT
# define PRINT(var) std::cout << #var << "=" << var << std::endl;
#ifdef __WIN32__
# define PING std::cout << __FILE__ << "::" << __LINE__ << ": " << __FUNCTION__ << std::endl;
#else
# define PING std::cout << __FILE__ << "::" << __LINE__ << ": " << __PRETTY_FUNCTION__ << std::endl;
#endif
#endif

  
#ifdef WIN32
# define OWL_TERMINAL_RED ""
# define OWL_TERMINAL_GREEN ""
# define OWL_TERMINAL_LIGHT_GREEN ""
# define OWL_TERMINAL_YELLOW ""
# define OWL_TERMINAL_BLUE ""
# define OWL_TERMINAL_LIGHT_BLUE ""
# define OWL_TERMINAL_RESET ""
# define OWL_TERMINAL_DEFAULT OWL_TERMINAL_RESET
# define OWL_TERMINAL_BOLD ""

# define OWL_TERMINAL_MAGENTA ""
# define OWL_TERMINAL_LIGHT_MAGENTA ""
# define OWL_TERMINAL_CYAN ""
# define OWL_TERMINAL_LIGHT_RED ""
#else
# define OWL_TERMINAL_RED "\033[0;31m"
# define OWL_TERMINAL_GREEN "\033[0;32m"
# define OWL_TERMINAL_LIGHT_GREEN "\033[1;32m"
# define OWL_TERMINAL_YELLOW "\033[1;33m"
# define OWL_TERMINAL_BLUE "\033[0;34m"
# define OWL_TERMINAL_LIGHT_BLUE "\033[1;34m"
# define OWL_TERMINAL_RESET "\033[0m"
# define OWL_TERMINAL_DEFAULT OWL_TERMINAL_RESET
# define OWL_TERMINAL_BOLD "\033[1;1m"

# define OWL_TERMINAL_MAGENTA "\e[35m"
# define OWL_TERMINAL_LIGHT_MAGENTA "\e[95m"
# define OWL_TERMINAL_CYAN "\e[36m"
# define OWL_TERMINAL_LIGHT_RED "\033[1;31m"
#endif

}
