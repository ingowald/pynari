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
#include "pynari/World.h"
#include "pynari/Frame.h"
#include "pynari/Geometry.h"
#include "pynari/Array.h"
#include "pynari/SpatialField.h"
#include "pynari/Volume.h"
#ifdef _WIN32
# include <windows.h>
#else
# include <dlfcn.h>
#endif
#include <algorithm>
#ifdef __PYNARI_HAVE_CUDA__
# include <cuda_runtime.h>
#endif

#define DEFAULT_DEVICE "barney"

extern "C" ANARIDevice createAnariDeviceBarney();

namespace pynari {

  bool has_cuda_capable_gpu() {
#ifdef __PYNARI_HAVE_CUDA__
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
  
#if PYNARI_BAKED_BACKENDS
  std::vector<std::string> getListOfBakedBackends()
  {
    static std::vector<std::string> listOfBakedBackends;
    if (listOfBakedBackends.empty()) {
      static char *fromEnv = getenv("PYNARI_BAKED");
      if (fromEnv) {
        listOfBakedBackends.push_back(fromEnv);
      } else {
        std::string allBaked = PYNARI_BAKED_BACKENDS_LIST;
        // PING; PRINT(allBaked);
        while (allBaked != "") {
          int pos = allBaked.find("@");
          const std::string baked = allBaked.substr(0,pos);
          listOfBakedBackends.push_back(baked);
          if (pos == allBaked.npos)
            allBaked = "";
          else
            allBaked = allBaked.substr(pos+1);
        }
      }
    }
    return listOfBakedBackends;
  }

  void *getLoadedLibraryFunction(const std::string &libName,
                                 const std::string &fctName)
  {
    static std::map<std::pair<std::string,std::string>,void *> alreadyLoaded;
    std::pair<std::string,std::string> key(libName,fctName);
    if (alreadyLoaded.find(key) == alreadyLoaded.end()) {
# ifdef _WIN32
      HMODULE lib = LoadLibrary(libName.c_str());//L"nvcuda.dll");
      if (!lib) throw std::runtime_error("could not load "+libName);
      
      void* sym = (void*)GetProcAddress(lib, fctName.c_str());
# else
      void *lib = dlopen(libName.c_str(),RTLD_LOCAL|RTLD_NOW);
      void *sym = dlsym(lib,fctName.c_str());
# endif
      alreadyLoaded[key] = sym;
    }
    return alreadyLoaded[key];
  }

  typedef anari::Device (*CreateDeviceFct)();
    
  anari::Device tryLoadBaked(const std::string &bakedDevName)
  {
    const std::string libName = "libpynari_baked_"+bakedDevName
# ifdef _WIN32
      +".dll"
# elif defined(__APPLE__)
      +".dylib"
#else
      +".so"
#endif
      ;
    // std::cout << "@pynari: trying >>> " << libName << " <<< " << std::endl;
    const std::string symName = "pynari_createDevice_"+bakedDevName;
    // PING(libName);
    // PRINT(symName);
    CreateDeviceFct fct = (CreateDeviceFct)getLoadedLibraryFunction(libName,symName);
    if (!fct) 
      throw std::runtime_error("could not get symbol '"+symName
#ifdef _WIN32
          +"'"
#else
          +"' : "+dlerror()
#endif
      );
    anari::Device dev = fct();
    if (!dev)
      throw std::runtime_error(std::string("could not create baked device"));
    return dev;
  }
#endif
  anari::Device createDevice(std::string libName)
  {
#if PYNARI_BAKED_BACKENDS
    if (libName == "cpu") libName = "barney_cpu";
    if (libName == "gpu") libName = "barney_cuda";
    if (libName == "cuda") libName = "barney_cuda";
    std::vector<std::string> backends = getListOfBakedBackends();
    if (std::find(backends.begin(),backends.end(),libName) != backends.end()) {
      if (logging_enabled())
        std::cout << OWL_TERMINAL_LIGHT_GREEN
                  << "#pynari: loading (baked) backend '" << libName
                  << "'"
                  << OWL_TERMINAL_DEFAULT << std::endl;
      return tryLoadBaked(libName);
    }
    if (libName == "default") {
      for (auto baked : backends) {
        if (logging_enabled())
          std::cout << OWL_TERMINAL_LIGHT_GREEN
                    << "#pynari: loading (baked) backend '" << baked
                    << "'"
                    << OWL_TERMINAL_DEFAULT << std::endl;
        try {
          return tryLoadBaked(baked);
        } catch (const std::exception &e) {
          if (logging_enabled())
            std::cout << OWL_TERMINAL_RED
                      << "error loading baked backend: " << e.what()
                      << OWL_TERMINAL_DEFAULT << std::endl;
        }
      }
    }
#endif
    // this is the "regular" path, relying on (system-installed) anari
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
      = anari::loadLibrary(libName.c_str(), nullptr);
    if (!library)
      throw std::runtime_error("pynari: could not load anari library '"
                               +libName+"'");
    anari::Device _device
      = anari::newDevice(library, "default");
    return _device;
  }

  Context::Context(const std::string &explicitLibName)
  {
    this->device = std::make_shared<Device>(createDevice(explicitLibName));
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
    std::vector<Object::SP> objects;
    for (auto item : list) {
      Object::SP object = item.cast<Object::SP>();
      assert(object);
      objects.push_back(object);
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
  
}
