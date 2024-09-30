# pynari - a Python Interface for ANARI

# PyNari equivalents of ANARI Data Types

## Low-level types

ANARI has a lot of data types of the form `ANARI_FLOAT32`,
`ANARI_FLOAT32_VEC3`, `ANARI_ARRAY1D`, etc. In `pynari`, any such type
`ANARI_SOMETHING` is expressed as `anari.SOMETHING; i.e.,
`ANARI_FLOAT32_VEC3` becomes `anari.FLOAT32_VEC3`. Additionally, for
typical scalar/vector types we also allow numpi or CUDA style naming,
i.e., instead of `anari.FLOAT32_VEC3` you can also use `anari.float3`.


## Scalar and Vector types

ANARI Vector types can be expressed as either python lists, or python
tuples, or numpy arrays. Eg, all of the following are valid:

- `matte.setParameter('color',anari.float3,(.5,.5,.5))`
- `matte.setParameter('color',anari.float3,[.5,.5,.5])`
- `matte.setParameter('color',nm.array([.5,.5,.5],dtype=np.float32)`

## Array types

For all *numerical* array types (ie, arrays of scalars, or arrays of
vector types) we use numpy arrays of the underlying scalar types:

```
vertex = np.array([
  -1.0, -1.0, 3.0,
  -1.0,  1.0, 3.0,
   1.0, -1.0, 3.0,
   0.1,  0.1, 0.3
], dtype = np.float32)
array = device.newArray(anari.FLOAT32_VEC3,vertex)
```

Note that arrays of vector types and arrays of scalar types will
automatically convert; e.g., in the example just given a array of 12
floats is implicitly the same as a array of 4 float3 vectors.

FOr all arrays of ANARI object types (e.g., an array of lights, an
array of surfaces, etc, simply use a python list:

```
world.setParameterArray('surface', anari.SURFACE, [ surface ])
```

# PyNari equivalents of ANARI API Functions

In anari, API functions are C99 style functions, but almost always
 called "on" either a specific ANARI device (e.g.,
 `anariNewRenderer(device,..)`, `anariNewLight(device,...)`, etc), or
 on a specific ANARI object that was previously created (e.g.,
 `anariCommitParameters(device,object)`,
 `anariSetParameter(device,object,...)`, etc.
 
 In pynari, we use a more pythonic/object-oriented way and instead map
 each such function to a method on either a device, or an
 object. I.e., functions that operate on a device become methods of
 that device:
 
 - `anariNewRenderer(device,..)` becomes `device.newRenderer(...)`
 - `anariNewLight(device,..)` becomes `device.newLight(...)`
 - ...
 
 Similarly, methods that operate on a given object become methods
 of that object:

- `anariCommitParameters(device,object,...))` becomes `object.commitParameters(...)`
- `anariSetParameter(device,object,...))` becomes `object.setParameter(...)`
- `anariRenderFrame(device,frame)` becomes `frame.renderFrame()`
- ...

# Biggest Differences to the ANARI C API

As described above, almost all functions in the ANARI C API have very
clear and direct equivalents in `pynari`, and only change in syntax.

There are, however, a few cases where we decided to slightly simplify
some of the more low-level concepts of the ANARI API.

## Device Creation

In ANARI, device creation is a slightly involved operation, also
requires the loading of libraries. etc.

In pynari we decided to simplify that to a simple `device =
anari.newDevice('libName')`, where `libName` is the name of the ANARI
library to be loaded for creating this device. In particular,
`anari.newDevice('default')` will simply use the system's default
ANARI library and create a device from that. Note that `default` will
still respect the `ANARI_LIBRARY` environment variable to specify
which library to use.

## Rendering and Frame Buffer mapping

ANARI allows for asynchronous frame rendering, and thus requires to
both 'start' and 'wait for' frames to finish. In `pynari`, this gets
simplified to simply calling `frame.render()`, which renders a frame
and waits for completion. 

Once a frame is rendered, ANARI allows for "mapping" the resulting
buffers, and processing them as plain C99 arrays/pointers. In pynari,
we instead return the frame buffer contents as numpi arrays of the
specific types. In particular:

- a frame buffer of format `anari.FLOAT32_VEC4` will be returned as
 `np.array` of shape `width,height,4` and type `np.float32`.

- a frame buffer of format `anari.UFIXED8_VEC4` will be returned as
 `np.array` of shape `width,height,1` and type `np.int32`.

- a depth buffer of format `anari.FLOAT32` will be returned as
 `np.array` of shape `width,height,1` and type `np.float32`.

Frame buffers do not get mapped and thus do not require unmapping,
either.  Different channels are read using
`frame.get('channel.color')` and `frame.get('channel.depth')`.

