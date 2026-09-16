// ======================================================================== //
// Copyright 2024-2026 Ingo Wald                                            //
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

#include "pynari/Device.h"
#include "pynari/Object.h"
#include "pynari/Context.h"

namespace pynari {

  Device::Device(anari::Device handle, Context *context)
    : handle(handle), context(context)
  {}

  Device::~Device()
  {
    // pynari wrapper object is dying; since this is a shared_ptr held
    // by all objects _and_ by python itself this can, in theory only
    // happen if all objects have already been released _and_ python's
    // garbage collection has killed the app's pynari device, too.
    assert(listOfAllObjectsCreatedOnThisDevice.empty()
           &&
           "sanity check - thanks to refcounting device should "
           "not ever die before all objects have died");

    // if the user behaved the way he should he should have released
    // the device at some point in time. if so the 'handle' should
    // already be null, and all objects should be released as well. in
    // that case it's the python runtime's shared_ptr to this device
    // that's dying last, and the actual anari device has already been
    // released long before that happens. If this is NOT the case,
    // this can only mean that the user forgot to call release() on
    // the device, and left it for python's GC - this isn't the way
    // it's intended, but let's clean up, anyway. Even _if_ that
    // happens, though, this device object can only get deleted after
    // all objects have already been deleted (because created objects
    // store a shared-Ptr to this device), so even if python GC
    // releases its objects i the 'wrong' order we should still die
    // last - so all we have to do is release the dangling anari
    // handle.
    if (handle) {
      if (context->verbose) {
        std::cout << "#pynari: warning - pynari device is dying without" << std::endl;
        std::cout << "#pynari: having been formally release()'d by the app." << std::endl;
        std::cout << "#pynari: I'll properly release everything in the" << std::endl;
        std::cout << "#pynari: proper order, but according to anari spec" << std::endl;
        std::cout << "#pynari: the app _should_ have formally released" << std::endl;
        std::cout << "#pynari: the device instead of just leaving it to" << std::endl;
        std::cout << "#pynari: python's garbage collection." << std::endl;
      }
      // iw: mind this is only the 'fallback' release if the app
      // didn't properly release the device. the 'real'/intended
      // release is in Device::releaseFromApp().
      anari::release(this->handle,this->handle);
      handle = {};
    }
  }
  
    /*! this gets called if - and only if - the python app calls
        anariDevice.release(). If so this will go over all the pynari
        object still alive at this moment, and force-release their
        anari handles (the pynari wrapper objects themselves are
        refcoutned by python and may thus stay alive for a while
        longer) */
  void Device::releaseFromApp()
  {
    // this fct can only get called by an explicit release from the
    // app. the only way the anari handle for this device could be
    // null is if this same fct has been called before, which would be
    // an error
    if (handle == 0)
      throw std::runtime_error("#pynari: python app release()'d the same device twice.");

    // note we know the pynari device is still alive, so python must
    // have at least one reference to this object - we know it cannot
    // get deleted whiel running this function, even if some of the
    // objects we store will die and decrease refcount.

    if (!listOfAllObjectsCreatedOnThisDevice.empty()) {
      if (context->verbose) {
        std::cout << "#pynari: warning - user release()'d device, while" << std::endl;
        std::cout << "#pynari: there are still un-released objects that" << std::endl;
        std::cout << "#pynari: were created on this device." << std::endl;
        std::cout << "#pynari: I'll force-release those here, but the" << std::endl;
        std::cout << "#pynari: cleaner way would have been to release all" << std::endl;
        std::cout << "#pynari: objects before releasing the device." << std::endl;
        std::cout << "#pynari: for your info, found these still-alive objects:" << std::endl;
        for (Object *obj : listOfAllObjectsCreatedOnThisDevice)
          std::cout << " - " << obj->toString() << std::endl;
      }

      // let's create a copy of the list of still-alive objects, so we
      // can iterate over these objects while the iterated-over
      // objects remove themselves from that list.
      std::set<Object *> copyOfCurrentObjects
        = listOfAllObjectsCreatedOnThisDevice;
      // now that we have this, tell these objects to relase their
      // internal stuff (including their anari-handle _and_ their
      // refcount to this device). Note we know that neither the
      // object _nor_ outselves can get deleted in any of these
      // operations: ours truly will lose refcounts from those
      // objects, but the app must have at least one refcount to have
      // called 'release()' through (so device cannot die); and the
      // objects, too, cannot have garbage collected, yet, because
      // otherwise they'd have removed themselves from this list
      // already.
      for (Object *obj : copyOfCurrentObjects)
        obj->releaseInternalDataAndDeregisterOnDevice();
    }
    
    // and finally, release the device itself
    if (context->verbose)
      std::cout << "#pynari: releasing anari device" << std::endl;
    anari::release(this->handle,this->handle);
    handle = nullptr;
  }
  
}
