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

#include "pynari/Device.h"
#include <helium/helium_math.h>
#include <anari/anari_cpp.hpp>

namespace pynari {

  struct Device;
  
  std::string to_string(anari::DataType type);

  /*! base class for any anari object type such as a light, a
      material, renderg,e tcpp */
  struct Object : public std::enable_shared_from_this<Object> {
    typedef std::shared_ptr<Object> SP;
    
    Object(Device::SP device);
    
    virtual ~Object();
    virtual std::string toString() const = 0;

    virtual ANARIDataType anariType() const = 0;
    
    void commit();

    void set_object(const char *name, int type,
                    const Object::SP &object);
    void set_object_notype(const char *name, 
                    const Object::SP &object);
    void set_string(const char *name, int type, 
                    const std::string &stringValue);
    void set_string_notype(const char *name, //int type,
                           const std::string &stringValue);

    /*! DEPRECATED for compatibility only! */
    void setArray_list(const char *name, int type, 
                       const py::list &list);
    /*! DEPRECATED for compatibility only! */
    void setArray_np(const char *name, int type, 
                     const py::buffer &buffer);
    
    
    void setArray1D_list(const char *name, int type, 
                         const py::list &list);
    void setArray1D_np(const char *name, int type, 
                       const py::buffer &buffer);
    // void setArray2D_list(const char *name, int type, 
    //                      const py::list &list);
    void setArray2D_np(const char *name, int type, 
                       const py::buffer &buffer);
    // void setArray3D_list(const char *name, int type, 
    //                      const py::list &list);
    void setArray3D_np(const char *name, int type, 
                       const py::buffer &buffer);
    void set_box1(const char *name, int type,
                  const helium::box1 b);
    void set_float(const char *name, int type,
                   float v);
    void set_float2(const char *name, int type, 
                    const std::tuple<float,float> &v);
    void set_float3(const char *name, int type, 
                    const std::tuple<float,float,float> &v);
    void set_float4(const char *name, int type, 
                    const std::tuple<float,float,float,float> &v);
    void set_float12(const char *name, int type, 
                     const std::tuple<
                     float,float,float,float,
                     float,float,float,float,
                     float,float,float,float> &v);
    void set_float16(const char *name, int type, 
                     const std::tuple<
                     float,float,float,float,
                     float,float,float,float,
                     float,float,float,float,
                     float,float,float,float> &v);
    void set_float_vec(const char *name, int type, 
                       const std::vector<float> &v);
    void set_uint(const char *name, int type,
                  uint v);
    void set_ulong(const char *name,
                   int type,
                   uint64_t v);
    void set_uint2(const char *name, int type, 
                   const std::tuple<uint,uint> &v);
    void set_uint3(const char *name, int type, 
                   const std::tuple<uint,uint,uint> &v);
    void set_uint4(const char *name, int type, 
                   const std::tuple<uint,uint,uint,uint> &v);
    void set_uint_vec(const char *name, int type, 
                      const std::vector<uint> &v);
    void assertThisObjectIsValid();

    /*! this is the function that gets called when the *app* calls
        release on an object handle */
    void releaseFromApp();

    /*! tells this object to relase all its internal data; most
        specifically its anari handle, and its refcount on the
        object. The pynari wrapper object itself may still remain
        alive until python's garbage collection decides to kill it,
        but it will from here on out be an empty shell that contains
        no data and holds to references that may keep anything else
        alive.

        Note this can possibly get called from three different
        locations: 1) when the app calls release() on the object (as
        it should); at which point that object should also release its
        internal data. 2) if the app forgot to relase some objects,
        but does release the device - this isn't 'clean' usage of the
        anari api, but in this case we'll have the device
        force-release all still-open objects, which will call this on
        each such object. 3) if the app didn't release anything, and
        python garbage collection kicks in to kill the object - this
        isn't clean _at all_, but we'll still release ni a clean
        order */
    void releaseInternalDataAndDeregisterOnDevice();

    /*! for objects that die while still having an active device
        handle (ie, it dies in python garbage collectio without haivng
        been formally release()'d by the app); this will print some
        debug info and force-release the specific object. In theory we
        could do this in ::~Object(), but this would require calling
        the virtual toString(), which at the point in time when parent
        descrutor gets called is no longer valid. */
    void fallbackDestructAndWarn(const std::string &objectDescription);
    
    Device::SP    device;
    anari::Object handle = {};
  };
  
}
