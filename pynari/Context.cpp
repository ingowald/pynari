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
#include "pynari/Camera.h"
#include "pynari/Renderer.h"
#include "pynari/Surface.h"
#include "pynari/Material.h"
#include "pynari/Light.h"
#include "pynari/Sampler.h"
#include "pynari/World.h"
#include "pynari/Frame.h"
#include "pynari/Group.h"
#include "pynari/Geometry.h"
#include "pynari/Instance.h"
#include "pynari/Array.h"
#include "pynari/SpatialField.h"
#include "pynari/Volume.h"
#ifdef _WIN32
# include <windows.h>
#else
# include <dlfcn.h>
#endif
#include <algorithm>
#ifdef PYNARI_HAVE_CUDA
# include <cuda_runtime.h>
#endif

#define DEFAULT_DEVICE "barney"

// extern "C" ANARIDevice createAnariDeviceBarney();

namespace pynari {

  static void statusFunc(const void * /*userData*/,
                         ANARIDevice /*device*/,
                         ANARIObject source,
                         ANARIDataType /*sourceType*/,
                         ANARIStatusSeverity severity,
                         ANARIStatusCode /*code*/,
                         const char *message)
  {
    if (severity == ANARI_SEVERITY_FATAL_ERROR) {
      fprintf(stderr, "[FATAL][%p] %s\n", source, message);
      std::exit(1);
    } else if (severity == ANARI_SEVERITY_ERROR) {
      fprintf(stderr, "[ERROR][%p] %s\n", source, message);
#ifndef NDEBUG
    } else if (severity == ANARI_SEVERITY_WARNING) {
      fprintf(stderr, "[WARN ][%p] %s\n", source, message);
    } else if (severity == ANARI_SEVERITY_PERFORMANCE_WARNING) {
      fprintf(stderr, "[PERF ][%p] %s\n", source, message);
#endif
    }
    // Ignore INFO/DEBUG messages
  }

  bool has_cuda_capable_gpu() {
#if PYNARI_HAVE_CUDA
    int numGPUs = 0;
    cudaGetDeviceCount(&numGPUs);
    return numGPUs;
#endif
    return false;
  }

  bool readDebugEnvVar()
  {
    char *flag = getenv("PYNARI_DBG");
    if (!flag)
      flag = getenv("PYNARI_LOG_LEVEL");
    if (!flag) return false;
    return std::stoi(flag);
  }

  bool logging_enabled()
  {
    static bool cachedValue = readDebugEnvVar();
    return cachedValue;
  }
  
  anari::Device createDevice(std::string libName,
                             const std::string devName)
  {
    if (libName == "default" || libName == "<default>") {
      char *envLib = getenv("ANARI_LIBRARY");
      libName = envLib ? envLib : "barney";
      libName = envLib ? envLib : DEFAULT_DEVICE;
    }
    if (logging_enabled())
    std::cout << OWL_TERMINAL_LIGHT_GREEN
              << "#pynari: looking for _system_ installed ANARI device '"
              << libName
              << "'"
              << OWL_TERMINAL_DEFAULT << std::endl;
    anari::Library library
      = anari::loadLibrary(libName.c_str(), statusFunc);
    if (!library)
      throw std::runtime_error("pynari: could not load anari library '"
                               +libName+"'");
    anari::Device _device
      = anari::newDevice(library, devName.c_str());
    // anariCommitParameters(_device,_device);
    return _device;
  }

  Context::Context(const std::string &explicitLibName, const std::string &subName)
  {
    this->device
      = std::make_shared<Device>(createDevice(explicitLibName,subName),this);
  }
    
  Context::~Context()
  {
    destroy();
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
  
  std::shared_ptr<Instance>
  Context::newInstance(const std::string &type)
  {
    return std::make_shared<Instance>(device,type);
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
  
  std::shared_ptr<Sampler>
  Context::newSampler(const std::string &type)
  {
    return std::make_shared<Sampler>(device,type);
  }
 
  std::shared_ptr<Light>
  Context::newLight(const std::string &type)
  {
    return std::make_shared<Light>(device,type);
  }
 
  std::shared_ptr<Array>
  Context::newArray_objects(int type, const py::list &list)
  {
    static bool warned = false;
    if (warned == false) {
      std::cout
        << "#pynari: this python app using pynari just called Object::newArray()\n"
        << "#pynari: due to some changes in the ANARI SDK these calls are now (starting\n"
        << "#pynari: with v0.15) any such call should now be replaced with either\n"
        << "#pynari: newArray1D, newArray2D, or newArray3D,\n"
        << "#pynari: depending on what dimensionality the underlying array is\n"
        << "#pynari: supposed to be. I'm trying my best to figure this out, but\n"
        << "#pynari: the better way would be for the app to swtich to the new\n"
        << "#pynari: intended behavior.\n"
        ;
      warned = true;
    }
    return newArray1D_objects(type,list);
  }

  std::shared_ptr<Array>
  Context::newArray1D_objects(int type, const py::list &list)
  {
    std::vector<Object::SP> objects;
    for (auto item : list) {
      Object::SP object = item.cast<Object::SP>();
      assert(object);
      objects.push_back(object);
    }
    return std::make_shared<Array>(device,(anari::DataType)type,objects);
  }
  
  std::shared_ptr<Group>
  Context::newGroup(const py::list &list)
  {
    return std::make_shared<Group>(device,list);
  }

  std::shared_ptr<Array>
  Context::newArray(int type, const py::buffer &buffer)
  {
    static bool warned = false;
    if (warned == false) {
      std::cout
        << "#pynari: this python app using pynari just called Object::newArray()\n"
        << "#pynari: due to some changes in the ANARI SDK these calls are now (starting\n"
        << "#pynari: with v0.15) any such call should now be replaced with either\n"
        << "#pynari: newArray1D, newArray2D, or newArray3D,\n"
        << "#pynari: depending on what dimensionality the underlying array is\n"
        << "#pynari: supposed to be. I'm trying my best to figure this out, but\n"
        << "#pynari: the better way would be for the app to swtich to the new\n"
        << "#pynari: intended behavior.\n"
        ;
      warned = true;
    }
    return newArray1D(type,buffer);
  }

  std::shared_ptr<Array>
  Context::newArray1D(int type, const py::buffer &buffer)
  {
    return std::make_shared<Array>(device,1,(anari::DataType)type,buffer);
  }
  
  std::shared_ptr<Array>
  Context::newArray2D(int type, const py::buffer &buffer)
  {
    return std::make_shared<Array>(device,2,(anari::DataType)type,buffer);
  }
  
  std::shared_ptr<Array>
  Context::newArray3D(int type, const py::buffer &buffer)
  {
    return std::make_shared<Array>(device,3,(anari::DataType)type,buffer);
  }
  
  std::shared_ptr<Context> createContext(const std::string &libName,
                                         const std::string &subName)
  {
    return std::make_shared<Context>(libName,subName);
  }

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
      // already destroyed, probably becasue the user called an
      // explicit context::destroy()
      return;
    
    device->release();
    device = nullptr;
  }

  void Context::set_ulong(const char *name,
                          int type,
                          uint64_t v)
  {
    switch(type) {
    case ANARI_DATA_TYPE:
      return anari::setParameter(device->handle,device->handle,name,
                                 (anari::DataType)v);
    case ANARI_INT32:
      return anari::setParameter(device->handle,device->handle,name,
                                 (int)v);
    case ANARI_INT64:
      return anari::setParameter(device->handle,device->handle,name,
                                 (int64_t)v);
    case ANARI_UINT64:
      return anari::setParameter(device->handle,device->handle,name,
                                 (uint64_t)v);
    case ANARI_UINT32:
      return anari::setParameter(device->handle,device->handle,name,
                                 (uint)v);
    case ANARI_FLOAT32:
      return anari::setParameter(device->handle,device->handle,name,
                                 (float)v);
    default:
      throw std::runtime_error
        (std::string(__PRETTY_FUNCTION__)
         +" unsupported type "+to_string((anari::DataType)type));
    }
  }

  void Context::commit()
  {
    anariCommitParameters(device->handle,device->handle);
  }
  
}
