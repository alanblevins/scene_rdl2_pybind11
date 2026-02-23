// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for AsciiReader, AsciiWriter, and module-level free functions.

#include "bindings.h"

void bind_io(py::module_& m)
{
    // -----------------------------------------------------------------------
    // AsciiReader
    // -----------------------------------------------------------------------
    py::class_<rdl2::AsciiReader>(m, "AsciiReader")
        .def(py::init<rdl2::SceneContext&>(), py::arg("context"))
        .def("fromFile",   &rdl2::AsciiReader::fromFile, py::arg("filename"))
        .def("fromString", &rdl2::AsciiReader::fromString,
             py::arg("code"), py::arg("chunk_name") = "@rdla")
        .def("setWarningsAsErrors", &rdl2::AsciiReader::setWarningsAsErrors,
             py::arg("warnings_as_errors"));

    // -----------------------------------------------------------------------
    // AsciiWriter
    // -----------------------------------------------------------------------
    py::class_<rdl2::AsciiWriter>(m, "AsciiWriter")
        .def(py::init<const rdl2::SceneContext&>(), py::arg("context"))
        .def("setDeltaEncoding",  &rdl2::AsciiWriter::setDeltaEncoding,
             py::arg("delta_encoding"))
        .def("setSkipDefaults",   &rdl2::AsciiWriter::setSkipDefaults,
             py::arg("skip_defaults"))
        .def("setElementsPerLine",&rdl2::AsciiWriter::setElementsPerLine,
             py::arg("elements_per_line"))
        .def("toFile",   &rdl2::AsciiWriter::toFile, py::arg("filename"))
        .def("toString", &rdl2::AsciiWriter::toString);

    // -----------------------------------------------------------------------
    // Free functions
    // -----------------------------------------------------------------------
    m.def("attributeTypeName",
          (const char* (*)(rdl2::AttributeType)) &rdl2::attributeTypeName,
          py::arg("type"),
          "Returns the string name of an AttributeType enum value.");
}
