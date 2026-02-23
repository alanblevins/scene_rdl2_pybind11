// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for Light.

#include "bindings.h"

void bind_light(py::module_& m)
{
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
}
