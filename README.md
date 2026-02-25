# scene_rdl2

Python bindings for the [DreamWorks MoonRay](https://github.com/dreamworksanimation/openmoonray) `scene_rdl2` library, built with pybind11.

## Requirements

- **macOS** (arm64)
- **CMake 3.12+**
- **Python 3.13** (homebrew) — `brew install python@3.13`
- **MoonRay** environment configured — `REZ_MOONRAY_ROOT` and `RDL2_DSO_PATH` must be set (source MoonRay's `setup.sh` or activate the Rez environment)

## Building

```bash
mkdir build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DPython3_EXECUTABLE=/opt/homebrew/bin/python3.13 \
  -DPython3_ROOT_DIR=/opt/homebrew/opt/python@3.13 \
  -DPython3_FIND_STRATEGY=LOCATION
cd .. && cmake --build build --parallel
```

The built module lands at `build/scene_rdl2.cpython-313-darwin.so`.

### VSCode

Open the project folder and use **Cmd+Shift+B** to build (runs CMake configure + build). The integrated terminal automatically has `PYTHONPATH` and `DYLD_LIBRARY_PATH` set correctly.

## Running the tests

```bash
python3.13 -m unittest discover -s tests -t . -v
```

The suite writes fixture files to `tests/fixtures/` (gitignored) on first run.

## Usage

For a complete, working example see **[example/example.py](example/example.py)**.  It
builds a render-ready MoonRay scene from scratch — five primitive geometry types lined
up in a row, each with a coloured material, a key light, and a framed camera — then
exports it as `scene.rdla`.  The output can be rendered directly:

```bash
cd example
python3.13 example.py          # writes scene.rdla
moonray -in scene.rdla -out beauty.exr
```

The snippets below document the individual parts of the API.

```python
import sys
sys.path.insert(0, 'build')
import scene_rdl2 as rdl2
```

### SceneContext & SceneVariables

```python
ctx = rdl2.SceneContext()
ctx.setDsoPath(os.environ['RDL2_DSO_PATH'])
sv  = ctx.getSceneVariables()

# Read an attribute using dict-style access
print(sv['frame'])          # 0.0

# Write attributes — no setup required
sv['image_width']  = 1920
sv['image_height'] = 1080
sv['frame']        = 12.5

# Access a blur (motion-blur) sample at a specific timestep
sv['frame', rdl2.TIMESTEP_END]
```

### Attribute introspection

```python
sc    = ctx.getSceneClass('SceneVariables')
attrs = sc.getAttributes()          # list[Attribute]

for a in attrs[:5]:
    print(a.getName(), rdl2.attributeTypeName(a.getType()),
          'bindable' if a.getFlags() & rdl2.FLAGS_BINDABLE else '')

# Lookup a specific attribute
a = sc.getAttribute('frame')
print(a.getType())           # AttributeType.TYPE_FLOAT
print(a.getDefaultValue())   # 0.0
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
│   ├── EnvMap
│   └── Joint
├── Light
├── Shader
│   └── RootShader
│       ├── Material
│       ├── Displacement
│       └── VolumeShader
├── Map / NormalMap  (via Shader)
├── GeometrySet
│   └── ShadowReceiverSet
├── LightSet
│   └── ShadowSet
├── LightFilter
├── LightFilterSet
├── DisplayFilter
├── Layer
├── RenderOutput
├── Metadata
├── TraceSet
└── UserData
```

`isA()` tests against a `SceneObjectInterface` flag; convenience predicates are also available:

```python
obj.isA(rdl2.INTERFACE_CAMERA)   # via interface bitmask
obj.isCamera()                   # convenience predicate
obj.isGeometry()
obj.isUserData()
# … and so on for every concrete type

cam = obj.asCamera()             # returns Camera* or None
```

### Loading and writing scene files

**ASCII (.rdla)**

```python
# Write
writer = rdl2.AsciiWriter(ctx)
writer.setSkipDefaults(True)     # omit attributes that equal their default
writer.toFile('scene.rdla')
rdla_str = writer.toString()     # write to an in-memory string instead

# Read
reader = rdl2.AsciiReader(ctx)
reader.fromFile('scene.rdla')
reader.fromString(rdla_str)
```

**Binary (.rdlb)**

```python
# Write to file
writer = rdl2.BinaryWriter(ctx)
writer.setSkipDefaults(False)
writer.setDeltaEncoding(True)    # encode only values that changed
writer.toFile('scene.rdlb')

# Write to in-memory bytes (manifest, payload)
manifest, payload = writer.toBytes()

# Read from file
rdl2.BinaryReader(ctx).fromFile('scene.rdlb')

# Read from in-memory bytes
rdl2.BinaryReader(ctx).fromBytes(manifest, payload)

# Inspect manifest without a context
print(rdl2.BinaryReader.showManifest(manifest))
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

Math types can be constructed implicitly from Python lists or tuples wherever a C++ value or const-reference parameter is expected:

```python
sv['fatal_color'] = [1.0, 0.0, 0.0]          # list → Rgb
sv['fatal_color'] = (0.5, 0.5, 0.5)          # tuple → Rgb

geo.setNodeXform([[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]])  # list-of-lists → Mat4d
```

### UserData

`UserData` objects carry typed key/value channels used to pass primitive attributes (per-vertex colours, UVs, etc.) through the rdl2 context.

```python
ctx.setDsoPath(os.environ['RDL2_DSO_PATH'])
ctx.loadAllSceneClasses()

ud = ctx.createSceneObject('UserData', '/my/primvars').asUserData()

ud.setRate(rdl2.UserData.Rate.VERTEX)
ud.setFloatData('Cd_r', [0.1, 0.2, 0.3])           # single timestep
ud.setVec3fData('N', [rdl2.Vec3f(0,1,0)],           # dual timestep (blur)
                     [rdl2.Vec3f(0,0,1)])

print(ud.getFloatKey())         # 'Cd_r'
print(list(ud.getFloatValues()))  # [0.1, 0.2, 0.3]
print(ud.getRate())             # UserData.Rate.VERTEX
```

Supported channels and their blur support:

| Channel | Type | Blur (dual-timestep) |
|---------|------|----------------------|
| Bool    | `bool` | No |
| Int     | `int` | No |
| String  | `str` | No |
| Float   | `float` | Yes |
| Color   | `Rgb` | Yes |
| Vec2f   | `Vec2f` | Yes |
| Vec3f   | `Vec3f` | Yes |
| Mat4f   | `Mat4f` | Yes |

Rate values: `UserData.Rate.AUTO`, `CONSTANT`, `PART`, `UNIFORM`, `VERTEX`, `VARYING`, `FACE_VARYING`.

### Metadata

```python
md = ctx.createSceneObject('Metadata', '/exr/meta').asMetadata()
md.setAttributes(['title', 'date'],
                 ['string', 'string'],
                 ['My Render', '2026-01-01'])

print(md.getAttributeNames())   # ['title', 'date']
print(md.getAttributeValues())  # ['My Render', '2026-01-01']
```

### TraceSet

```python
ts  = ctx.createSceneObject('TraceSet', '/traceset').asTraceSet()
geo = ctx.createSceneObject('BoxGeometry', '/box').asGeometry()

aid = ts.assign(geo, 'part_A')   # returns assignment ID

geom, part = ts.lookupGeomAndPart(aid)
ids = ts.getAssignmentIds(geo)       # list[int]
```

## API reference

| Category | Types / symbols |
|---|---|
| **Math** | `Rgb` `Rgba` `Vec2f` `Vec2d` `Vec3f` `Vec3d` `Vec4f` `Vec4d` `Mat4f` `Mat4d` |
| **Enums** | `AttributeType` `AttributeFlags` `AttributeTimestep` `SceneObjectInterface` `MotionBlurType` `PixelFilterType` `TaskDistributionType` `VolumeOverlapMode` `ShadowTerminatorFix` `TextureFilterType` `GeometrySideType` `UserData.Rate` |
| **Scene** | `SceneContext` `SceneClass` `SceneObject` `SceneVariables` |
| **Nodes** | `Node` `Camera` `Geometry` `EnvMap` `Joint` |
| **Light** | `Light` |
| **Shaders** | `Shader` `RootShader` `Material` `Displacement` `VolumeShader` `Map` `NormalMap` |
| **Collections** | `GeometrySet` `ShadowReceiverSet` `LightSet` `ShadowSet` `LightFilter` `LightFilterSet` `DisplayFilter` `Layer` `LayerAssignment` |
| **Data / metadata** | `UserData` `Metadata` `TraceSet` |
| **Output** | `RenderOutput` |
| **I/O** | `AsciiReader` `AsciiWriter` `BinaryReader` `BinaryWriter` |
| **Free functions** | `attributeTypeName(AttributeType) -> str` |

### SceneObject dict-style attribute access

All `SceneObject` subclasses (including `SceneVariables`) support `[]` for attribute read/write:

```python
obj['attr_name']                            # get at TIMESTEP_BEGIN
obj['attr_name', rdl2.TIMESTEP_END]         # get at specific timestep
obj['attr_name'] = value                    # set at TIMESTEP_BEGIN
obj['attr_name', rdl2.TIMESTEP_END] = value # set at specific timestep
'attr_name' in obj                          # True if attribute exists
```

## Known limitation

The `SceneContext` destructor calls into MoonRay's DSO-loading infrastructure, which aborts when run outside the full MoonRay pipeline. Normal script usage (create context, use it, exit) is unaffected because Python's interpreter shutdown triggers the abort after all user code has completed.

## License

MIT — see [LICENSE.md](LICENSE.md).

The underlying [DreamWorks MoonRay](https://github.com/dreamworksanimation/openmoonray) libraries (`libscene_rdl2.dylib` and friends) are licensed under Apache 2.0.
