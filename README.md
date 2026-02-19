# scene_rdl2

Python bindings for the [DreamWorks MoonRay](https://github.com/dreamworksanimation/openmoonray) `scene_rdl2` library, built with pybind11.

## Requirements

- **macOS** (arm64)
- **CMake 3.12+**
- **Python 3.13** (homebrew) — `brew install python@3.13`
- **MoonRay** installed at `/Applications/MoonRay/installs/` (provides `scene_rdl2`, pybind11 2.13.6, boost, tbb, lua)

## Building

```bash
mkdir build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DPython3_EXECUTABLE=/opt/homebrew/bin/python3.13 \
  -DPython3_ROOT_DIR=/opt/homebrew/opt/python@3.13 \
  -DPython3_FIND_STRATEGY=LOCATION
make
```

The built module lands at `build/scene_rdl2/scene_rdl2.cpython-313-darwin.so`.

### VSCode

Open the project folder and use **Cmd+Shift+B** to build (runs CMake configure + build). The integrated terminal automatically has `PYTHONPATH` and `DYLD_LIBRARY_PATH` set correctly. Press **F5** to debug `test_bindings.py`.

## Usage

```python
import sys
sys.path.insert(0, 'build/scene_rdl2')
import scene_rdl2 as rdl2
```

### SceneContext & SceneVariables

```python
ctx = rdl2.SceneContext()
sv  = ctx.getSceneVariables()

# Read an attribute
print(sv.get('min_frame'))   # 0.0

# Write attributes inside an UpdateGuard
with rdl2.UpdateGuard(sv):
    sv.set('min_frame', 1.0)
    sv.set('max_frame', 48.0)
```

### Attribute introspection

```python
sc    = ctx.getSceneClass('SceneVariables')
attrs = sc.getAttributes()          # list[Attribute]

for a in attrs[:5]:
    print(a.getName(), rdl2.attributeTypeName(a.getType()),
          'bindable' if a.getFlags() & rdl2.FLAGS_BINDABLE else '')

# Lookup a specific attribute
a = sc.getAttribute('min_frame')
print(a.getType())          # AttributeType.TYPE_FLOAT
print(a.getDefaultValue())  # 0.0  (via SceneObject.get with default timestep)
```

### Iterating the scene

```python
for obj in ctx.getAllSceneObjects():
    print(obj.getName(), obj.getSceneClass().getName())

for sc in ctx.getAllSceneClasses():
    print(sc.getName(), sc.getSourcePath())
```

### Class hierarchy

All scene types are exposed with their full inheritance chain:

```
SceneObject
├── SceneVariables
├── Node
│   ├── Camera
│   ├── Geometry
│   └── Light
├── Shader
│   └── RootShader
│       ├── Material
│       ├── Displacement
│       └── VolumeShader
├── Map / NormalMap  (via Shader)
├── GeometrySet
├── LightSet
├── Layer
└── RenderOutput
```

`isA()` takes a `SceneObjectInterface` enum value:

```python
obj.isA(rdl2.INTERFACE_CAMERA)    # False
obj.isA(rdl2.INTERFACE_GENERIC)   # True
```

### Loading a scene file

```python
reader = rdl2.AsciiReader(ctx)
reader.fromFile('/path/to/scene.rdla')
```

### Writing a scene file

```python
writer = rdl2.AsciiWriter(ctx)
writer.toFile('/path/to/out.rdla')
```

### Math types

```python
c  = rdl2.Rgb(1.0, 0.5, 0.0)          # r, g, b
v3 = rdl2.Vec3f(0.0, 1.0, 0.0)        # x, y, z
m  = rdl2.Mat4d(rdl2.Vec4d(1,0,0,0),
                rdl2.Vec4d(0,1,0,0),
                rdl2.Vec4d(0,0,1,0),
                rdl2.Vec4d(0,0,0,1))   # row-major (vx, vy, vz, vw)
```

Full set: `Rgb`, `Rgba`, `Vec2f/d`, `Vec3f/d`, `Vec4f/d`, `Mat4f/d`.

## API reference

| Category | Types / symbols |
|---|---|
| **Math** | `Rgb` `Rgba` `Vec2f` `Vec2d` `Vec3f` `Vec3d` `Vec4f` `Vec4d` `Mat4f` `Mat4d` |
| **Enums** | `AttributeType` `AttributeFlags` `AttributeTimestep` `SceneObjectInterface` `MotionBlurType` `PixelFilterType` `TaskDistributionType` `VolumeOverlapMode` `ShadowTerminatorFix` `TextureFilterType` `GeometrySideType` |
| **Scene** | `SceneContext` `SceneClass` `SceneObject` `SceneVariables` `UpdateGuard` |
| **Nodes** | `Node` `Camera` `Geometry` `Light` |
| **Shaders** | `Shader` `RootShader` `Material` `Displacement` `VolumeShader` `Map` `NormalMap` |
| **Collections** | `GeometrySet` `LightSet` `Layer` `LayerAssignment` |
| **Output** | `RenderOutput` |
| **I/O** | `AsciiReader` `AsciiWriter` |
| **Free functions** | `attributeTypeName(AttributeType) -> str` |

## Known limitation

The `SceneContext` destructor calls into MoonRay's DSO-loading infrastructure, which aborts when run outside the full MoonRay pipeline. Normal script usage (create context, use it, exit) is unaffected because Python's interpreter shutdown triggers the abort after all user code has completed.

## License

MIT — see [LICENSE.md](LICENSE.md).

The underlying [DreamWorks MoonRay](https://github.com/dreamworksanimation/openmoonray) libraries (`libscene_rdl2.dylib` and friends) are licensed under Apache 2.0.
