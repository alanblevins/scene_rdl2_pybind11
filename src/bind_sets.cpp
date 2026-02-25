// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for scene object collection types:
//   GeometrySet, LightSet

#include "bindings.h"

void bind_sets(py::module_& m)
{
    // -----------------------------------------------------------------------
    // GeometrySet (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::GeometrySet, rdl2::SceneObject,
               std::unique_ptr<rdl2::GeometrySet, py::nodelete>>(m, "GeometrySet")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::GeometrySet* {
            auto* r = obj->asA<rdl2::GeometrySet>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to GeometrySet");
            return r;
        }), py::arg("scene_object"))
        .def("getGeometries", [](const rdl2::GeometrySet& self) {
            const rdl2::SceneObjectIndexable& idx = self.getGeometries();
            return std::vector<rdl2::SceneObject*>(idx.begin(), idx.end());
        }, py::return_value_policy::reference,
        "Returns a list of Geometry SceneObjects in this set.")
        .def("add", [](rdl2::GeometrySet& self, rdl2::Geometry* g) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.add(g);
        }, py::arg("geometry"))
        .def("remove", [](rdl2::GeometrySet& self, rdl2::Geometry* g) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.remove(g);
        }, py::arg("geometry"))
        .def("contains", &rdl2::GeometrySet::contains, py::arg("geometry"))
        .def("clear", [](rdl2::GeometrySet& self) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.clear();
        })
        .def("isStatic", &rdl2::GeometrySet::isStatic)
        .def("haveGeometriesChanged", &rdl2::GeometrySet::haveGeometriesChanged);

    // -----------------------------------------------------------------------
    // LightSet (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::LightSet, rdl2::SceneObject,
               std::unique_ptr<rdl2::LightSet, py::nodelete>>(m, "LightSet")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::LightSet* {
            auto* r = obj->asA<rdl2::LightSet>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to LightSet");
            return r;
        }), py::arg("scene_object"))
        .def("getLights", [](const rdl2::LightSet& self) {
            return self.getLights();
        }, py::return_value_policy::reference,
        "Returns a list of Light SceneObjects in this set.")
        .def("add", [](rdl2::LightSet& self, rdl2::Light* l) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.add(l);
        }, py::arg("light"))
        .def("remove", [](rdl2::LightSet& self, rdl2::Light* l) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.remove(l);
        }, py::arg("light"))
        .def("contains", &rdl2::LightSet::contains, py::arg("light"))
        .def("clear", [](rdl2::LightSet& self) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.clear();
        });

    // -----------------------------------------------------------------------
    // LightFilter (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::LightFilter, rdl2::SceneObject,
               std::unique_ptr<rdl2::LightFilter, py::nodelete>>(m, "LightFilter")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::LightFilter* {
            auto* r = obj->asA<rdl2::LightFilter>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to LightFilter");
            return r;
        }), py::arg("scene_object"))
        .def("isOn", &rdl2::LightFilter::isOn);

    // -----------------------------------------------------------------------
    // LightFilterSet (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::LightFilterSet, rdl2::SceneObject,
               std::unique_ptr<rdl2::LightFilterSet, py::nodelete>>(m, "LightFilterSet")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::LightFilterSet* {
            auto* r = obj->asA<rdl2::LightFilterSet>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to LightFilterSet");
            return r;
        }), py::arg("scene_object"))
        .def("getLightFilters", [](const rdl2::LightFilterSet& self) {
            const rdl2::SceneObjectVector& v = self.getLightFilters();
            return std::vector<rdl2::SceneObject*>(v.begin(), v.end());
        }, py::return_value_policy::reference,
        "Returns a list of LightFilter SceneObjects in this set.")
        .def("add", [](rdl2::LightFilterSet& self, rdl2::LightFilter* lf) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.add(lf);
        }, py::arg("light_filter"))
        .def("remove", [](rdl2::LightFilterSet& self, rdl2::LightFilter* lf) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.remove(lf);
        }, py::arg("light_filter"))
        .def("contains", &rdl2::LightFilterSet::contains, py::arg("light_filter"))
        .def("clear", [](rdl2::LightFilterSet& self) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.clear();
        });

    // -----------------------------------------------------------------------
    // ShadowSet (inherits LightSet)
    // -----------------------------------------------------------------------
    py::class_<rdl2::ShadowSet, rdl2::LightSet,
               std::unique_ptr<rdl2::ShadowSet, py::nodelete>>(m, "ShadowSet")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::ShadowSet* {
            auto* r = obj->asA<rdl2::ShadowSet>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to ShadowSet");
            return r;
        }), py::arg("scene_object"))
        .def("haveLightsChanged", &rdl2::ShadowSet::haveLightsChanged);

    // -----------------------------------------------------------------------
    // ShadowReceiverSet (inherits GeometrySet)
    // -----------------------------------------------------------------------
    py::class_<rdl2::ShadowReceiverSet, rdl2::GeometrySet,
               std::unique_ptr<rdl2::ShadowReceiverSet, py::nodelete>>(m, "ShadowReceiverSet")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::ShadowReceiverSet* {
            auto* r = obj->asA<rdl2::ShadowReceiverSet>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to ShadowReceiverSet");
            return r;
        }), py::arg("scene_object"))
        .def("haveGeometriesChanged", &rdl2::ShadowReceiverSet::haveGeometriesChanged);

    // -----------------------------------------------------------------------
    // DisplayFilter (inherits SceneObject)
    // getInputData / filterv use moonray-internal types and are not exposed.
    // -----------------------------------------------------------------------
    py::class_<rdl2::DisplayFilter, rdl2::SceneObject,
               std::unique_ptr<rdl2::DisplayFilter, py::nodelete>>(m, "DisplayFilter")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::DisplayFilter* {
            auto* r = obj->asA<rdl2::DisplayFilter>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to DisplayFilter");
            return r;
        }), py::arg("scene_object"));

    // -----------------------------------------------------------------------
    // Metadata (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Metadata, rdl2::SceneObject,
               std::unique_ptr<rdl2::Metadata, py::nodelete>>(m, "Metadata")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::Metadata* {
            auto* r = obj->asA<rdl2::Metadata>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to Metadata");
            return r;
        }), py::arg("scene_object"))
        .def("setAttributes", [](rdl2::Metadata& self,
                                 const std::vector<std::string>& names,
                                 const std::vector<std::string>& types,
                                 const std::vector<std::string>& values) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            rdl2::StringVector n(names), t(types), v(values);
            self.setAttributes(n, t, v);
        }, py::arg("names"), py::arg("types"), py::arg("values"),
        "Set EXR header metadata entries as parallel name/type/value lists.")
        .def("getAttributeNames",  [](const rdl2::Metadata& self) {
            return std::vector<std::string>(self.getAttributeNames().begin(),
                                           self.getAttributeNames().end());
        })
        .def("getAttributeTypes",  [](const rdl2::Metadata& self) {
            return std::vector<std::string>(self.getAttributeTypes().begin(),
                                           self.getAttributeTypes().end());
        })
        .def("getAttributeValues", [](const rdl2::Metadata& self) {
            return std::vector<std::string>(self.getAttributeValues().begin(),
                                           self.getAttributeValues().end());
        });

    // -----------------------------------------------------------------------
    // TraceSet (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::TraceSet, rdl2::SceneObject,
               std::unique_ptr<rdl2::TraceSet, py::nodelete>>(m, "TraceSet")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::TraceSet* {
            auto* r = obj->asA<rdl2::TraceSet>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to TraceSet");
            return r;
        }), py::arg("scene_object"))
        .def("getAssignmentCount", &rdl2::TraceSet::getAssignmentCount,
             "Returns the number of Geometry/Part assignments in this TraceSet.")
        .def("assign", [](rdl2::TraceSet& self, rdl2::Geometry* g, const std::string& part) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            return self.assign(g, part);
        }, py::arg("geometry"), py::arg("part_name"),
        "Add a Geometry/Part pair and return its assignment ID.")
        .def("lookupGeomAndPart", [](const rdl2::TraceSet& self, int32_t assignmentId) {
            auto pair = self.lookupGeomAndPart(assignmentId);
            return py::make_tuple(
                py::cast(pair.first, py::return_value_policy::reference),
                std::string(pair.second));
        }, py::arg("assignment_id"),
        "Return (Geometry, part_name) for a given assignment ID.")
        .def("getAssignmentId", &rdl2::TraceSet::getAssignmentId,
             py::arg("geometry"), py::arg("part_name"),
             "Return the assignment ID for a Geometry/Part pair, or -1 if not found.")
        .def("contains", &rdl2::TraceSet::contains,
             py::arg("geometry"),
             "Return True if the given Geometry appears in this TraceSet.")
        .def("getAssignmentIds", [](const rdl2::TraceSet& self,
                                    const rdl2::Geometry* geometry) {
            std::vector<int32_t> ids;
            for (auto it = self.begin(geometry); it != self.end(geometry); ++it)
                ids.push_back(*it);
            return ids;
        }, py::arg("geometry"),
        "Return a list of all assignment IDs for the given Geometry.");

    // -----------------------------------------------------------------------
    // UserData (inherits SceneObject)
    // Typed key/value channels for passing primitive attributes through rdl2.
    // Bool, Int, String: single-timestep only.
    // Float, Color, Vec2f, Vec3f, Mat4f: support dual-timestep (blur).
    // -----------------------------------------------------------------------
    py::class_<rdl2::UserData, rdl2::SceneObject,
               std::unique_ptr<rdl2::UserData, py::nodelete>> ud(m, "UserData");

    py::enum_<rdl2::UserData::Rate>(ud, "Rate")
        .value("AUTO",         rdl2::UserData::AUTO)
        .value("CONSTANT",     rdl2::UserData::CONSTANT)
        .value("PART",         rdl2::UserData::PART)
        .value("UNIFORM",      rdl2::UserData::UNIFORM)
        .value("VERTEX",       rdl2::UserData::VERTEX)
        .value("VARYING",      rdl2::UserData::VARYING)
        .value("FACE_VARYING", rdl2::UserData::FACE_VARYING)
        .export_values();

    ud
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::UserData* {
            auto* r = obj->asA<rdl2::UserData>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to UserData");
            return r;
        }), py::arg("scene_object"))
        .def("setRate", [](rdl2::UserData& self, rdl2::UserData::Rate rate) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setRate(static_cast<int>(rate));
        }, py::arg("rate"))
        .def("getRate", [](const rdl2::UserData& self) {
            return static_cast<rdl2::UserData::Rate>(self.getRate());
        })
        // Bool (single timestep)
        .def("hasBoolData",  &rdl2::UserData::hasBoolData)
        .def("setBoolData", [](rdl2::UserData& self, const std::string& key,
                               const rdl2::BoolVector& values) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setBoolData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("getBoolKey",    &rdl2::UserData::getBoolKey)
        .def("getBoolValues", &rdl2::UserData::getBoolValues)
        // Int (single timestep)
        .def("hasIntData",  &rdl2::UserData::hasIntData)
        .def("setIntData", [](rdl2::UserData& self, const std::string& key,
                              const rdl2::IntVector& values) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setIntData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("getIntKey",    &rdl2::UserData::getIntKey)
        .def("getIntValues", &rdl2::UserData::getIntValues)
        // Float (dual timestep)
        .def("hasFloatData",  &rdl2::UserData::hasFloatData)
        .def("hasFloatData0", &rdl2::UserData::hasFloatData0)
        .def("hasFloatData1", &rdl2::UserData::hasFloatData1)
        .def("setFloatData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::FloatVector& values) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setFloatData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("setFloatData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::FloatVector& values0,
                                const rdl2::FloatVector& values1) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setFloatData(key, values0, values1);
        }, py::arg("key"), py::arg("values0"), py::arg("values1"))
        .def("getFloatKey",    &rdl2::UserData::getFloatKey)
        .def("getFloatValues", &rdl2::UserData::getFloatValues)
        .def("getFloatValues0",&rdl2::UserData::getFloatValues0)
        .def("getFloatValues1",&rdl2::UserData::getFloatValues1)
        // String (single timestep)
        .def("hasStringData",  &rdl2::UserData::hasStringData)
        .def("setStringData", [](rdl2::UserData& self, const std::string& key,
                                 const rdl2::StringVector& values) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setStringData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("getStringKey",    &rdl2::UserData::getStringKey)
        .def("getStringValues", &rdl2::UserData::getStringValues)
        // Color / Rgb (dual timestep)
        .def("hasColorData",  &rdl2::UserData::hasColorData)
        .def("hasColorData0", &rdl2::UserData::hasColorData0)
        .def("hasColorData1", &rdl2::UserData::hasColorData1)
        .def("setColorData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::RgbVector& values) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setColorData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("setColorData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::RgbVector& values0,
                                const rdl2::RgbVector& values1) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setColorData(key, values0, values1);
        }, py::arg("key"), py::arg("values0"), py::arg("values1"))
        .def("getColorKey",    &rdl2::UserData::getColorKey)
        .def("getColorValues", &rdl2::UserData::getColorValues)
        .def("getColorValues0",&rdl2::UserData::getColorValues0)
        .def("getColorValues1",&rdl2::UserData::getColorValues1)
        // Vec2f (dual timestep)
        .def("hasVec2fData",  &rdl2::UserData::hasVec2fData)
        .def("hasVec2fData0", &rdl2::UserData::hasVec2fData0)
        .def("hasVec2fData1", &rdl2::UserData::hasVec2fData1)
        .def("setVec2fData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::Vec2fVector& values) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setVec2fData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("setVec2fData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::Vec2fVector& values0,
                                const rdl2::Vec2fVector& values1) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setVec2fData(key, values0, values1);
        }, py::arg("key"), py::arg("values0"), py::arg("values1"))
        .def("getVec2fKey",    &rdl2::UserData::getVec2fKey)
        .def("getVec2fValues", &rdl2::UserData::getVec2fValues)
        .def("getVec2fValues0",&rdl2::UserData::getVec2fValues0)
        .def("getVec2fValues1",&rdl2::UserData::getVec2fValues1)
        // Vec3f (dual timestep)
        .def("hasVec3fData",  &rdl2::UserData::hasVec3fData)
        .def("hasVec3fData0", &rdl2::UserData::hasVec3fData0)
        .def("hasVec3fData1", &rdl2::UserData::hasVec3fData1)
        .def("setVec3fData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::Vec3fVector& values) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setVec3fData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("setVec3fData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::Vec3fVector& values0,
                                const rdl2::Vec3fVector& values1) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setVec3fData(key, values0, values1);
        }, py::arg("key"), py::arg("values0"), py::arg("values1"))
        .def("getVec3fKey",    &rdl2::UserData::getVec3fKey)
        .def("getVec3fValues", &rdl2::UserData::getVec3fValues)
        .def("getVec3fValues0",&rdl2::UserData::getVec3fValues0)
        .def("getVec3fValues1",&rdl2::UserData::getVec3fValues1)
        // Mat4f (dual timestep)
        .def("hasMat4fData",  &rdl2::UserData::hasMat4fData)
        .def("hasMat4fData0", &rdl2::UserData::hasMat4fData0)
        .def("hasMat4fData1", &rdl2::UserData::hasMat4fData1)
        .def("setMat4fData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::Mat4fVector& values) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setMat4fData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("setMat4fData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::Mat4fVector& values0,
                                const rdl2::Mat4fVector& values1) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setMat4fData(key, values0, values1);
        }, py::arg("key"), py::arg("values0"), py::arg("values1"))
        .def("getMat4fKey",    &rdl2::UserData::getMat4fKey)
        .def("getMat4fValues", &rdl2::UserData::getMat4fValues)
        .def("getMat4fValues0",&rdl2::UserData::getMat4fValues0)
        .def("getMat4fValues1",&rdl2::UserData::getMat4fValues1);
}
