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
#include "pynari/Object.h"

namespace pynari {

  struct Camera;
  struct Renderer;
  struct Surface;
  struct World;
  struct Frame;
  struct Geometry;
  struct Array;
  struct Material;
  struct Light;
  struct SpatialField;
  struct Volume;
  struct Sampler;
  
  struct Context {
    typedef std::shared_ptr<Context> SP;
    
    Context(const std::string &libName, const std::string &subName);
    
    virtual ~Context();

    static SP create(const std::string &libName, const std::string &devName);

    std::shared_ptr<World> newWorld();
    std::shared_ptr<Frame> newFrame();
    std::shared_ptr<Geometry> newGeometry(const std::string &type);
    std::shared_ptr<Camera> newCamera(const std::string &type);
    std::shared_ptr<Renderer> newRenderer(const std::string &type);
    std::shared_ptr<Surface> newSurface();
    std::shared_ptr<SpatialField> newSpatialField(const std::string &type);
    std::shared_ptr<Volume> newVolume(const std::string &type);
    std::shared_ptr<Sampler> newSampler(const std::string &type);
    std::shared_ptr<Array> newArray(int type, const py::buffer &buffer);
    std::shared_ptr<Array> newArray_objects(int type,
                                                const py::list &list);
    std::shared_ptr<Material> newMaterial(const std::string &type);
    std::shared_ptr<Light> newLight(const std::string &type);

    /*! allows to query whether the user has already explicitly called
      contextDestroy. if so, any releases of handles are no longer
      valid because whatever they may have pointed to inside the
      (owl-)context is already dead */
    bool alive();

    void destroy();

    Device::SP device;
    std::mutex mutex;
  };

  std::shared_ptr<Context> createContext(const std::string &libName,
					 const std::string &devName="default");
}
