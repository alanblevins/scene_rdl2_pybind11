// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for LayerAssignment and Layer.

#include "bindings.h"

#define DEF_DOWNCAST_CTOR(CLS, NM)                                          \
    .def_static("__new__",                                                  \
        [](py::handle type, rdl2::SceneObject* obj) -> py::object {        \
            auto* r = obj->asA<rdl2::CLS>();                                \
            if (!r) throw py::type_error(                                   \
                ("cannot cast '" + obj->getSceneClass().getName() +         \
                 "' to " NM).c_str());                                      \
            return py::inst_reference(type, r);                             \
        }, py::arg("type"), py::arg("scene_object"))                        \
    .def("__init__", [](py::object, rdl2::SceneObject*) {},                 \
         py::arg("scene_object"))

void bind_layer(py::module_& m)
{
    // -----------------------------------------------------------------------
    // LayerAssignment (plain struct)
    // -----------------------------------------------------------------------
    py::class_<rdl2::LayerAssignment>(m, "LayerAssignment")
        .def(py::init<>())
        .def_rw("material",          &rdl2::LayerAssignment::mMaterial)
        .def_rw("lightSet",          &rdl2::LayerAssignment::mLightSet)
        .def_rw("displacement",      &rdl2::LayerAssignment::mDisplacement)
        .def_rw("volumeShader",      &rdl2::LayerAssignment::mVolumeShader)
        .def_rw("lightFilterSet",    &rdl2::LayerAssignment::mLightFilterSet)
        .def_rw("shadowSet",         &rdl2::LayerAssignment::mShadowSet)
        .def_rw("shadowReceiverSet", &rdl2::LayerAssignment::mShadowReceiverSet);

    // -----------------------------------------------------------------------
    // Layer (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Layer, rdl2::SceneObject>(m, "Layer")
        DEF_DOWNCAST_CTOR(Layer, "Layer")
        .def("assign", [](rdl2::Layer& self, rdl2::Geometry* g, const std::string& part,
                          rdl2::Material* mat, rdl2::LightSet* ls) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            return self.assign(g, part, mat, ls);
        }, py::arg("geometry"), py::arg("part_name"),
           py::arg("material"), py::arg("light_set"))
        .def("assign", [](rdl2::Layer& self, rdl2::Geometry* g, const std::string& part,
                          rdl2::Material* mat, rdl2::LightSet* ls,
                          rdl2::Displacement* disp, rdl2::VolumeShader* vs) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            return self.assign(g, part, mat, ls, disp, vs);
        }, py::arg("geometry"), py::arg("part_name"),
           py::arg("material"), py::arg("light_set"),
           py::arg("displacement"), py::arg("volume_shader"))
        .def("assign", [](rdl2::Layer& self, rdl2::Geometry* g, const std::string& part,
                          const rdl2::LayerAssignment& a) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            return self.assign(g, part, a);
        }, py::arg("geometry"), py::arg("part_name"), py::arg("assignment"))
        .def("lookupMaterial",         &rdl2::Layer::lookupMaterial,
             py::arg("assignment_id"), py::rv_policy::reference)
        .def("lookupLightSet",         &rdl2::Layer::lookupLightSet,
             py::arg("assignment_id"), py::rv_policy::reference)
        .def("lookupDisplacement",     &rdl2::Layer::lookupDisplacement,
             py::arg("assignment_id"), py::rv_policy::reference)
        .def("lookupVolumeShader",     &rdl2::Layer::lookupVolumeShader,
             py::arg("assignment_id"), py::rv_policy::reference)
        .def("lookupLightFilterSet",   &rdl2::Layer::lookupLightFilterSet,
             py::arg("assignment_id"), py::rv_policy::reference)
        .def("lookupShadowSet",        &rdl2::Layer::lookupShadowSet,
             py::arg("assignment_id"), py::rv_policy::reference)
        .def("lookupShadowReceiverSet",&rdl2::Layer::lookupShadowReceiverSet,
             py::arg("assignment_id"), py::rv_policy::reference)
        .def("clear", [](rdl2::Layer& self) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.clear();
        })
        .def("lightSetsChanged", &rdl2::Layer::lightSetsChanged);
}

#undef DEF_DOWNCAST_CTOR
