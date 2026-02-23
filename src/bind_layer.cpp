// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for LayerAssignment and Layer.

#include "bindings.h"

void bind_layer(py::module_& m)
{
    // -----------------------------------------------------------------------
    // LayerAssignment (plain struct)
    // -----------------------------------------------------------------------
    py::class_<rdl2::LayerAssignment>(m, "LayerAssignment")
        .def(py::init<>())
        .def_readwrite("material",          &rdl2::LayerAssignment::mMaterial)
        .def_readwrite("lightSet",          &rdl2::LayerAssignment::mLightSet)
        .def_readwrite("displacement",      &rdl2::LayerAssignment::mDisplacement)
        .def_readwrite("volumeShader",      &rdl2::LayerAssignment::mVolumeShader)
        .def_readwrite("lightFilterSet",    &rdl2::LayerAssignment::mLightFilterSet)
        .def_readwrite("shadowSet",         &rdl2::LayerAssignment::mShadowSet)
        .def_readwrite("shadowReceiverSet", &rdl2::LayerAssignment::mShadowReceiverSet);

    // -----------------------------------------------------------------------
    // Layer (inherits SceneObject; Layer inherits via TraceSet in the C++ lib,
    // but we bind it directly under SceneObject for simplicity)
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
        .def("lookupMaterial",         &rdl2::Layer::lookupMaterial,
             py::arg("assignment_id"), py::return_value_policy::reference)
        .def("lookupLightSet",         &rdl2::Layer::lookupLightSet,
             py::arg("assignment_id"), py::return_value_policy::reference)
        .def("lookupDisplacement",     &rdl2::Layer::lookupDisplacement,
             py::arg("assignment_id"), py::return_value_policy::reference)
        .def("lookupVolumeShader",     &rdl2::Layer::lookupVolumeShader,
             py::arg("assignment_id"), py::return_value_policy::reference)
        .def("lookupLightFilterSet",   &rdl2::Layer::lookupLightFilterSet,
             py::arg("assignment_id"), py::return_value_policy::reference)
        .def("lookupShadowSet",        &rdl2::Layer::lookupShadowSet,
             py::arg("assignment_id"), py::return_value_policy::reference)
        .def("lookupShadowReceiverSet",&rdl2::Layer::lookupShadowReceiverSet,
             py::arg("assignment_id"), py::return_value_policy::reference)
        .def("clear",            &rdl2::Layer::clear)
        .def("lightSetsChanged", &rdl2::Layer::lightSetsChanged);
}
