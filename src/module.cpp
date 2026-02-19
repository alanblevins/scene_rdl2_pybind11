// Copyright 2023-2024 DreamWorks Animation LLC
// SPDX-License-Identifier: Apache-2.0
//
// Comprehensive Python bindings for the scene_rdl2 library using pybind11.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>

// scene_rdl2 headers - order matters for forward declarations
#include <scene_rdl2/scene/rdl2/Types.h>
#include <scene_rdl2/scene/rdl2/Attribute.h>
#include <scene_rdl2/scene/rdl2/AttributeKey.h>
#include <scene_rdl2/scene/rdl2/SceneClass.h>
#include <scene_rdl2/scene/rdl2/SceneObject.h>
#include <scene_rdl2/scene/rdl2/SceneContext.h>
#include <scene_rdl2/scene/rdl2/SceneVariables.h>
#include <scene_rdl2/scene/rdl2/Node.h>
#include <scene_rdl2/scene/rdl2/Camera.h>
#include <scene_rdl2/scene/rdl2/Geometry.h>
#include <scene_rdl2/scene/rdl2/GeometrySet.h>
#include <scene_rdl2/scene/rdl2/Light.h>
#include <scene_rdl2/scene/rdl2/LightSet.h>
#include <scene_rdl2/scene/rdl2/LightFilterSet.h>
#include <scene_rdl2/scene/rdl2/ShadowSet.h>
#include <scene_rdl2/scene/rdl2/ShadowReceiverSet.h>
#include <scene_rdl2/scene/rdl2/Layer.h>
#include <scene_rdl2/scene/rdl2/Material.h>
#include <scene_rdl2/scene/rdl2/Displacement.h>
#include <scene_rdl2/scene/rdl2/VolumeShader.h>
#include <scene_rdl2/scene/rdl2/Map.h>
#include <scene_rdl2/scene/rdl2/NormalMap.h>
#include <scene_rdl2/scene/rdl2/RootShader.h>
#include <scene_rdl2/scene/rdl2/Shader.h>
#include <scene_rdl2/scene/rdl2/RenderOutput.h>
#include <scene_rdl2/scene/rdl2/UserData.h>
#include <scene_rdl2/scene/rdl2/AsciiReader.h>
#include <scene_rdl2/scene/rdl2/AsciiWriter.h>

// SceneObject and all its subclasses declare their copy constructors as
// private and leave them undefined (the C++03 non-copyable idiom, without
// = delete).  On some compilers/libc++ versions __is_constructible() returns
// true for these types anyway (it doesn't check access), which causes
// pybind11 to emit a copy-constructor wrapper that references the undefined
// symbol.  We specialise pybind11::detail::is_copy_constructible to false for
// the entire hierarchy so that no such code is generated.
namespace pybind11 { namespace detail {
// GeometrySet, LightSet, etc. inherit SceneObject whose copy constructor is
// private+undefined (C++03 non-copyable idiom, not `= delete`).  Derived
// classes get a synthesised move constructor that silently falls back to the
// copy constructor because SceneObject has no move constructor.  The compiler
// intrinsic __is_constructible() doesn't check access so it reports these
// types as both copy- and move-constructible, causing pybind11 to emit
// make_copy_constructor / make_move_constructor lambdas that reference the
// undefined symbol.  Specialise all three traits here to suppress this.
#define MARK_NON_COPYABLE(T) \
    template <> struct is_copy_constructible<T> : std::false_type {}; \
    template <> struct is_copy_assignable<T>     : std::false_type {}; \
    template <> struct is_move_constructible<T>  : std::false_type {};
MARK_NON_COPYABLE(scene_rdl2::rdl2::SceneObject)
MARK_NON_COPYABLE(scene_rdl2::rdl2::SceneVariables)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Node)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Camera)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Geometry)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Light)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Shader)
MARK_NON_COPYABLE(scene_rdl2::rdl2::RootShader)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Material)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Displacement)
MARK_NON_COPYABLE(scene_rdl2::rdl2::VolumeShader)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Map)
MARK_NON_COPYABLE(scene_rdl2::rdl2::NormalMap)
MARK_NON_COPYABLE(scene_rdl2::rdl2::GeometrySet)
MARK_NON_COPYABLE(scene_rdl2::rdl2::LightSet)
MARK_NON_COPYABLE(scene_rdl2::rdl2::LightFilterSet)
MARK_NON_COPYABLE(scene_rdl2::rdl2::ShadowSet)
MARK_NON_COPYABLE(scene_rdl2::rdl2::ShadowReceiverSet)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Layer)
MARK_NON_COPYABLE(scene_rdl2::rdl2::RenderOutput)
#undef MARK_NON_COPYABLE
}} // namespace pybind11::detail

namespace py = pybind11;
namespace rdl2 = scene_rdl2::rdl2;
namespace math = scene_rdl2::math;

// ---------------------------------------------------------------------------
// Helper: dynamic attribute getter by name
// ---------------------------------------------------------------------------
static py::object getAttrByName(
    const rdl2::SceneObject& self,
    const std::string& name,
    rdl2::AttributeTimestep ts = rdl2::TIMESTEP_BEGIN)
{
    const rdl2::Attribute* attr = self.getSceneClass().getAttribute(name);
    switch (attr->getType()) {
        case rdl2::TYPE_BOOL:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Bool>(*attr), ts));
        case rdl2::TYPE_INT:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Int>(*attr), ts));
        case rdl2::TYPE_LONG:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Long>(*attr), ts));
        case rdl2::TYPE_FLOAT:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Float>(*attr), ts));
        case rdl2::TYPE_DOUBLE:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Double>(*attr), ts));
        case rdl2::TYPE_STRING:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::String>(*attr), ts));
        case rdl2::TYPE_RGB:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Rgb>(*attr), ts));
        case rdl2::TYPE_RGBA:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Rgba>(*attr), ts));
        case rdl2::TYPE_VEC2F:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec2f>(*attr), ts));
        case rdl2::TYPE_VEC2D:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec2d>(*attr), ts));
        case rdl2::TYPE_VEC3F:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec3f>(*attr), ts));
        case rdl2::TYPE_VEC3D:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec3d>(*attr), ts));
        case rdl2::TYPE_VEC4F:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec4f>(*attr), ts));
        case rdl2::TYPE_VEC4D:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec4d>(*attr), ts));
        case rdl2::TYPE_MAT4F:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Mat4f>(*attr), ts));
        case rdl2::TYPE_MAT4D:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Mat4d>(*attr), ts));
        case rdl2::TYPE_SCENE_OBJECT:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::SceneObject*>(*attr)));
        case rdl2::TYPE_BOOL_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::BoolVector>(*attr)));
        case rdl2::TYPE_INT_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::IntVector>(*attr)));
        case rdl2::TYPE_LONG_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::LongVector>(*attr)));
        case rdl2::TYPE_FLOAT_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::FloatVector>(*attr)));
        case rdl2::TYPE_DOUBLE_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::DoubleVector>(*attr)));
        case rdl2::TYPE_STRING_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::StringVector>(*attr)));
        case rdl2::TYPE_RGB_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::RgbVector>(*attr)));
        case rdl2::TYPE_RGBA_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::RgbaVector>(*attr)));
        case rdl2::TYPE_VEC2F_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec2fVector>(*attr)));
        case rdl2::TYPE_VEC2D_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec2dVector>(*attr)));
        case rdl2::TYPE_VEC3F_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec3fVector>(*attr)));
        case rdl2::TYPE_VEC3D_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec3dVector>(*attr)));
        case rdl2::TYPE_VEC4F_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec4fVector>(*attr)));
        case rdl2::TYPE_VEC4D_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec4dVector>(*attr)));
        case rdl2::TYPE_MAT4F_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Mat4fVector>(*attr)));
        case rdl2::TYPE_MAT4D_VECTOR:
            return py::cast(self.get(rdl2::AttributeKey<rdl2::Mat4dVector>(*attr)));
        case rdl2::TYPE_SCENE_OBJECT_VECTOR: {
            const rdl2::SceneObjectVector& v =
                self.get(rdl2::AttributeKey<rdl2::SceneObjectVector>(*attr));
            return py::cast(v);
        }
        case rdl2::TYPE_SCENE_OBJECT_INDEXABLE: {
            const rdl2::SceneObjectIndexable& v =
                self.get(rdl2::AttributeKey<rdl2::SceneObjectIndexable>(*attr));
            // Convert to a list of SceneObject*
            std::vector<rdl2::SceneObject*> result(v.begin(), v.end());
            return py::cast(result);
        }
        default:
            throw std::runtime_error("Unknown or unsupported attribute type for get()");
    }
}

// ---------------------------------------------------------------------------
// Helper: dynamic attribute setter by name
// ---------------------------------------------------------------------------
static void setAttrByName(
    rdl2::SceneObject& self,
    const std::string& name,
    py::object value,
    rdl2::AttributeTimestep ts = rdl2::TIMESTEP_BEGIN)
{
    const rdl2::Attribute* attr = self.getSceneClass().getAttribute(name);
    switch (attr->getType()) {
        case rdl2::TYPE_BOOL:
            self.set(rdl2::AttributeKey<rdl2::Bool>(*attr), value.cast<rdl2::Bool>(), ts);
            break;
        case rdl2::TYPE_INT:
            self.set(rdl2::AttributeKey<rdl2::Int>(*attr), value.cast<rdl2::Int>(), ts);
            break;
        case rdl2::TYPE_LONG:
            self.set(rdl2::AttributeKey<rdl2::Long>(*attr), value.cast<rdl2::Long>(), ts);
            break;
        case rdl2::TYPE_FLOAT:
            self.set(rdl2::AttributeKey<rdl2::Float>(*attr), value.cast<rdl2::Float>(), ts);
            break;
        case rdl2::TYPE_DOUBLE:
            self.set(rdl2::AttributeKey<rdl2::Double>(*attr), value.cast<rdl2::Double>(), ts);
            break;
        case rdl2::TYPE_STRING:
            self.set(rdl2::AttributeKey<rdl2::String>(*attr), value.cast<rdl2::String>(), ts);
            break;
        case rdl2::TYPE_RGB:
            self.set(rdl2::AttributeKey<rdl2::Rgb>(*attr), value.cast<rdl2::Rgb>(), ts);
            break;
        case rdl2::TYPE_RGBA:
            self.set(rdl2::AttributeKey<rdl2::Rgba>(*attr), value.cast<rdl2::Rgba>(), ts);
            break;
        case rdl2::TYPE_VEC2F:
            self.set(rdl2::AttributeKey<rdl2::Vec2f>(*attr), value.cast<rdl2::Vec2f>(), ts);
            break;
        case rdl2::TYPE_VEC2D:
            self.set(rdl2::AttributeKey<rdl2::Vec2d>(*attr), value.cast<rdl2::Vec2d>(), ts);
            break;
        case rdl2::TYPE_VEC3F:
            self.set(rdl2::AttributeKey<rdl2::Vec3f>(*attr), value.cast<rdl2::Vec3f>(), ts);
            break;
        case rdl2::TYPE_VEC3D:
            self.set(rdl2::AttributeKey<rdl2::Vec3d>(*attr), value.cast<rdl2::Vec3d>(), ts);
            break;
        case rdl2::TYPE_VEC4F:
            self.set(rdl2::AttributeKey<rdl2::Vec4f>(*attr), value.cast<rdl2::Vec4f>(), ts);
            break;
        case rdl2::TYPE_VEC4D:
            self.set(rdl2::AttributeKey<rdl2::Vec4d>(*attr), value.cast<rdl2::Vec4d>(), ts);
            break;
        case rdl2::TYPE_MAT4F:
            self.set(rdl2::AttributeKey<rdl2::Mat4f>(*attr), value.cast<rdl2::Mat4f>(), ts);
            break;
        case rdl2::TYPE_MAT4D:
            self.set(rdl2::AttributeKey<rdl2::Mat4d>(*attr), value.cast<rdl2::Mat4d>(), ts);
            break;
        case rdl2::TYPE_SCENE_OBJECT:
            self.set(rdl2::AttributeKey<rdl2::SceneObject*>(*attr),
                     value.cast<rdl2::SceneObject*>());
            break;
        case rdl2::TYPE_BOOL_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::BoolVector>(*attr),
                     value.cast<rdl2::BoolVector>(), ts);
            break;
        case rdl2::TYPE_INT_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::IntVector>(*attr),
                     value.cast<rdl2::IntVector>(), ts);
            break;
        case rdl2::TYPE_LONG_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::LongVector>(*attr),
                     value.cast<rdl2::LongVector>(), ts);
            break;
        case rdl2::TYPE_FLOAT_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::FloatVector>(*attr),
                     value.cast<rdl2::FloatVector>(), ts);
            break;
        case rdl2::TYPE_DOUBLE_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::DoubleVector>(*attr),
                     value.cast<rdl2::DoubleVector>(), ts);
            break;
        case rdl2::TYPE_STRING_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::StringVector>(*attr),
                     value.cast<rdl2::StringVector>(), ts);
            break;
        case rdl2::TYPE_RGB_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::RgbVector>(*attr),
                     value.cast<rdl2::RgbVector>(), ts);
            break;
        case rdl2::TYPE_RGBA_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::RgbaVector>(*attr),
                     value.cast<rdl2::RgbaVector>(), ts);
            break;
        case rdl2::TYPE_VEC2F_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Vec2fVector>(*attr),
                     value.cast<rdl2::Vec2fVector>(), ts);
            break;
        case rdl2::TYPE_VEC2D_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Vec2dVector>(*attr),
                     value.cast<rdl2::Vec2dVector>(), ts);
            break;
        case rdl2::TYPE_VEC3F_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Vec3fVector>(*attr),
                     value.cast<rdl2::Vec3fVector>(), ts);
            break;
        case rdl2::TYPE_VEC3D_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Vec3dVector>(*attr),
                     value.cast<rdl2::Vec3dVector>(), ts);
            break;
        case rdl2::TYPE_VEC4F_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Vec4fVector>(*attr),
                     value.cast<rdl2::Vec4fVector>(), ts);
            break;
        case rdl2::TYPE_VEC4D_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Vec4dVector>(*attr),
                     value.cast<rdl2::Vec4dVector>(), ts);
            break;
        case rdl2::TYPE_MAT4F_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Mat4fVector>(*attr),
                     value.cast<rdl2::Mat4fVector>(), ts);
            break;
        case rdl2::TYPE_MAT4D_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Mat4dVector>(*attr),
                     value.cast<rdl2::Mat4dVector>(), ts);
            break;
        case rdl2::TYPE_SCENE_OBJECT_VECTOR: {
            rdl2::SceneObjectVector v = value.cast<rdl2::SceneObjectVector>();
            self.set(rdl2::AttributeKey<rdl2::SceneObjectVector>(*attr), v, ts);
            break;
        }
        case rdl2::TYPE_SCENE_OBJECT_INDEXABLE: {
            // Accept a list of SceneObject* and build an indexable from it
            auto list = value.cast<std::vector<rdl2::SceneObject*>>();
            rdl2::SceneObjectIndexable indexable(list.begin(), list.end());
            self.set(rdl2::AttributeKey<rdl2::SceneObjectIndexable>(*attr), indexable, ts);
            break;
        }
        default:
            throw std::runtime_error("Unknown or unsupported attribute type for set()");
    }
}

// ---------------------------------------------------------------------------
// Helper: check if attribute has changed (by name)
// ---------------------------------------------------------------------------
static bool hasAttrChanged(const rdl2::SceneObject& self, const std::string& name) {
    const rdl2::Attribute* attr = self.getSceneClass().getAttribute(name);
    return self.hasChanged(attr);
}

static bool hasBindingAttrChanged(const rdl2::SceneObject& self, const std::string& name) {
    const rdl2::Attribute* attr = self.getSceneClass().getAttribute(name);
    return self.hasBindingChanged(attr);
}

// ---------------------------------------------------------------------------
// Helper: get/set binding by attribute name
// ---------------------------------------------------------------------------
static rdl2::SceneObject* getBindingByName(
    const rdl2::SceneObject& self,
    const std::string& name)
{
    const rdl2::Attribute* attr = self.getSceneClass().getAttribute(name);
    return self.getBinding(*attr);
}

static void setBindingByName(
    rdl2::SceneObject& self,
    const std::string& name,
    rdl2::SceneObject* obj)
{
    self.setBinding(name, obj);
}

// ---------------------------------------------------------------------------
// Helper: isDefault by attribute name
// ---------------------------------------------------------------------------
static bool isDefaultByName(const rdl2::SceneObject& self, const std::string& name) {
    const rdl2::Attribute* attr = self.getSceneClass().getAttribute(name);
    return self.isDefault(*attr);
}

static bool isDefaultAndUnboundByName(const rdl2::SceneObject& self, const std::string& name) {
    const rdl2::Attribute* attr = self.getSceneClass().getAttribute(name);
    return self.isDefaultAndUnbound(*attr);
}

// ---------------------------------------------------------------------------
// Helper: get all attributes of a SceneClass as a list of Attribute*
// ---------------------------------------------------------------------------
static std::vector<const rdl2::Attribute*> getAttributeList(const rdl2::SceneClass& sc) {
    std::vector<const rdl2::Attribute*> result;
    for (auto it = sc.beginAttributes(); it != sc.endAttributes(); ++it) {
        result.push_back(*it);
    }
    return result;
}

static std::vector<std::string> getGroupNames(const rdl2::SceneClass& sc) {
    std::vector<std::string> result;
    for (auto it = sc.beginGroups(); it != sc.endGroups(); ++it) {
        result.push_back(*it);
    }
    return result;
}

// ---------------------------------------------------------------------------
// Helper: get all scene objects from SceneContext as a list
// ---------------------------------------------------------------------------
static std::vector<rdl2::SceneObject*> getAllSceneObjects(rdl2::SceneContext& ctx) {
    std::vector<rdl2::SceneObject*> result;
    for (auto it = ctx.beginSceneObject(); it != ctx.endSceneObject(); ++it) {
        result.push_back(it->second);
    }
    return result;
}

static std::vector<const rdl2::SceneClass*> getAllSceneClasses(const rdl2::SceneContext& ctx) {
    std::vector<const rdl2::SceneClass*> result;
    for (auto it = ctx.beginSceneClass(); it != ctx.endSceneClass(); ++it) {
        result.push_back(it->second);
    }
    return result;
}

// ---------------------------------------------------------------------------
// Main module definition
// ---------------------------------------------------------------------------
PYBIND11_MODULE(scene_rdl2_pybind11, m) {
    m.doc() = "Python bindings for the scene_rdl2 library";

    // -----------------------------------------------------------------------
    // Math types
    // -----------------------------------------------------------------------

    // Rgb (Color = Col3f): r, g, b
    py::class_<rdl2::Rgb>(m, "Rgb")
        .def(py::init<>())
        .def(py::init<float>(), py::arg("v"))
        .def(py::init<float, float, float>(), py::arg("r"), py::arg("g"), py::arg("b"))
        .def_readwrite("r", &rdl2::Rgb::r)
        .def_readwrite("g", &rdl2::Rgb::g)
        .def_readwrite("b", &rdl2::Rgb::b)
        .def("__repr__", [](const rdl2::Rgb& c) {
            return "Rgb(" + std::to_string(c.r) + ", " + std::to_string(c.g) + ", " + std::to_string(c.b) + ")";
        })
        .def("__eq__", [](const rdl2::Rgb& a, const rdl2::Rgb& b) {
            return a.r == b.r && a.g == b.g && a.b == b.b;
        })
        .def("__iter__", [](const rdl2::Rgb& c) {
            return py::iter(py::make_tuple(c.r, c.g, c.b));
        });

    // Rgba (Color4): r, g, b, a
    py::class_<rdl2::Rgba>(m, "Rgba")
        .def(py::init<>())
        .def(py::init<float>(), py::arg("v"))
        .def(py::init<float, float, float, float>(), py::arg("r"), py::arg("g"), py::arg("b"), py::arg("a"))
        .def_readwrite("r", &rdl2::Rgba::r)
        .def_readwrite("g", &rdl2::Rgba::g)
        .def_readwrite("b", &rdl2::Rgba::b)
        .def_readwrite("a", &rdl2::Rgba::a)
        .def("__repr__", [](const rdl2::Rgba& c) {
            return "Rgba(" + std::to_string(c.r) + ", " + std::to_string(c.g) + ", " + std::to_string(c.b) + ", " + std::to_string(c.a) + ")";
        })
        .def("__eq__", [](const rdl2::Rgba& a, const rdl2::Rgba& b) {
            return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
        });

    // Vec2f
    py::class_<rdl2::Vec2f>(m, "Vec2f")
        .def(py::init<>())
        .def(py::init<float>(), py::arg("v"))
        .def(py::init<float, float>(), py::arg("x"), py::arg("y"))
        .def_readwrite("x", &rdl2::Vec2f::x)
        .def_readwrite("y", &rdl2::Vec2f::y)
        .def("__repr__", [](const rdl2::Vec2f& v) {
            return "Vec2f(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        })
        .def("__eq__", [](const rdl2::Vec2f& a, const rdl2::Vec2f& b) {
            return a.x == b.x && a.y == b.y;
        });

    // Vec2d
    py::class_<rdl2::Vec2d>(m, "Vec2d")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("v"))
        .def(py::init<double, double>(), py::arg("x"), py::arg("y"))
        .def_readwrite("x", &rdl2::Vec2d::x)
        .def_readwrite("y", &rdl2::Vec2d::y)
        .def("__repr__", [](const rdl2::Vec2d& v) {
            return "Vec2d(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        });

    // Vec3f
    py::class_<rdl2::Vec3f>(m, "Vec3f")
        .def(py::init<>())
        .def(py::init<float>(), py::arg("v"))
        .def(py::init<float, float, float>(), py::arg("x"), py::arg("y"), py::arg("z"))
        .def_readwrite("x", &rdl2::Vec3f::x)
        .def_readwrite("y", &rdl2::Vec3f::y)
        .def_readwrite("z", &rdl2::Vec3f::z)
        .def("__repr__", [](const rdl2::Vec3f& v) {
            return "Vec3f(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
        })
        .def("__eq__", [](const rdl2::Vec3f& a, const rdl2::Vec3f& b) {
            return a.x == b.x && a.y == b.y && a.z == b.z;
        })
        .def("__iter__", [](const rdl2::Vec3f& v) {
            return py::iter(py::make_tuple(v.x, v.y, v.z));
        });

    // Vec3d
    py::class_<rdl2::Vec3d>(m, "Vec3d")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("v"))
        .def(py::init<double, double, double>(), py::arg("x"), py::arg("y"), py::arg("z"))
        .def_readwrite("x", &rdl2::Vec3d::x)
        .def_readwrite("y", &rdl2::Vec3d::y)
        .def_readwrite("z", &rdl2::Vec3d::z)
        .def("__repr__", [](const rdl2::Vec3d& v) {
            return "Vec3d(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
        });

    // Vec4f
    py::class_<rdl2::Vec4f>(m, "Vec4f")
        .def(py::init<>())
        .def(py::init<float>(), py::arg("v"))
        .def(py::init<float, float, float, float>(), py::arg("x"), py::arg("y"), py::arg("z"), py::arg("w"))
        .def_readwrite("x", &rdl2::Vec4f::x)
        .def_readwrite("y", &rdl2::Vec4f::y)
        .def_readwrite("z", &rdl2::Vec4f::z)
        .def_readwrite("w", &rdl2::Vec4f::w)
        .def("__repr__", [](const rdl2::Vec4f& v) {
            return "Vec4f(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")";
        });

    // Vec4d
    py::class_<rdl2::Vec4d>(m, "Vec4d")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("v"))
        .def(py::init<double, double, double, double>(), py::arg("x"), py::arg("y"), py::arg("z"), py::arg("w"))
        .def_readwrite("x", &rdl2::Vec4d::x)
        .def_readwrite("y", &rdl2::Vec4d::y)
        .def_readwrite("z", &rdl2::Vec4d::z)
        .def_readwrite("w", &rdl2::Vec4d::w)
        .def("__repr__", [](const rdl2::Vec4d& v) {
            return "Vec4d(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")";
        });

    // Mat4f (vx, vy, vz, vw are Vec4f rows)
    py::class_<rdl2::Mat4f>(m, "Mat4f")
        .def(py::init<>())
        .def(py::init<rdl2::Vec4f, rdl2::Vec4f, rdl2::Vec4f, rdl2::Vec4f>(),
             py::arg("vx"), py::arg("vy"), py::arg("vz"), py::arg("vw"))
        .def_readwrite("vx", &rdl2::Mat4f::vx)
        .def_readwrite("vy", &rdl2::Mat4f::vy)
        .def_readwrite("vz", &rdl2::Mat4f::vz)
        .def_readwrite("vw", &rdl2::Mat4f::vw)
        .def("__repr__", [](const rdl2::Mat4f& m) {
            return "Mat4f([" + std::to_string(m.vx.x) + ", " + std::to_string(m.vx.y) +
                   ", " + std::to_string(m.vx.z) + ", " + std::to_string(m.vx.w) + "], ...)";
        });

    // Mat4d (vx, vy, vz, vw are Vec4d rows)
    py::class_<rdl2::Mat4d>(m, "Mat4d")
        .def(py::init<>())
        .def(py::init<rdl2::Vec4d, rdl2::Vec4d, rdl2::Vec4d, rdl2::Vec4d>(),
             py::arg("vx"), py::arg("vy"), py::arg("vz"), py::arg("vw"))
        .def_readwrite("vx", &rdl2::Mat4d::vx)
        .def_readwrite("vy", &rdl2::Mat4d::vy)
        .def_readwrite("vz", &rdl2::Mat4d::vz)
        .def_readwrite("vw", &rdl2::Mat4d::vw)
        .def("__repr__", [](const rdl2::Mat4d& m) {
            return "Mat4d([" + std::to_string(m.vx.x) + ", " + std::to_string(m.vx.y) +
                   ", " + std::to_string(m.vx.z) + ", " + std::to_string(m.vx.w) + "], ...)";
        });

    // -----------------------------------------------------------------------
    // Enums from Types.h
    // -----------------------------------------------------------------------

    py::enum_<rdl2::AttributeType>(m, "AttributeType")
        .value("TYPE_UNKNOWN", rdl2::TYPE_UNKNOWN)
        .value("TYPE_BOOL", rdl2::TYPE_BOOL)
        .value("TYPE_INT", rdl2::TYPE_INT)
        .value("TYPE_LONG", rdl2::TYPE_LONG)
        .value("TYPE_FLOAT", rdl2::TYPE_FLOAT)
        .value("TYPE_DOUBLE", rdl2::TYPE_DOUBLE)
        .value("TYPE_STRING", rdl2::TYPE_STRING)
        .value("TYPE_RGB", rdl2::TYPE_RGB)
        .value("TYPE_RGBA", rdl2::TYPE_RGBA)
        .value("TYPE_VEC2F", rdl2::TYPE_VEC2F)
        .value("TYPE_VEC2D", rdl2::TYPE_VEC2D)
        .value("TYPE_VEC3F", rdl2::TYPE_VEC3F)
        .value("TYPE_VEC3D", rdl2::TYPE_VEC3D)
        .value("TYPE_VEC4F", rdl2::TYPE_VEC4F)
        .value("TYPE_VEC4D", rdl2::TYPE_VEC4D)
        .value("TYPE_MAT4F", rdl2::TYPE_MAT4F)
        .value("TYPE_MAT4D", rdl2::TYPE_MAT4D)
        .value("TYPE_SCENE_OBJECT", rdl2::TYPE_SCENE_OBJECT)
        .value("TYPE_BOOL_VECTOR", rdl2::TYPE_BOOL_VECTOR)
        .value("TYPE_INT_VECTOR", rdl2::TYPE_INT_VECTOR)
        .value("TYPE_LONG_VECTOR", rdl2::TYPE_LONG_VECTOR)
        .value("TYPE_FLOAT_VECTOR", rdl2::TYPE_FLOAT_VECTOR)
        .value("TYPE_DOUBLE_VECTOR", rdl2::TYPE_DOUBLE_VECTOR)
        .value("TYPE_STRING_VECTOR", rdl2::TYPE_STRING_VECTOR)
        .value("TYPE_RGB_VECTOR", rdl2::TYPE_RGB_VECTOR)
        .value("TYPE_RGBA_VECTOR", rdl2::TYPE_RGBA_VECTOR)
        .value("TYPE_VEC2F_VECTOR", rdl2::TYPE_VEC2F_VECTOR)
        .value("TYPE_VEC2D_VECTOR", rdl2::TYPE_VEC2D_VECTOR)
        .value("TYPE_VEC3F_VECTOR", rdl2::TYPE_VEC3F_VECTOR)
        .value("TYPE_VEC3D_VECTOR", rdl2::TYPE_VEC3D_VECTOR)
        .value("TYPE_VEC4F_VECTOR", rdl2::TYPE_VEC4F_VECTOR)
        .value("TYPE_VEC4D_VECTOR", rdl2::TYPE_VEC4D_VECTOR)
        .value("TYPE_MAT4F_VECTOR", rdl2::TYPE_MAT4F_VECTOR)
        .value("TYPE_MAT4D_VECTOR", rdl2::TYPE_MAT4D_VECTOR)
        .value("TYPE_SCENE_OBJECT_VECTOR", rdl2::TYPE_SCENE_OBJECT_VECTOR)
        .value("TYPE_SCENE_OBJECT_INDEXABLE", rdl2::TYPE_SCENE_OBJECT_INDEXABLE)
        .export_values();

    py::enum_<rdl2::AttributeFlags>(m, "AttributeFlags", py::arithmetic())
        .value("FLAGS_NONE", rdl2::FLAGS_NONE)
        .value("FLAGS_BINDABLE", rdl2::FLAGS_BINDABLE)
        .value("FLAGS_BLURRABLE", rdl2::FLAGS_BLURRABLE)
        .value("FLAGS_ENUMERABLE", rdl2::FLAGS_ENUMERABLE)
        .value("FLAGS_FILENAME", rdl2::FLAGS_FILENAME)
        .value("FLAGS_CAN_SKIP_GEOM_RELOAD", rdl2::FLAGS_CAN_SKIP_GEOM_RELOAD)
        .export_values();

    py::enum_<rdl2::AttributeTimestep>(m, "AttributeTimestep")
        .value("TIMESTEP_BEGIN", rdl2::TIMESTEP_BEGIN)
        .value("TIMESTEP_END", rdl2::TIMESTEP_END)
        .value("NUM_TIMESTEPS", rdl2::NUM_TIMESTEPS)
        .export_values();

    py::enum_<rdl2::SceneObjectInterface>(m, "SceneObjectInterface", py::arithmetic())
        .value("INTERFACE_GENERIC", rdl2::INTERFACE_GENERIC)
        .value("INTERFACE_GEOMETRYSET", rdl2::INTERFACE_GEOMETRYSET)
        .value("INTERFACE_LAYER", rdl2::INTERFACE_LAYER)
        .value("INTERFACE_LIGHTSET", rdl2::INTERFACE_LIGHTSET)
        .value("INTERFACE_NODE", rdl2::INTERFACE_NODE)
        .value("INTERFACE_CAMERA", rdl2::INTERFACE_CAMERA)
        .value("INTERFACE_ENVMAP", rdl2::INTERFACE_ENVMAP)
        .value("INTERFACE_GEOMETRY", rdl2::INTERFACE_GEOMETRY)
        .value("INTERFACE_LIGHT", rdl2::INTERFACE_LIGHT)
        .value("INTERFACE_SHADER", rdl2::INTERFACE_SHADER)
        .value("INTERFACE_DISPLACEMENT", rdl2::INTERFACE_DISPLACEMENT)
        .value("INTERFACE_MAP", rdl2::INTERFACE_MAP)
        .value("INTERFACE_ROOTSHADER", rdl2::INTERFACE_ROOTSHADER)
        .value("INTERFACE_MATERIAL", rdl2::INTERFACE_MATERIAL)
        .value("INTERFACE_VOLUMESHADER", rdl2::INTERFACE_VOLUMESHADER)
        .value("INTERFACE_RENDEROUTPUT", rdl2::INTERFACE_RENDEROUTPUT)
        .value("INTERFACE_USERDATA", rdl2::INTERFACE_USERDATA)
        .value("INTERFACE_METADATA", rdl2::INTERFACE_METADATA)
        .value("INTERFACE_LIGHTFILTER", rdl2::INTERFACE_LIGHTFILTER)
        .value("INTERFACE_TRACESET", rdl2::INTERFACE_TRACESET)
        .value("INTERFACE_JOINT", rdl2::INTERFACE_JOINT)
        .value("INTERFACE_LIGHTFILTERSET", rdl2::INTERFACE_LIGHTFILTERSET)
        .value("INTERFACE_SHADOWSET", rdl2::INTERFACE_SHADOWSET)
        .value("INTERFACE_NORMALMAP", rdl2::INTERFACE_NORMALMAP)
        .value("INTERFACE_DISPLAYFILTER", rdl2::INTERFACE_DISPLAYFILTER)
        .value("INTERFACE_SHADOWRECEIVERSET", rdl2::INTERFACE_SHADOWRECEIVERSET)
        .export_values();

    // Geometry enums
    py::enum_<rdl2::MotionBlurType>(m, "MotionBlurType")
        .value("STATIC", rdl2::MotionBlurType::STATIC)
        .value("VELOCITY", rdl2::MotionBlurType::VELOCITY)
        .value("FRAME_DELTA", rdl2::MotionBlurType::FRAME_DELTA)
        .value("ACCELERATION", rdl2::MotionBlurType::ACCELERATION)
        .value("HERMITE", rdl2::MotionBlurType::HERMITE)
        .value("STATIC_DUPLICATE", rdl2::MotionBlurType::STATIC_DUPLICATE)
        .value("BEST", rdl2::MotionBlurType::BEST)
        .export_values();

    py::enum_<rdl2::PrimitiveAttributeFrame>(m, "PrimitiveAttributeFrame")
        .value("FIRST_MOTION_STEP", rdl2::PrimitiveAttributeFrame::FIRST_MOTION_STEP)
        .value("SECOND_MOTION_STEP", rdl2::PrimitiveAttributeFrame::SECOND_MOTION_STEP)
        .value("BOTH_MOTION_STEPS", rdl2::PrimitiveAttributeFrame::BOTH_MOTION_STEPS)
        .export_values();

    // SceneVariables enums
    py::enum_<rdl2::PixelFilterType>(m, "PixelFilterType")
        .value("box", rdl2::PixelFilterType::box)
        .value("cubicBSpline", rdl2::PixelFilterType::cubicBSpline)
        .value("quadraticBSpline", rdl2::PixelFilterType::quadraticBSpline)
        .export_values();

    py::enum_<rdl2::TaskDistributionType>(m, "TaskDistributionType")
        .value("NON_OVERLAPPED_TILE", rdl2::TaskDistributionType::NON_OVERLAPPED_TILE)
        .value("MULTIPLEX_PIXEL", rdl2::TaskDistributionType::MULTIPLEX_PIXEL)
        .export_values();

    py::enum_<rdl2::VolumeOverlapMode>(m, "VolumeOverlapMode")
        .value("SUM", rdl2::VolumeOverlapMode::SUM)
        .value("MAX", rdl2::VolumeOverlapMode::MAX)
        .value("RND", rdl2::VolumeOverlapMode::RND)
        .export_values();

    py::enum_<rdl2::ShadowTerminatorFix>(m, "ShadowTerminatorFix")
        .value("OFF", rdl2::ShadowTerminatorFix::OFF)
        .value("CUSTOM", rdl2::ShadowTerminatorFix::CUSTOM)
        .value("SINE_COMPENSATION", rdl2::ShadowTerminatorFix::SINE_COMPENSATION)
        .value("GGX", rdl2::ShadowTerminatorFix::GGX)
        .value("COSINE_COMPENSATION", rdl2::ShadowTerminatorFix::COSINE_COMPENSATION)
        .export_values();

    // Light enum
    py::enum_<rdl2::TextureFilterType>(m, "TextureFilterType")
        .value("TEXTURE_FILTER_NEAREST", rdl2::TextureFilterType::TEXTURE_FILTER_NEAREST)
        .value("TEXTURE_FILTER_BILINEAR", rdl2::TextureFilterType::TEXTURE_FILTER_BILINEAR)
        .value("TEXTURE_FILTER_NEAREST_MIP_NEAREST", rdl2::TextureFilterType::TEXTURE_FILTER_NEAREST_MIP_NEAREST)
        .value("TEXTURE_FILTER_BILINEAR_MIP_NEAREST", rdl2::TextureFilterType::TEXTURE_FILTER_BILINEAR_MIP_NEAREST)
        .export_values();

    // -----------------------------------------------------------------------
    // Attribute class (non-copyable, only returned by reference from SceneClass)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Attribute>(m, "Attribute")
        .def("getName", &rdl2::Attribute::getName,
             py::return_value_policy::reference)
        .def("getAliases", &rdl2::Attribute::getAliases)
        .def("getType", &rdl2::Attribute::getType)
        .def("getObjectType", &rdl2::Attribute::getObjectType)
        .def("getFlags", &rdl2::Attribute::getFlags)
        .def("isBindable", &rdl2::Attribute::isBindable)
        .def("isBlurrable", &rdl2::Attribute::isBlurrable)
        .def("isEnumerable", &rdl2::Attribute::isEnumerable)
        .def("isFilename", &rdl2::Attribute::isFilename)
        .def("updateRequiresGeomReload", &rdl2::Attribute::updateRequiresGeomReload)
        .def("getMetadata", &rdl2::Attribute::getMetadata)
        .def("setMetadata", &rdl2::Attribute::setMetadata)
        .def("metadataExists", &rdl2::Attribute::metadataExists)
        .def("metadataEmpty", &rdl2::Attribute::metadataEmpty)
        .def("getMetadataDict", [](const rdl2::Attribute& self) {
            std::map<std::string, std::string> result;
            for (auto it = self.beginMetadata(); it != self.endMetadata(); ++it) {
                result[it->first] = it->second;
            }
            return result;
        }, "Returns all metadata as a dict.")
        .def("getEnumDescription", &rdl2::Attribute::getEnumDescription)
        .def("setEnumValue", &rdl2::Attribute::setEnumValue)
        .def("getEnumValue", &rdl2::Attribute::getEnumValue)
        .def("isValidEnumValue", &rdl2::Attribute::isValidEnumValue)
        .def("getEnumValuesDict", [](const rdl2::Attribute& self) {
            std::map<int, std::string> result;
            for (auto it = self.beginEnumValues(); it != self.endEnumValues(); ++it) {
                result[it->first] = it->second;
            }
            return result;
        }, "Returns all enum values as a dict mapping Int->description.")
        .def("show", &rdl2::Attribute::show)
        .def("__repr__", [](const rdl2::Attribute& a) {
            return "<Attribute name='" + a.getName() + "'>";
        });

    // -----------------------------------------------------------------------
    // SceneClass
    // -----------------------------------------------------------------------
    py::class_<rdl2::SceneClass>(m, "SceneClass")
        .def("getName", &rdl2::SceneClass::getName,
             py::return_value_policy::reference)
        .def("getDeclaredInterface", &rdl2::SceneClass::getDeclaredInterface)
        .def("hasAttribute", &rdl2::SceneClass::hasAttribute)
        .def("getAttribute",
             (const rdl2::Attribute* (rdl2::SceneClass::*)(const std::string&) const) &rdl2::SceneClass::getAttribute,
             py::return_value_policy::reference)
        .def("getAttributeGroup", &rdl2::SceneClass::getAttributeGroup,
             py::return_value_policy::reference)
        .def("getGroupNames", &getGroupNames)
        .def("getAttributes", &getAttributeList,
             "Returns a list of all Attribute objects in this SceneClass.")
        .def("getSourcePath", &rdl2::SceneClass::getSourcePath)
        .def("showAllAttributes", &rdl2::SceneClass::showAllAttributes)
        .def("getSceneContext",
             (const rdl2::SceneContext* (rdl2::SceneClass::*)() const) &rdl2::SceneClass::getSceneContext,
             py::return_value_policy::reference)
        .def("__repr__", [](const rdl2::SceneClass& sc) {
            return "<SceneClass name='" + sc.getName() + "'>";
        });

    // -----------------------------------------------------------------------
    // SceneObject  (base class for everything in the scene)
    // -----------------------------------------------------------------------
    py::class_<rdl2::SceneObject> sceneObjectClass(m, "SceneObject");
    sceneObjectClass
        .def("getName", &rdl2::SceneObject::getName,
             py::return_value_policy::reference)
        .def("getSceneClass", &rdl2::SceneObject::getSceneClass,
             py::return_value_policy::reference)
        .def("getType", &rdl2::SceneObject::getType)
        // isA by interface bitmask
        .def("isA", [](const rdl2::SceneObject& self, rdl2::SceneObjectInterface iface) {
            return (self.getType() & iface) != 0;
        }, py::arg("interface"))
        // isA by type name string (convenience)
        .def("isCamera",       [](const rdl2::SceneObject& s){ return s.isA<rdl2::Camera>(); })
        .def("isGeometry",     [](const rdl2::SceneObject& s){ return s.isA<rdl2::Geometry>(); })
        .def("isLight",        [](const rdl2::SceneObject& s){ return s.isA<rdl2::Light>(); })
        .def("isMaterial",     [](const rdl2::SceneObject& s){ return s.isA<rdl2::Material>(); })
        .def("isDisplacement", [](const rdl2::SceneObject& s){ return s.isA<rdl2::Displacement>(); })
        .def("isVolumeShader", [](const rdl2::SceneObject& s){ return s.isA<rdl2::VolumeShader>(); })
        .def("isLayer",        [](const rdl2::SceneObject& s){ return s.isA<rdl2::Layer>(); })
        .def("isGeometrySet",  [](const rdl2::SceneObject& s){ return s.isA<rdl2::GeometrySet>(); })
        .def("isLightSet",     [](const rdl2::SceneObject& s){ return s.isA<rdl2::LightSet>(); })
        .def("isNode",         [](const rdl2::SceneObject& s){ return s.isA<rdl2::Node>(); })
        .def("isRenderOutput", [](const rdl2::SceneObject& s){ return s.isA<rdl2::RenderOutput>(); })
        .def("isUserData",     [](const rdl2::SceneObject& s){ return s.isA<rdl2::UserData>(); })
        // Safe casts
        .def("asCamera", [](rdl2::SceneObject* s) -> rdl2::Camera* {
            return s->asA<rdl2::Camera>();
        }, py::return_value_policy::reference)
        .def("asGeometry", [](rdl2::SceneObject* s) -> rdl2::Geometry* {
            return s->asA<rdl2::Geometry>();
        }, py::return_value_policy::reference)
        .def("asLight", [](rdl2::SceneObject* s) -> rdl2::Light* {
            return s->asA<rdl2::Light>();
        }, py::return_value_policy::reference)
        .def("asMaterial", [](rdl2::SceneObject* s) -> rdl2::Material* {
            return s->asA<rdl2::Material>();
        }, py::return_value_policy::reference)
        .def("asLayer", [](rdl2::SceneObject* s) -> rdl2::Layer* {
            return s->asA<rdl2::Layer>();
        }, py::return_value_policy::reference)
        .def("asGeometrySet", [](rdl2::SceneObject* s) -> rdl2::GeometrySet* {
            return s->asA<rdl2::GeometrySet>();
        }, py::return_value_policy::reference)
        .def("asLightSet", [](rdl2::SceneObject* s) -> rdl2::LightSet* {
            return s->asA<rdl2::LightSet>();
        }, py::return_value_policy::reference)
        .def("asNode", [](rdl2::SceneObject* s) -> rdl2::Node* {
            return s->asA<rdl2::Node>();
        }, py::return_value_policy::reference)
        .def("asRenderOutput", [](rdl2::SceneObject* s) -> rdl2::RenderOutput* {
            return s->asA<rdl2::RenderOutput>();
        }, py::return_value_policy::reference)
        // Dynamic attribute access
        .def("get", [](const rdl2::SceneObject& self, const std::string& name) {
            return getAttrByName(self, name, rdl2::TIMESTEP_BEGIN);
        }, py::arg("name"), "Get attribute value by name (at TIMESTEP_BEGIN).")
        .def("get", [](const rdl2::SceneObject& self, const std::string& name, rdl2::AttributeTimestep ts) {
            return getAttrByName(self, name, ts);
        }, py::arg("name"), py::arg("timestep"), "Get attribute value by name at specific timestep.")
        .def("set", [](rdl2::SceneObject& self, const std::string& name, py::object value) {
            setAttrByName(self, name, value, rdl2::TIMESTEP_BEGIN);
        }, py::arg("name"), py::arg("value"), "Set attribute value by name (at TIMESTEP_BEGIN).")
        .def("set", [](rdl2::SceneObject& self, const std::string& name, py::object value, rdl2::AttributeTimestep ts) {
            setAttrByName(self, name, value, ts);
        }, py::arg("name"), py::arg("value"), py::arg("timestep"), "Set attribute value by name at specific timestep.")
        // Update guard
        .def("beginUpdate", &rdl2::SceneObject::beginUpdate)
        .def("endUpdate", &rdl2::SceneObject::endUpdate)
        // Reset
        .def("resetToDefault", (void (rdl2::SceneObject::*)(const std::string&)) &rdl2::SceneObject::resetToDefault,
             py::arg("name"))
        .def("resetToDefault", (void (rdl2::SceneObject::*)(const rdl2::Attribute*)) &rdl2::SceneObject::resetToDefault,
             py::arg("attribute"), py::return_value_policy::reference)
        .def("resetAllToDefault", &rdl2::SceneObject::resetAllToDefault)
        // Default checking
        .def("isDefault", &isDefaultByName, py::arg("name"))
        .def("isDefaultAndUnbound", &isDefaultAndUnboundByName, py::arg("name"))
        // Change tracking
        .def("hasChanged", &hasAttrChanged, py::arg("name"))
        .def("hasBindingChanged", &hasBindingAttrChanged, py::arg("name"))
        .def("isDirty", &rdl2::SceneObject::isDirty)
        .def("requestUpdate", &rdl2::SceneObject::requestUpdate)
        // Binding access
        .def("getBinding", &getBindingByName, py::arg("name"),
             py::return_value_policy::reference)
        .def("setBinding", &setBindingByName, py::arg("name"), py::arg("object"))
        .def("setBinding", [](rdl2::SceneObject& self, const rdl2::Attribute& attr, rdl2::SceneObject* obj) {
            self.setBinding(attr, obj);
        }, py::arg("attribute"), py::arg("object"))
        // Copy
        .def("copyAll", &rdl2::SceneObject::copyAll, py::arg("source"))
        .def("copyValues", [](rdl2::SceneObject& self, const std::string& attrName, const rdl2::SceneObject& source) {
            const rdl2::Attribute* attr = self.getSceneClass().getAttribute(attrName);
            self.copyValues(*attr, source);
        }, py::arg("attribute_name"), py::arg("source"))
        .def("__repr__", [](const rdl2::SceneObject& obj) {
            return "<SceneObject class='" + obj.getSceneClass().getName() + "' name='" + obj.getName() + "'>";
        });

    // UpdateGuard as a context manager
    py::class_<rdl2::SceneObject::UpdateGuard>(m, "UpdateGuard")
        .def(py::init<rdl2::SceneObject*>(), py::arg("scene_object"))
        .def("__enter__", [](rdl2::SceneObject::UpdateGuard& g) -> rdl2::SceneObject::UpdateGuard& { return g; })
        .def("__exit__", [](rdl2::SceneObject::UpdateGuard&, py::object, py::object, py::object) {});

    // -----------------------------------------------------------------------
    // SceneVariables (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::SceneVariables, rdl2::SceneObject>(m, "SceneVariables")
        .def("getRezedWidth", &rdl2::SceneVariables::getRezedWidth)
        .def("getRezedHeight", &rdl2::SceneVariables::getRezedHeight)
        .def("getMachineId", &rdl2::SceneVariables::getMachineId)
        .def("getNumMachines", &rdl2::SceneVariables::getNumMachines)
        .def("getLayer", &rdl2::SceneVariables::getLayer,
             py::return_value_policy::reference)
        .def("getCamera", &rdl2::SceneVariables::getCamera,
             py::return_value_policy::reference)
        .def("getExrHeaderAttributes", &rdl2::SceneVariables::getExrHeaderAttributes,
             py::return_value_policy::reference)
        .def("getTmpDir", &rdl2::SceneVariables::getTmpDir);

    // -----------------------------------------------------------------------
    // Node (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Node, rdl2::SceneObject>(m, "Node")
        .def("getNodeXform", [](const rdl2::Node& self) {
            return self.get(rdl2::Node::sNodeXformKey);
        }, "Returns the node transform matrix (Mat4d).")
        .def("setNodeXform", [](rdl2::Node& self, const rdl2::Mat4d& xform) {
            self.set(rdl2::Node::sNodeXformKey, xform);
        }, py::arg("xform"), "Sets the node transform matrix.");

    // -----------------------------------------------------------------------
    // Camera (inherits Node)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Camera, rdl2::Node>(m, "Camera")
        .def("getMediumMaterial", &rdl2::Camera::getMediumMaterial,
             py::return_value_policy::reference)
        .def("getMediumGeometry", &rdl2::Camera::getMediumGeometry,
             py::return_value_policy::reference)
        .def("getNear", [](const rdl2::Camera& self) {
            return self.get(rdl2::Camera::sNearKey);
        })
        .def("getFar", [](const rdl2::Camera& self) {
            return self.get(rdl2::Camera::sFarKey);
        })
        .def("setNear", &rdl2::Camera::setNear, py::arg("near"))
        .def("setFar", &rdl2::Camera::setFar, py::arg("far"));

    // -----------------------------------------------------------------------
    // Geometry (inherits Node)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Geometry, rdl2::Node>(m, "Geometry")
        .def("isStatic", &rdl2::Geometry::isStatic)
        .def("getSideType", &rdl2::Geometry::getSideType)
        .def("getReverseNormals", &rdl2::Geometry::getReverseNormals)
        .def("getRayEpsilon", &rdl2::Geometry::getRayEpsilon)
        .def("getShadowRayEpsilon", &rdl2::Geometry::getShadowRayEpsilon)
        .def("getShadowReceiverLabel", &rdl2::Geometry::getShadowReceiverLabel,
             py::return_value_policy::reference)
        .def("getShadowExclusionMappings", &rdl2::Geometry::getShadowExclusionMappings,
             py::return_value_policy::reference)
        .def("getVisibilityMask", &rdl2::Geometry::getVisibilityMask);

    py::enum_<rdl2::Geometry::SideType>(m, "GeometrySideType")
        .value("TWO_SIDED", rdl2::Geometry::TWO_SIDED)
        .value("SINGLE_SIDED", rdl2::Geometry::SINGLE_SIDED)
        .value("MESH_DEFAULT_SIDED", rdl2::Geometry::MESH_DEFAULT_SIDED)
        .export_values();

    // -----------------------------------------------------------------------
    // Light (inherits Node)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Light, rdl2::Node>(m, "Light")
        .def("getVisibilityMask", &rdl2::Light::getVisibilityMask)
        .def("isOn", [](const rdl2::Light& self) {
            return self.get(rdl2::Light::sOnKey);
        })
        .def("getColor", [](const rdl2::Light& self) {
            return self.get(rdl2::Light::sColorKey);
        })
        .def("getIntensity", [](const rdl2::Light& self) {
            return self.get(rdl2::Light::sIntensityKey);
        })
        .def("getExposure", [](const rdl2::Light& self) {
            return self.get(rdl2::Light::sExposureKey);
        })
        .def("getLabel", [](const rdl2::Light& self) {
            return self.get(rdl2::Light::sLabel);
        });

    // -----------------------------------------------------------------------
    // Shader hierarchy (Shader -> RootShader -> Material/Displacement/VolumeShader/Map/NormalMap)
    // We don't need all internals, just enough for casting and identification
    // -----------------------------------------------------------------------
    py::class_<rdl2::Shader, rdl2::SceneObject>(m, "Shader");
    py::class_<rdl2::RootShader, rdl2::Shader>(m, "RootShader");
    py::class_<rdl2::Material, rdl2::RootShader>(m, "Material");
    py::class_<rdl2::Displacement, rdl2::RootShader>(m, "Displacement");
    py::class_<rdl2::VolumeShader, rdl2::RootShader>(m, "VolumeShader");
    py::class_<rdl2::Map, rdl2::Shader>(m, "Map");
    py::class_<rdl2::NormalMap, rdl2::Shader>(m, "NormalMap");

    // -----------------------------------------------------------------------
    // GeometrySet (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::GeometrySet, rdl2::SceneObject>(m, "GeometrySet")
        .def("getGeometries", [](const rdl2::GeometrySet& self) {
            const rdl2::SceneObjectIndexable& idx = self.getGeometries();
            return std::vector<rdl2::SceneObject*>(idx.begin(), idx.end());
        }, "Returns a list of Geometry SceneObjects in this set.")
        .def("add", &rdl2::GeometrySet::add, py::arg("geometry"))
        .def("remove", &rdl2::GeometrySet::remove, py::arg("geometry"))
        .def("contains", &rdl2::GeometrySet::contains, py::arg("geometry"))
        .def("clear", &rdl2::GeometrySet::clear)
        .def("isStatic", &rdl2::GeometrySet::isStatic)
        .def("haveGeometriesChanged", &rdl2::GeometrySet::haveGeometriesChanged);

    // -----------------------------------------------------------------------
    // LightSet (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::LightSet, rdl2::SceneObject>(m, "LightSet")
        .def("getLights", [](const rdl2::LightSet& self) {
            return self.getLights();
        }, "Returns a list of Light SceneObjects in this set.")
        .def("add", &rdl2::LightSet::add, py::arg("light"))
        .def("remove", &rdl2::LightSet::remove, py::arg("light"))
        .def("contains", &rdl2::LightSet::contains, py::arg("light"))
        .def("clear", &rdl2::LightSet::clear);

    // -----------------------------------------------------------------------
    // Layer assignment struct
    // -----------------------------------------------------------------------
    py::class_<rdl2::LayerAssignment>(m, "LayerAssignment")
        .def(py::init<>())
        .def_readwrite("material", &rdl2::LayerAssignment::mMaterial)
        .def_readwrite("lightSet", &rdl2::LayerAssignment::mLightSet)
        .def_readwrite("displacement", &rdl2::LayerAssignment::mDisplacement)
        .def_readwrite("volumeShader", &rdl2::LayerAssignment::mVolumeShader)
        .def_readwrite("lightFilterSet", &rdl2::LayerAssignment::mLightFilterSet)
        .def_readwrite("shadowSet", &rdl2::LayerAssignment::mShadowSet)
        .def_readwrite("shadowReceiverSet", &rdl2::LayerAssignment::mShadowReceiverSet);

    // -----------------------------------------------------------------------
    // Layer (inherits via TraceSet -> SceneObject; we skip TraceSet for now)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Layer, rdl2::SceneObject>(m, "Layer")
        .def("assign",
             (int32_t (rdl2::Layer::*)(rdl2::Geometry*, const rdl2::String&,
                                       rdl2::Material*, rdl2::LightSet*))
             &rdl2::Layer::assign,
             py::arg("geometry"), py::arg("part_name"),
             py::arg("material"), py::arg("light_set"))
        .def("assign",
             (int32_t (rdl2::Layer::*)(rdl2::Geometry*, const rdl2::String&,
                                       rdl2::Material*, rdl2::LightSet*,
                                       rdl2::Displacement*, rdl2::VolumeShader*))
             &rdl2::Layer::assign,
             py::arg("geometry"), py::arg("part_name"),
             py::arg("material"), py::arg("light_set"),
             py::arg("displacement"), py::arg("volume_shader"))
        .def("assign",
             (int32_t (rdl2::Layer::*)(rdl2::Geometry*, const rdl2::String&,
                                       const rdl2::LayerAssignment&))
             &rdl2::Layer::assign,
             py::arg("geometry"), py::arg("part_name"), py::arg("assignment"))
        .def("lookupMaterial", &rdl2::Layer::lookupMaterial,
             py::arg("assignment_id"), py::return_value_policy::reference)
        .def("lookupLightSet", &rdl2::Layer::lookupLightSet,
             py::arg("assignment_id"), py::return_value_policy::reference)
        .def("lookupDisplacement", &rdl2::Layer::lookupDisplacement,
             py::arg("assignment_id"), py::return_value_policy::reference)
        .def("lookupVolumeShader", &rdl2::Layer::lookupVolumeShader,
             py::arg("assignment_id"), py::return_value_policy::reference)
        .def("clear", &rdl2::Layer::clear)
        .def("lightSetsChanged", &rdl2::Layer::lightSetsChanged);

    // -----------------------------------------------------------------------
    // RenderOutput (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::RenderOutput, rdl2::SceneObject>(m, "RenderOutput")
        .def("getActive", &rdl2::RenderOutput::getActive)
        .def("getResult", &rdl2::RenderOutput::getResult)
        .def("getOutputType", &rdl2::RenderOutput::getOutputType)
        .def("getStateVariable", &rdl2::RenderOutput::getStateVariable)
        .def("getPrimitiveAttribute", &rdl2::RenderOutput::getPrimitiveAttribute)
        .def("getPrimitiveAttributeType", &rdl2::RenderOutput::getPrimitiveAttributeType)
        .def("getMaterialAov", &rdl2::RenderOutput::getMaterialAov)
        .def("getLpe", &rdl2::RenderOutput::getLpe)
        .def("getVisibilityAov", &rdl2::RenderOutput::getVisibilityAov)
        .def("getFileName", &rdl2::RenderOutput::getFileName)
        .def("getFilePart", &rdl2::RenderOutput::getFilePart)
        .def("getCompression", &rdl2::RenderOutput::getCompression)
        .def("getCompressionLevel", &rdl2::RenderOutput::getCompressionLevel)
        .def("getChannelName", &rdl2::RenderOutput::getChannelName)
        .def("getChannelSuffixMode", &rdl2::RenderOutput::getChannelSuffixMode)
        .def("getChannelFormat", &rdl2::RenderOutput::getChannelFormat)
        .def("getMathFilter", &rdl2::RenderOutput::getMathFilter)
        .def("getExrHeaderAttributes", &rdl2::RenderOutput::getExrHeaderAttributes)
        .def("getDenoiserInput", &rdl2::RenderOutput::getDenoiserInput)
        .def("getDenoise", &rdl2::RenderOutput::getDenoise)
        .def("getCheckpointFileName", &rdl2::RenderOutput::getCheckpointFileName)
        .def("getCheckpointMultiVersionFileName", &rdl2::RenderOutput::getCheckpointMultiVersionFileName)
        .def("getResumeFileName", &rdl2::RenderOutput::getResumeFileName)
        .def("getCryptomatteDepth", &rdl2::RenderOutput::getCryptomatteDepth)
        .def("getCryptomatteNumLayers", &rdl2::RenderOutput::getCryptomatteNumLayers)
        .def("getCryptomatteOutputPositions", &rdl2::RenderOutput::getCryptomatteOutputPositions)
        .def("getCryptomatteOutputP0", &rdl2::RenderOutput::getCryptomatteOutputP0)
        .def("getCryptomatteOutputNormals", &rdl2::RenderOutput::getCryptomatteOutputNormals)
        .def("getCryptomatteOutputBeauty", &rdl2::RenderOutput::getCryptomatteOutputBeauty)
        .def("getCryptomatteOutputRefP", &rdl2::RenderOutput::getCryptomatteOutputRefP)
        .def("getCryptomatteOutputRefN", &rdl2::RenderOutput::getCryptomatteOutputRefN)
        .def("getCryptomatteOutputUV", &rdl2::RenderOutput::getCryptomatteOutputUV)
        .def("getCryptomatteSupportResumeRender", &rdl2::RenderOutput::getCryptomatteSupportResumeRender)
        .def("getCryptomatteRecordReflected", &rdl2::RenderOutput::getCryptomatteRecordReflected)
        .def("getCryptomatteRecordRefracted", &rdl2::RenderOutput::getCryptomatteRecordRefracted)
        .def("getCryptomatteNumExtraChannels", &rdl2::RenderOutput::getCryptomatteNumExtraChannels)
        .def("cryptomatteHasExtraOutput", &rdl2::RenderOutput::cryptomatteHasExtraOutput)
        .def("getCamera", &rdl2::RenderOutput::getCamera,
             py::return_value_policy::reference);

    // RenderOutput enums
    py::enum_<rdl2::RenderOutput::ChannelFormat>(m, "ChannelFormat")
        .value("CHANNEL_FORMAT_FLOAT", rdl2::RenderOutput::CHANNEL_FORMAT_FLOAT)
        .value("CHANNEL_FORMAT_HALF", rdl2::RenderOutput::CHANNEL_FORMAT_HALF)
        .export_values();

    py::enum_<rdl2::RenderOutput::Compression>(m, "Compression")
        .value("COMPRESSION_NONE", rdl2::RenderOutput::COMPRESSION_NONE)
        .value("COMPRESSION_ZIP", rdl2::RenderOutput::COMPRESSION_ZIP)
        .value("COMPRESSION_RLE", rdl2::RenderOutput::COMPRESSION_RLE)
        .value("COMPRESSION_ZIPS", rdl2::RenderOutput::COMPRESSION_ZIPS)
        .value("COMPRESSION_PIZ", rdl2::RenderOutput::COMPRESSION_PIZ)
        .value("COMPRESSION_PXR24", rdl2::RenderOutput::COMPRESSION_PXR24)
        .value("COMPRESSION_B44", rdl2::RenderOutput::COMPRESSION_B44)
        .value("COMPRESSION_B44A", rdl2::RenderOutput::COMPRESSION_B44A)
        .value("COMPRESSION_DWAA", rdl2::RenderOutput::COMPRESSION_DWAA)
        .value("COMPRESSION_DWAB", rdl2::RenderOutput::COMPRESSION_DWAB)
        .export_values();

    py::enum_<rdl2::RenderOutput::Result>(m, "Result")
        .value("RESULT_BEAUTY", rdl2::RenderOutput::RESULT_BEAUTY)
        .value("RESULT_ALPHA", rdl2::RenderOutput::RESULT_ALPHA)
        .value("RESULT_DEPTH", rdl2::RenderOutput::RESULT_DEPTH)
        .value("RESULT_STATE_VARIABLE", rdl2::RenderOutput::RESULT_STATE_VARIABLE)
        .value("RESULT_PRIMITIVE_ATTRIBUTE", rdl2::RenderOutput::RESULT_PRIMITIVE_ATTRIBUTE)
        .value("RESULT_HEAT_MAP", rdl2::RenderOutput::RESULT_HEAT_MAP)
        .value("RESULT_WIREFRAME", rdl2::RenderOutput::RESULT_WIREFRAME)
        .value("RESULT_MATERIAL_AOV", rdl2::RenderOutput::RESULT_MATERIAL_AOV)
        .value("RESULT_LIGHT_AOV", rdl2::RenderOutput::RESULT_LIGHT_AOV)
        .value("RESULT_VISIBILITY_AOV", rdl2::RenderOutput::RESULT_VISIBILITY_AOV)
        .value("RESULT_WEIGHT", rdl2::RenderOutput::RESULT_WEIGHT)
        .value("RESULT_BEAUTY_AUX", rdl2::RenderOutput::RESULT_BEAUTY_AUX)
        .value("RESULT_CRYPTOMATTE", rdl2::RenderOutput::RESULT_CRYPTOMATTE)
        .value("RESULT_ALPHA_AUX", rdl2::RenderOutput::RESULT_ALPHA_AUX)
        .value("RESULT_DISPLAY_FILTER", rdl2::RenderOutput::RESULT_DISPLAY_FILTER)
        .export_values();

    py::enum_<rdl2::RenderOutput::StateVariable>(m, "StateVariable")
        .value("STATE_VARIABLE_P", rdl2::RenderOutput::STATE_VARIABLE_P)
        .value("STATE_VARIABLE_NG", rdl2::RenderOutput::STATE_VARIABLE_NG)
        .value("STATE_VARIABLE_N", rdl2::RenderOutput::STATE_VARIABLE_N)
        .value("STATE_VARIABLE_ST", rdl2::RenderOutput::STATE_VARIABLE_ST)
        .value("STATE_VARIABLE_DPDS", rdl2::RenderOutput::STATE_VARIABLE_DPDS)
        .value("STATE_VARIABLE_DPDT", rdl2::RenderOutput::STATE_VARIABLE_DPDT)
        .value("STATE_VARIABLE_DSDX", rdl2::RenderOutput::STATE_VARIABLE_DSDX)
        .value("STATE_VARIABLE_DSDY", rdl2::RenderOutput::STATE_VARIABLE_DSDY)
        .value("STATE_VARIABLE_DTDX", rdl2::RenderOutput::STATE_VARIABLE_DTDX)
        .value("STATE_VARIABLE_DTDY", rdl2::RenderOutput::STATE_VARIABLE_DTDY)
        .value("STATE_VARIABLE_WP", rdl2::RenderOutput::STATE_VARIABLE_WP)
        .value("STATE_VARIABLE_DEPTH", rdl2::RenderOutput::STATE_VARIABLE_DEPTH)
        .value("STATE_VARIABLE_MOTION", rdl2::RenderOutput::STATE_VARIABLE_MOTION)
        .export_values();

    py::enum_<rdl2::RenderOutput::PrimitiveAttributeType>(m, "PrimitiveAttributeType")
        .value("PRIMITIVE_ATTRIBUTE_TYPE_FLOAT", rdl2::RenderOutput::PRIMITIVE_ATTRIBUTE_TYPE_FLOAT)
        .value("PRIMITIVE_ATTRIBUTE_TYPE_VEC2F", rdl2::RenderOutput::PRIMITIVE_ATTRIBUTE_TYPE_VEC2F)
        .value("PRIMITIVE_ATTRIBUTE_TYPE_VEC3F", rdl2::RenderOutput::PRIMITIVE_ATTRIBUTE_TYPE_VEC3F)
        .value("PRIMITIVE_ATTRIBUTE_TYPE_RGB", rdl2::RenderOutput::PRIMITIVE_ATTRIBUTE_TYPE_RGB)
        .export_values();

    py::enum_<rdl2::RenderOutput::MathFilter>(m, "MathFilter")
        .value("MATH_FILTER_AVG", rdl2::RenderOutput::MATH_FILTER_AVG)
        .value("MATH_FILTER_SUM", rdl2::RenderOutput::MATH_FILTER_SUM)
        .value("MATH_FILTER_MIN", rdl2::RenderOutput::MATH_FILTER_MIN)
        .value("MATH_FILTER_MAX", rdl2::RenderOutput::MATH_FILTER_MAX)
        .value("MATH_FILTER_FORCE_CONSISTENT_SAMPLING", rdl2::RenderOutput::MATH_FILTER_FORCE_CONSISTENT_SAMPLING)
        .value("MATH_FILTER_CLOSEST", rdl2::RenderOutput::MATH_FILTER_CLOSEST)
        .export_values();

    py::enum_<rdl2::RenderOutput::SuffixMode>(m, "SuffixMode")
        .value("SUFFIX_MODE_AUTO", rdl2::RenderOutput::SUFFIX_MODE_AUTO)
        .value("SUFFIX_MODE_RGB", rdl2::RenderOutput::SUFFIX_MODE_RGB)
        .value("SUFFIX_MODE_XYZ", rdl2::RenderOutput::SUFFIX_MODE_XYZ)
        .value("SUFFIX_MODE_UVW", rdl2::RenderOutput::SUFFIX_MODE_UVW)
        .value("SUFFIX_MODE_NUM_MODES", rdl2::RenderOutput::SUFFIX_MODE_NUM_MODES)
        .export_values();

    py::enum_<rdl2::RenderOutput::DenoiserInput>(m, "DenoiserInput")
        .value("DENOISER_INPUT_NONE", rdl2::RenderOutput::DENOISER_INPUT_NONE)
        .value("DENOISER_INPUT_ALBEDO", rdl2::RenderOutput::DENOISER_INPUT_ALBEDO)
        .value("DENOISER_INPUT_NORMAL", rdl2::RenderOutput::DENOISER_INPUT_NORMAL)
        .export_values();

    // -----------------------------------------------------------------------
    // SceneContext
    // -----------------------------------------------------------------------
    py::class_<rdl2::SceneContext>(m, "SceneContext")
        .def(py::init<>())
        // DSO path
        .def("getDsoPath", &rdl2::SceneContext::getDsoPath)
        .def("setDsoPath", &rdl2::SceneContext::setDsoPath)
        // Proxy mode
        .def("getProxyModeEnabled", &rdl2::SceneContext::getProxyModeEnabled)
        .def("setProxyModeEnabled", &rdl2::SceneContext::setProxyModeEnabled)
        // Scene variables
        .def("getSceneVariables",
             (const rdl2::SceneVariables& (rdl2::SceneContext::*)() const) &rdl2::SceneContext::getSceneVariables,
             py::return_value_policy::reference)
        .def("getSceneVariables",
             (rdl2::SceneVariables& (rdl2::SceneContext::*)()) &rdl2::SceneContext::getSceneVariables,
             py::return_value_policy::reference)
        // Scene classes
        .def("getSceneClass",
             (const rdl2::SceneClass* (rdl2::SceneContext::*)(const std::string&) const) &rdl2::SceneContext::getSceneClass,
             py::return_value_policy::reference)
        .def("sceneClassExists", &rdl2::SceneContext::sceneClassExists)
        .def("createSceneClass", &rdl2::SceneContext::createSceneClass,
             py::return_value_policy::reference)
        .def("getAllSceneClasses", &getAllSceneClasses,
             "Returns a list of all SceneClass objects in the context.")
        // Scene objects
        .def("getSceneObject",
             (const rdl2::SceneObject* (rdl2::SceneContext::*)(const std::string&) const) &rdl2::SceneContext::getSceneObject,
             py::return_value_policy::reference)
        .def("getSceneObject",
             (rdl2::SceneObject* (rdl2::SceneContext::*)(const std::string&)) &rdl2::SceneContext::getSceneObject,
             py::return_value_policy::reference)
        .def("sceneObjectExists", &rdl2::SceneContext::sceneObjectExists)
        .def("createSceneObject", &rdl2::SceneContext::createSceneObject,
             py::return_value_policy::reference)
        .def("getAllSceneObjects", &getAllSceneObjects,
             "Returns a list of all SceneObject instances in the context.")
        // Cameras
        .def("getPrimaryCamera", &rdl2::SceneContext::getPrimaryCamera,
             py::return_value_policy::reference)
        .def("getCameras", &rdl2::SceneContext::getCameras)
        .def("getActiveCameras", &rdl2::SceneContext::getActiveCameras)
        .def("getDicingCamera", &rdl2::SceneContext::getDicingCamera,
             py::return_value_policy::reference)
        // Transforms
        .def("getRender2World", &rdl2::SceneContext::getRender2World,
             py::return_value_policy::reference)
        .def("setRender2World", &rdl2::SceneContext::setRender2World)
        // Checkpoint/resume
        .def("getCheckpointActive", &rdl2::SceneContext::getCheckpointActive)
        .def("getResumableOutput", &rdl2::SceneContext::getResumableOutput)
        .def("getResumeRender", &rdl2::SceneContext::getResumeRender)
        // Commit / load
        .def("commitAllChanges", &rdl2::SceneContext::commitAllChanges)
        .def("loadAllSceneClasses", &rdl2::SceneContext::loadAllSceneClasses)
        // DSO counts
        .def("getDsoCounts", &rdl2::SceneContext::getDsoCounts);

    // -----------------------------------------------------------------------
    // AsciiReader
    // -----------------------------------------------------------------------
    py::class_<rdl2::AsciiReader>(m, "AsciiReader")
        .def(py::init<rdl2::SceneContext&>(), py::arg("context"))
        .def("fromFile", &rdl2::AsciiReader::fromFile, py::arg("filename"))
        .def("fromString", &rdl2::AsciiReader::fromString,
             py::arg("code"), py::arg("chunk_name") = "@rdla")
        .def("setWarningsAsErrors", &rdl2::AsciiReader::setWarningsAsErrors,
             py::arg("warnings_as_errors"));

    // -----------------------------------------------------------------------
    // AsciiWriter
    // -----------------------------------------------------------------------
    py::class_<rdl2::AsciiWriter>(m, "AsciiWriter")
        .def(py::init<const rdl2::SceneContext&>(), py::arg("context"))
        .def("setDeltaEncoding", &rdl2::AsciiWriter::setDeltaEncoding,
             py::arg("delta_encoding"))
        .def("setSkipDefaults", &rdl2::AsciiWriter::setSkipDefaults,
             py::arg("skip_defaults"))
        .def("setElementsPerLine", &rdl2::AsciiWriter::setElementsPerLine,
             py::arg("elements_per_line"))
        .def("toFile", &rdl2::AsciiWriter::toFile, py::arg("filename"))
        .def("toString", &rdl2::AsciiWriter::toString);

    // -----------------------------------------------------------------------
    // Free functions
    // -----------------------------------------------------------------------
    m.def("attributeTypeName",
          (const char* (*)(rdl2::AttributeType)) &rdl2::attributeTypeName,
          py::arg("type"),
          "Returns the string name of an AttributeType enum value.");
}
