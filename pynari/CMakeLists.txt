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

find_package(anari 0.1.0 COMPONENTS code_gen)

set(EXP_SAUCY ON)
set(BARNEY_BUILD_ANARI ON)
set(BARNEY_DISABLE_DENOISING ON)
set(BARNEY_USE_CUBQL OFF)
add_subdirectory(../../barney EXCLUDE_FROM_ALL builddir_barney)

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
target_link_libraries(pynari
  PRIVATE
  anari::helium
  Python3::Python
  #  "$<LINK_LIBRARY:WHOLE_ARCHIVE,anari_library_barney>"
#  "$<LINK_ONLY:barney_static>"
#"$<LINK_LIBRARY:WHOLE_ARCHIVE,barney_static>"
#barney_static
barney_static
 barney-Spheres-ptx
 barney-StructuredData-ptx
 barney-cylinders-ptx
 barney-capsules-ptx
 barney-triangles-ptx
 barney-traceRays-ptx  
 barney-UMeshRTXObjectSpace-ptx
 barney-UMeshMC-ptx
 barney-AWT-ptx  
 barney-BlockStructuredMC-ptx

  anari_library_barney_static
  )

#get_target_property(LINK_LIBS anari_library_barney INTERFACE_LINK_LIBRARIES)
#get_target_property(LINK_LIBS barney_static INTERFACE_LINK_LIBRARIES)
#message(STATUS "Dependent libraries of my_target: ${LINK_LIBS}")

#foreach(lib ${LINK_LIBS})
#  set(whole_libs "barney_static")
#  message("embeddign ${lib}")
#  string(FIND ${lib} "-ptx" index)
#  message("index ${index}")
#  if (index GREATER -1)
#    message("adding lib ${lib}")
#    set(whole_libs "${whole_libs},${lib}")
#    target_link_libraries(pynari PRIVATE  
#      "$<LINK_LIBRARY:WHOLE_ARCHIVE,${lib}>"
#      #    ${lib}
#      )
#  else()
#    target_link_libraries(pynari PRIVATE 
#      ${lib}
#      )
#  endif()
  #  target_link_libraries(pynari PRIVATE 
  #    "$<LINK_LIBRARY:WHOLE_ARCHIVE,${lib}>"
  #    ${lib}
  #    )
#endforeach()
#message("linking whole: ${whole_libs}")
#target_link_libraries(pynari PRIVATE  
#  "$<LINK_LIBRARY:WHOLE_ARCHIVE,${whole_libs}>"
  #    ${lib}
#  )


#  Python3::Python
#  anari::helium
#  #barney
#  stb_image
#  anari_library_barney
#  )

#target_link_libraries(lib2 PRIVATE "$<LINK_LIBRARY:WHOLE_ARCHIVE,lib1>")

install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

