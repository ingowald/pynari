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
#include "pynari/FrameBuffer.h"
#include "pynari/Camera.h"
#include "pynari/Renderer.h"
#include "pynari/Surface.h"
#include "pynari/Material.h"
#include "pynari/Light.h"
#include "pynari/World.h"
#include "pynari/Frame.h"
#include "pynari/Geometry.h"
#include "pynari/Array.h"
#include "pynari/SpatialField.h"
#include "pynari/Volume.h"

namespace pynari {
  
  Context::Context(const std::string &explicitLibName)
  {
    std::cout << OWL_TERMINAL_LIGHT_GREEN
              << "#pynari: creating context..."
              << OWL_TERMINAL_DEFAULT
              << std::endl;

    std::string libName = explicitLibName;
    if (libName == "default" || libName == "<default>") {
      char *envLib = getenv("ANARI_LIBRARY");
      libName = envLib ? envLib : "barney";
    }
    anari::Library library
      = anari::loadLibrary(libName.c_str(), nullptr);
    if (!library)
      throw std::runtime_error("pynari: could not load anari library '"
                               +libName+"'");
    anari::Device device
      = anari::newDevice(library, "default");
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
  
  std::shared_ptr<World>
  Context::newWorld()
  {
    return std::make_shared<World>(device);
  }
  
  std::shared_ptr<Frame>
  Context::newFrame()
  {
    return std::make_shared<Frame>(device);
  }
  
  std::shared_ptr<Geometry>
  Context::newGeometry(const std::string &type)
  {
    return std::make_shared<Geometry>(device,type);
  }
  
  std::shared_ptr<Renderer>
  Context::newRenderer(const std::string &type)
  {
    return std::make_shared<Renderer>(device,type);
  }
  
  std::shared_ptr<Camera>
  Context::newCamera(const std::string &type)
  {
    return std::make_shared<Camera>(device,type);
  }
  
  std::shared_ptr<Surface>
  Context::newSurface()
  {
    return std::make_shared<Surface>(device);
  }
  
  std::shared_ptr<Volume>
  Context::newVolume(const std::string &type)
  {
    return std::make_shared<Volume>(device,type);
  }
  
  std::shared_ptr<SpatialField>
  Context::newSpatialField(const std::string &type)
  {
    return std::make_shared<SpatialField>(device,type);
  }
  
  std::shared_ptr<Material>
  Context::newMaterial(const std::string &type)
  {
    return std::make_shared<Material>(device,type);
  }
  
  std::shared_ptr<Light>
  Context::newLight(const std::string &type)
  {
    return std::make_shared<Light>(device,type);
  }

  std::shared_ptr<Array>
  Context::newArray_objects(int type, const py::list &list)
  {
    // std::vector<Object::SP> objects;
    std::vector<Object::SP> objects;
    for (auto item : list) {
      Object::SP object = item.cast<Object::SP>();
      assert(object);
      objects.push_back(object);
      // ANARIObject handle = object->getHandle();
      // objects.push_back(handle);
    }
    return std::make_shared<Array>(device,(anari::DataType)type,objects);
  }

  std::shared_ptr<Array>
  Context::newArray(int type, const py::buffer &buffer)
  {
    return std::make_shared<Array>(device,(anari::DataType)type,buffer);
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
  
}
