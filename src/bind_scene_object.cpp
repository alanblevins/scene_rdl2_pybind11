// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for SceneObject and its UpdateGuard.

#include "bindings.h"

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
        case rdl2::TYPE_BOOL:   return py::cast(self.get(rdl2::AttributeKey<rdl2::Bool>(*attr), ts));
        case rdl2::TYPE_INT:    return py::cast(self.get(rdl2::AttributeKey<rdl2::Int>(*attr), ts));
        case rdl2::TYPE_LONG:   return py::cast(self.get(rdl2::AttributeKey<rdl2::Long>(*attr), ts));
        case rdl2::TYPE_FLOAT:  return py::cast(self.get(rdl2::AttributeKey<rdl2::Float>(*attr), ts));
        case rdl2::TYPE_DOUBLE: return py::cast(self.get(rdl2::AttributeKey<rdl2::Double>(*attr), ts));
        case rdl2::TYPE_STRING: return py::cast(self.get(rdl2::AttributeKey<rdl2::String>(*attr), ts));
        case rdl2::TYPE_RGB:    return py::cast(self.get(rdl2::AttributeKey<rdl2::Rgb>(*attr), ts));
        case rdl2::TYPE_RGBA:   return py::cast(self.get(rdl2::AttributeKey<rdl2::Rgba>(*attr), ts));
        case rdl2::TYPE_VEC2F:  return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec2f>(*attr), ts));
        case rdl2::TYPE_VEC2D:  return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec2d>(*attr), ts));
        case rdl2::TYPE_VEC3F:  return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec3f>(*attr), ts));
        case rdl2::TYPE_VEC3D:  return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec3d>(*attr), ts));
        case rdl2::TYPE_VEC4F:  return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec4f>(*attr), ts));
        case rdl2::TYPE_VEC4D:  return py::cast(self.get(rdl2::AttributeKey<rdl2::Vec4d>(*attr), ts));
        case rdl2::TYPE_MAT4F:  return py::cast(self.get(rdl2::AttributeKey<rdl2::Mat4f>(*attr), ts));
        case rdl2::TYPE_MAT4D:  return py::cast(self.get(rdl2::AttributeKey<rdl2::Mat4d>(*attr), ts));
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
            self.set(rdl2::AttributeKey<rdl2::Bool>(*attr), value.cast<rdl2::Bool>(), ts); break;
        case rdl2::TYPE_INT:
            self.set(rdl2::AttributeKey<rdl2::Int>(*attr), value.cast<rdl2::Int>(), ts); break;
        case rdl2::TYPE_LONG:
            self.set(rdl2::AttributeKey<rdl2::Long>(*attr), value.cast<rdl2::Long>(), ts); break;
        case rdl2::TYPE_FLOAT:
            self.set(rdl2::AttributeKey<rdl2::Float>(*attr), value.cast<rdl2::Float>(), ts); break;
        case rdl2::TYPE_DOUBLE:
            self.set(rdl2::AttributeKey<rdl2::Double>(*attr), value.cast<rdl2::Double>(), ts); break;
        case rdl2::TYPE_STRING:
            self.set(rdl2::AttributeKey<rdl2::String>(*attr), value.cast<rdl2::String>(), ts); break;
        case rdl2::TYPE_RGB:
            self.set(rdl2::AttributeKey<rdl2::Rgb>(*attr), value.cast<rdl2::Rgb>(), ts); break;
        case rdl2::TYPE_RGBA:
            self.set(rdl2::AttributeKey<rdl2::Rgba>(*attr), value.cast<rdl2::Rgba>(), ts); break;
        case rdl2::TYPE_VEC2F:
            self.set(rdl2::AttributeKey<rdl2::Vec2f>(*attr), value.cast<rdl2::Vec2f>(), ts); break;
        case rdl2::TYPE_VEC2D:
            self.set(rdl2::AttributeKey<rdl2::Vec2d>(*attr), value.cast<rdl2::Vec2d>(), ts); break;
        case rdl2::TYPE_VEC3F:
            self.set(rdl2::AttributeKey<rdl2::Vec3f>(*attr), value.cast<rdl2::Vec3f>(), ts); break;
        case rdl2::TYPE_VEC3D:
            self.set(rdl2::AttributeKey<rdl2::Vec3d>(*attr), value.cast<rdl2::Vec3d>(), ts); break;
        case rdl2::TYPE_VEC4F:
            self.set(rdl2::AttributeKey<rdl2::Vec4f>(*attr), value.cast<rdl2::Vec4f>(), ts); break;
        case rdl2::TYPE_VEC4D:
            self.set(rdl2::AttributeKey<rdl2::Vec4d>(*attr), value.cast<rdl2::Vec4d>(), ts); break;
        case rdl2::TYPE_MAT4F:
            self.set(rdl2::AttributeKey<rdl2::Mat4f>(*attr), value.cast<rdl2::Mat4f>(), ts); break;
        case rdl2::TYPE_MAT4D:
            self.set(rdl2::AttributeKey<rdl2::Mat4d>(*attr), value.cast<rdl2::Mat4d>(), ts); break;
        case rdl2::TYPE_SCENE_OBJECT:
            self.set(rdl2::AttributeKey<rdl2::SceneObject*>(*attr),
                     value.cast<rdl2::SceneObject*>()); break;
        case rdl2::TYPE_BOOL_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::BoolVector>(*attr),
                     value.cast<rdl2::BoolVector>(), ts); break;
        case rdl2::TYPE_INT_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::IntVector>(*attr),
                     value.cast<rdl2::IntVector>(), ts); break;
        case rdl2::TYPE_LONG_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::LongVector>(*attr),
                     value.cast<rdl2::LongVector>(), ts); break;
        case rdl2::TYPE_FLOAT_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::FloatVector>(*attr),
                     value.cast<rdl2::FloatVector>(), ts); break;
        case rdl2::TYPE_DOUBLE_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::DoubleVector>(*attr),
                     value.cast<rdl2::DoubleVector>(), ts); break;
        case rdl2::TYPE_STRING_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::StringVector>(*attr),
                     value.cast<rdl2::StringVector>(), ts); break;
        case rdl2::TYPE_RGB_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::RgbVector>(*attr),
                     value.cast<rdl2::RgbVector>(), ts); break;
        case rdl2::TYPE_RGBA_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::RgbaVector>(*attr),
                     value.cast<rdl2::RgbaVector>(), ts); break;
        case rdl2::TYPE_VEC2F_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Vec2fVector>(*attr),
                     value.cast<rdl2::Vec2fVector>(), ts); break;
        case rdl2::TYPE_VEC2D_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Vec2dVector>(*attr),
                     value.cast<rdl2::Vec2dVector>(), ts); break;
        case rdl2::TYPE_VEC3F_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Vec3fVector>(*attr),
                     value.cast<rdl2::Vec3fVector>(), ts); break;
        case rdl2::TYPE_VEC3D_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Vec3dVector>(*attr),
                     value.cast<rdl2::Vec3dVector>(), ts); break;
        case rdl2::TYPE_VEC4F_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Vec4fVector>(*attr),
                     value.cast<rdl2::Vec4fVector>(), ts); break;
        case rdl2::TYPE_VEC4D_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Vec4dVector>(*attr),
                     value.cast<rdl2::Vec4dVector>(), ts); break;
        case rdl2::TYPE_MAT4F_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Mat4fVector>(*attr),
                     value.cast<rdl2::Mat4fVector>(), ts); break;
        case rdl2::TYPE_MAT4D_VECTOR:
            self.set(rdl2::AttributeKey<rdl2::Mat4dVector>(*attr),
                     value.cast<rdl2::Mat4dVector>(), ts); break;
        case rdl2::TYPE_SCENE_OBJECT_VECTOR: {
            rdl2::SceneObjectVector v = value.cast<rdl2::SceneObjectVector>();
            self.set(rdl2::AttributeKey<rdl2::SceneObjectVector>(*attr), v, ts); break;
        }
        case rdl2::TYPE_SCENE_OBJECT_INDEXABLE: {
            auto list = value.cast<std::vector<rdl2::SceneObject*>>();
            rdl2::SceneObjectIndexable indexable(list.begin(), list.end());
            self.set(rdl2::AttributeKey<rdl2::SceneObjectIndexable>(*attr), indexable, ts); break;
        }
        default:
            throw std::runtime_error("Unknown or unsupported attribute type for set()");
    }
}

// ---------------------------------------------------------------------------
// Miscellaneous SceneObject helpers
// ---------------------------------------------------------------------------
static bool hasAttrChanged(const rdl2::SceneObject& self, const std::string& name) {
    const rdl2::Attribute* attr = self.getSceneClass().getAttribute(name);
    return self.hasChanged(attr);
}

static bool hasBindingAttrChanged(const rdl2::SceneObject& self, const std::string& name) {
    const rdl2::Attribute* attr = self.getSceneClass().getAttribute(name);
    return self.hasBindingChanged(attr);
}

static rdl2::SceneObject* getBindingByName(const rdl2::SceneObject& self, const std::string& name) {
    const rdl2::Attribute* attr = self.getSceneClass().getAttribute(name);
    return self.getBinding(*attr);
}

static void setBindingByName(rdl2::SceneObject& self, const std::string& name, rdl2::SceneObject* obj) {
    self.setBinding(name, obj);
}

static bool isDefaultByName(const rdl2::SceneObject& self, const std::string& name) {
    const rdl2::Attribute* attr = self.getSceneClass().getAttribute(name);
    return self.isDefault(*attr);
}

static bool isDefaultAndUnboundByName(const rdl2::SceneObject& self, const std::string& name) {
    const rdl2::Attribute* attr = self.getSceneClass().getAttribute(name);
    return self.isDefaultAndUnbound(*attr);
}

// ---------------------------------------------------------------------------
// bind_scene_object
// ---------------------------------------------------------------------------
void bind_scene_object(py::module_& m)
{
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
        // isA convenience predicates
        .def("isCamera",       [](const rdl2::SceneObject& s){ return s.isA<rdl2::Camera>(); })
        .def("isGeometry",     [](const rdl2::SceneObject& s){ return s.isA<rdl2::Geometry>(); })
        .def("isLight",        [](const rdl2::SceneObject& s){ return s.isA<rdl2::Light>(); })
        .def("isMaterial",     [](const rdl2::SceneObject& s){ return s.isA<rdl2::Material>(); })
        .def("isDisplacement", [](const rdl2::SceneObject& s){ return s.isA<rdl2::Displacement>(); })
        .def("isVolumeShader", [](const rdl2::SceneObject& s){ return s.isA<rdl2::VolumeShader>(); })
        .def("isLayer",        [](const rdl2::SceneObject& s){ return s.isA<rdl2::Layer>(); })
        .def("isGeometrySet",  [](const rdl2::SceneObject& s){ return s.isA<rdl2::GeometrySet>(); })
        .def("isLightSet",     [](const rdl2::SceneObject& s){ return s.isA<rdl2::LightSet>(); })
        .def("isNode",              [](const rdl2::SceneObject& s){ return s.isA<rdl2::Node>(); })
        .def("isRenderOutput",      [](const rdl2::SceneObject& s){ return s.isA<rdl2::RenderOutput>(); })
        .def("isUserData",          [](const rdl2::SceneObject& s){ return s.isA<rdl2::UserData>(); })
        .def("isLightFilter",       [](const rdl2::SceneObject& s){ return s.isA<rdl2::LightFilter>(); })
        .def("isLightFilterSet",    [](const rdl2::SceneObject& s){ return s.isA<rdl2::LightFilterSet>(); })
        .def("isShadowSet",         [](const rdl2::SceneObject& s){ return s.isA<rdl2::ShadowSet>(); })
        .def("isShadowReceiverSet", [](const rdl2::SceneObject& s){ return s.isA<rdl2::ShadowReceiverSet>(); })
        // Safe downcasts
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
        .def("asLightFilter", [](rdl2::SceneObject* s) -> rdl2::LightFilter* {
            return s->asA<rdl2::LightFilter>();
        }, py::return_value_policy::reference)
        .def("asLightFilterSet", [](rdl2::SceneObject* s) -> rdl2::LightFilterSet* {
            return s->asA<rdl2::LightFilterSet>();
        }, py::return_value_policy::reference)
        .def("asShadowSet", [](rdl2::SceneObject* s) -> rdl2::ShadowSet* {
            return s->asA<rdl2::ShadowSet>();
        }, py::return_value_policy::reference)
        .def("asShadowReceiverSet", [](rdl2::SceneObject* s) -> rdl2::ShadowReceiverSet* {
            return s->asA<rdl2::ShadowReceiverSet>();
        }, py::return_value_policy::reference)
        // Dictionary-style attribute access
        // obj["attr"]                         -> get at TIMESTEP_BEGIN
        // obj["attr", rdl2.TIMESTEP_END]      -> get at specific timestep
        .def("__getitem__", [](const rdl2::SceneObject& self, py::object key) -> py::object {
            if (py::isinstance<py::str>(key)) {
                return getAttrByName(self, key.cast<std::string>(), rdl2::TIMESTEP_BEGIN);
            }
            if (py::isinstance<py::tuple>(key)) {
                py::tuple t = key.cast<py::tuple>();
                if (t.size() != 2)
                    throw py::key_error("key tuple must be (attr_name, timestep)");
                return getAttrByName(self,
                                     t[0].cast<std::string>(),
                                     t[1].cast<rdl2::AttributeTimestep>());
            }
            throw py::key_error("key must be a string or (string, AttributeTimestep) tuple");
        })
        // obj["attr"] = value                 -> set at TIMESTEP_BEGIN
        // obj["attr", rdl2.TIMESTEP_END] = v  -> set at specific timestep
        .def("__setitem__", [](rdl2::SceneObject& self, py::object key, py::object value) {
            if (py::isinstance<py::str>(key)) {
                setAttrByName(self, key.cast<std::string>(), value, rdl2::TIMESTEP_BEGIN);
                return;
            }
            if (py::isinstance<py::tuple>(key)) {
                py::tuple t = key.cast<py::tuple>();
                if (t.size() != 2)
                    throw py::key_error("key tuple must be (attr_name, timestep)");
                setAttrByName(self,
                              t[0].cast<std::string>(),
                              value,
                              t[1].cast<rdl2::AttributeTimestep>());
                return;
            }
            throw py::key_error("key must be a string or (string, AttributeTimestep) tuple");
        })
        // "attr" in obj -> True if the SceneClass declares that attribute
        .def("__contains__", [](const rdl2::SceneObject& self, const std::string& name) {
            try { self.getSceneClass().getAttribute(name); return true; }
            catch (...) { return false; }
        })
        // Update guard
        .def("beginUpdate", &rdl2::SceneObject::beginUpdate)
        .def("endUpdate",   &rdl2::SceneObject::endUpdate)
        // Reset
        .def("resetToDefault",
             (void (rdl2::SceneObject::*)(const std::string&)) &rdl2::SceneObject::resetToDefault,
             py::arg("name"))
        .def("resetToDefault",
             (void (rdl2::SceneObject::*)(const rdl2::Attribute*)) &rdl2::SceneObject::resetToDefault,
             py::arg("attribute"), py::return_value_policy::reference)
        .def("resetAllToDefault", &rdl2::SceneObject::resetAllToDefault)
        // Default checking
        .def("isDefault",          &isDefaultByName,          py::arg("name"))
        .def("isDefaultAndUnbound",&isDefaultAndUnboundByName, py::arg("name"))
        // Change tracking
        .def("hasChanged",        &hasAttrChanged,        py::arg("name"))
        .def("hasBindingChanged", &hasBindingAttrChanged, py::arg("name"))
        .def("isDirty",        &rdl2::SceneObject::isDirty)
        .def("requestUpdate",  &rdl2::SceneObject::requestUpdate)
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
        .def("__exit__",  [](rdl2::SceneObject::UpdateGuard&, py::object, py::object, py::object) {});
}
