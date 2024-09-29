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

#pragma once

#include "pynari/common.h"
// #include "pynari/Module.h"
// #include "pynari/GeomType.h"
// #include "pynari/Buffer.h"
// #include "pynari/Geom.h"
// #include "pynari/Group.h"
// #include "pynari/MissProg.h"
// #include "pynari/RayGen.h"
#include "pynari/FrameBuffer.h"
#include "pynari/Model.h"
#include "pynari/Material.h"

namespace pynari {

  struct Camera;
  struct Model;
  
  struct Context {
    typedef std::shared_ptr<Context> SP;
    
    Context(const std::string &libName);
    
    virtual ~Context();

    static SP create(const std::string &libName);

    std::shared_ptr<FrameBuffer> fbCreate(int sx, int sy);
    std::shared_ptr<Model> modelCreate();
    std::shared_ptr<Camera> newCamera(const std::string &type);

    // /*! create a new PTX Module from ptx file indicated by given
    //     fileName */
    // std::shared_ptr<Module>
    // createModuleFromFile(const std::string &fileName);
    
    // /*! creates a new PTX module directly from a string containting
    //     the PTX code */
    // std::shared_ptr<Module>
    // createModuleFromString(const std::string &fileName);

    // GeomType::SP
    // createGeomType(const int kind,
    //                const std::shared_ptr<Module> &module,
    //                const std::string &name);

    // MissProg::SP createMissProg(const std::shared_ptr<Module> &module,
    //                             const std::string &typeName,
    //                             const std::string &funcName);
    
    // RayGen::SP createRayGen(const std::shared_ptr<Module> &module,
    //                             const std::string &typeName,
    //                             const std::string &funcName);
    
    // std::shared_ptr<Geom>
    // createGeom(const std::shared_ptr<GeomType> type);

    // Buffer::SP createDeviceBuffer(int type, const py::buffer &buffer);
    // Buffer::SP createHostPinnedBuffer(int type, int size);

    // Group::SP createTrianglesGeomGroup(const py::list &list)
    // {
    //   return Group::createTrianglesGG(this,list);
    // }
    // Group::SP createUserGeomGroup(const py::list &list)
    // {
    //   return Group::createUserGG(this,list);
    // }
    // Group::SP createInstanceGroup(const py::list &list)
    // {
    //   return Group::createInstanceGroup(this,list);
    // }

    // void buildPrograms();
    // void buildPipeline();
    // void buildSBT();
    
    /*! allows to query whether the user has already explicitly called
      contextDestroy. if so, any releases of handles are no longer
      valid because whatever they may have pointed to inside the
      (owl-)context is already dead */
    bool alive();

    void destroy();

    Device::SP device;
  };

  std::shared_ptr<Context> createContext(const std::string &libName);

  // void save_png_rgba8(Buffer::SP buffer,
  //                     const std::vector<int> &fbSize,
  //                     const std::string &fileName);
}
