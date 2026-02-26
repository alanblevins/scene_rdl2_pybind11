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
    // BinaryReader
    // -----------------------------------------------------------------------
    py::class_<rdl2::BinaryReader>(m, "BinaryReader")
        .def(py::init<rdl2::SceneContext&>(), py::arg("context"))
        .def("fromFile", &rdl2::BinaryReader::fromFile,
             py::arg("filename"))
        .def("fromBytes", [](rdl2::BinaryReader& self, py::bytes manifest, py::bytes payload) {
                std::string mstr(manifest.c_str(), manifest.size());
                std::string pstr(payload.c_str(), payload.size());
                self.fromBytes(mstr, pstr);
             },
             py::arg("manifest"), py::arg("payload"),
             "Decode RDL binary from (manifest, payload) bytes objects.")
        .def("setWarningsAsErrors", &rdl2::BinaryReader::setWarningsAsErrors,
             py::arg("warnings_as_errors"))
        .def_static("showManifest", [](py::bytes manifest) {
                std::string mstr(manifest.c_str(), manifest.size());
                return rdl2::BinaryReader::showManifest(mstr);
             },
             py::arg("manifest"),
             "Return a human-readable description of a binary manifest (debug utility).");

    // -----------------------------------------------------------------------
    // BinaryWriter
    // -----------------------------------------------------------------------
    py::class_<rdl2::BinaryWriter>(m, "BinaryWriter")
        .def(py::init<const rdl2::SceneContext&>(), py::arg("context"))
        .def("setTransientEncoding", &rdl2::BinaryWriter::setTransientEncoding,
             py::arg("transient_encoding"))
        .def("setDeltaEncoding", &rdl2::BinaryWriter::setDeltaEncoding,
             py::arg("delta_encoding"))
        .def("setSkipDefaults", &rdl2::BinaryWriter::setSkipDefaults,
             py::arg("skip_defaults"))
        .def("setSplitMode", &rdl2::BinaryWriter::setSplitMode,
             py::arg("min_vector_size"))
        .def("clearSplitMode", &rdl2::BinaryWriter::clearSplitMode)
        .def("toFile", &rdl2::BinaryWriter::toFile,
             py::arg("filename"))
        .def("toBytes", [](const rdl2::BinaryWriter& self) {
                std::string manifest, payload;
                self.toBytes(manifest, payload);
                return py::make_tuple(
                    py::bytes(manifest.c_str(), manifest.size()),
                    py::bytes(payload.c_str(), payload.size()));
             },
             "Write RDL binary and return (manifest, payload) as bytes objects.")
        .def("show", &rdl2::BinaryWriter::show,
             py::arg("indent") = "", py::arg("sort") = false,
             "Return a human-readable dump of the context (debug utility).");

    // -----------------------------------------------------------------------
    // Free functions
    // -----------------------------------------------------------------------
    m.def("attributeTypeName",
          (const char* (*)(rdl2::AttributeType)) &rdl2::attributeTypeName,
          py::arg("type"),
          "Returns the string name of an AttributeType enum value.");
}
