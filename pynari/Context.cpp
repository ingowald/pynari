// ======================================================================== //
// Copyright 2024++ Ingo Wald                                               //
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

#include "pynari/Context.h"
#include "pynari/Model.h"
#include "pynari/FrameBuffer.h"
#include "pynari/Camera.h"

namespace pynari {

  Context::Context(const std::string &explicitLibName)
  {
    std::cout << OWL_TERMINAL_LIGHT_GREEN
              << "#pynari: creating context..."
              << OWL_TERMINAL_DEFAULT
              << std::endl;

    std::string libName = explicitLibName;
    if (libName == "default" || libName == "<default>") {
      char *envlib = getenv("ANARI_LIBRARY");
      libName = envlib ? "environment" : "barney";
    }
    anari::Library library = anari::loadLibrary(libName.c_str(), nullptr);
    anari::Device device = anari::newDevice(library, "default");
    this->device = std::make_shared<Device>(device);//anari::bnContextCreate());
    std::cout << OWL_TERMINAL_GREEN
              << "#pynari: context created."
              << OWL_TERMINAL_DEFAULT
              << std::endl;
  }
    
  Context::~Context()
  {
    destroy();
  }

  std::shared_ptr<FrameBuffer>
  Context::fbCreate(int sx, int sy)
  {
    FrameBuffer::SP fb = std::make_shared<FrameBuffer>(device);
    fb->resize(math::int2(sx,sy));
    return fb;
  }
  
  std::shared_ptr<Model>
  Context::modelCreate()
  {
    Model::SP model = std::make_shared<Model>(device);
    return model;
  }
  
  std::shared_ptr<Camera>
  Context::newCamera(const std::string &type)
  {
    Camera::SP camera = std::make_shared<Camera>(device,type);
    return camera;
  }
  
  std::shared_ptr<Context> createContext(const std::string &libName)
  {
    return std::make_shared<Context>(libName);
  }

  std::shared_ptr<Context> Context::create(const std::string &libName)
  {
    return std::make_shared<Context>(libName);
  }
  
  // Buffer::SP Context::createDeviceBuffer(int type, const py::buffer &buffer)
  // {
  //   return DeviceBuffer::create(this,(OWLDataType)type,buffer);
  // }
  
  // Buffer::SP Context::createHostPinnedBuffer(int type, int size)
  // {
  //   return HostPinnedBuffer::create(this,(OWLDataType)type,size);
  //   // return Buffer::create(this,(OWLDataType)type,buffer);
  // }

  // Geom::SP Context::createGeom(const std::shared_ptr<GeomType> type)
  // {
  //   return Geom::create(this,type);
  // }

  // MissProg::SP Context::createMissProg(const std::shared_ptr<Module> &module,
  //                                      const std::string &typeName,
  //                                      const std::string &funcName)
  // {
  //   return MissProg::create(this,module,typeName,funcName);
  // }

  // RayGen::SP Context::createRayGen(const std::shared_ptr<Module> &module,
  //                                  const std::string &typeName,
  //                                  const std::string &funcName)
  // {
  //   return RayGen::create(this,module,typeName,funcName);
  // }

  /*! allows to query whether the user has already explicitly called
    contextDestroy. if so, any releases of handles are no longer
    valid because whatever they may have pointed to inside the
    (owl-)context is already dead */
  bool Context::alive()
  {
    return (bool)device;
  }
  
  void Context::destroy()
  {
    if (!device)
      // already destroyed, probably becasue the user called an explicit context::destroy()
      return;
    
    std::cout << OWL_TERMINAL_GREEN
              << "#pynari: context shutting down."
              << OWL_TERMINAL_DEFAULT
              << std::endl;
    anari::release(device->handle,device->handle);
    device = 0;
  }
  
  // Group::SP Context::createTrianglesGeomGroup(const std::vector<Geom::SP> &list
  //                                             //const py::list &list
  //                                             )
  // {
  //   // return Group::createTrianglesGG(this,list);
  // }

}
