// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for Attribute and SceneClass.

#include "bindings.h"

static std::vector<const rdl2::Attribute*> getAttributeList(const rdl2::SceneClass& sc)
{
    std::vector<const rdl2::Attribute*> result;
    for (auto it = sc.beginAttributes(); it != sc.endAttributes(); ++it)
        result.push_back(*it);
    return result;
}

static std::vector<std::string> getGroupNames(const rdl2::SceneClass& sc)
{
    std::vector<std::string> result;
    for (auto it = sc.beginGroups(); it != sc.endGroups(); ++it)
        result.push_back(*it);
    return result;
}

void bind_attribute(py::module_& m)
{
    // -----------------------------------------------------------------------
    // Attribute (non-copyable; only returned by reference from SceneClass)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Attribute>(m, "Attribute")
        .def("getName",    &rdl2::Attribute::getName,
             py::rv_policy::reference)
        .def("getAliases", &rdl2::Attribute::getAliases)
        .def("getType",    &rdl2::Attribute::getType)
        .def("getObjectType", &rdl2::Attribute::getObjectType)
        .def("getFlags",   &rdl2::Attribute::getFlags)
        .def("isBindable", &rdl2::Attribute::isBindable)
        .def("isBlurrable",&rdl2::Attribute::isBlurrable)
        .def("isEnumerable",&rdl2::Attribute::isEnumerable)
        .def("isFilename", &rdl2::Attribute::isFilename)
        .def("updateRequiresGeomReload", &rdl2::Attribute::updateRequiresGeomReload)
        .def("getMetadata",    &rdl2::Attribute::getMetadata)
        .def("setMetadata",    &rdl2::Attribute::setMetadata)
        .def("metadataExists", &rdl2::Attribute::metadataExists)
        .def("metadataEmpty",  &rdl2::Attribute::metadataEmpty)
        .def("getMetadataDict", [](const rdl2::Attribute& self) {
            std::map<std::string, std::string> result;
            for (auto it = self.beginMetadata(); it != self.endMetadata(); ++it)
                result[it->first] = it->second;
            return result;
        }, "Returns all metadata as a dict.")
        .def("getEnumDescription", &rdl2::Attribute::getEnumDescription)
        .def("setEnumValue",       &rdl2::Attribute::setEnumValue)
        .def("getEnumValue",       &rdl2::Attribute::getEnumValue)
        .def("isValidEnumValue",   &rdl2::Attribute::isValidEnumValue)
        .def("getEnumValuesDict",  [](const rdl2::Attribute& self) {
            std::map<int, std::string> result;
            for (auto it = self.beginEnumValues(); it != self.endEnumValues(); ++it)
                result[it->first] = it->second;
            return result;
        }, "Returns all enum values as a dict mapping Int -> description.")
        .def("show", &rdl2::Attribute::show)
        .def("__repr__", [](const rdl2::Attribute& a) {
            return "<Attribute name='" + a.getName() + "'>";
        });

    // -----------------------------------------------------------------------
    // SceneClass
    // -----------------------------------------------------------------------
    py::class_<rdl2::SceneClass>(m, "SceneClass")
        .def("getName", &rdl2::SceneClass::getName,
             py::rv_policy::reference)
        .def("getDeclaredInterface", [](const rdl2::SceneClass& sc) {
            return static_cast<int>(sc.getDeclaredInterface());
        })
        .def("hasAttribute", &rdl2::SceneClass::hasAttribute)
        .def("getAttribute",
             (const rdl2::Attribute* (rdl2::SceneClass::*)(const std::string&) const)
             &rdl2::SceneClass::getAttribute,
             py::rv_policy::reference)
        .def("getAttributeGroup", &rdl2::SceneClass::getAttributeGroup,
             py::rv_policy::reference)
        .def("getGroupNames",  &getGroupNames)
        .def("getAttributes",  &getAttributeList,
             py::rv_policy::reference,
             "Returns a list of all Attribute objects in this SceneClass.")
        .def("getSourcePath",  &rdl2::SceneClass::getSourcePath)
        .def("showAllAttributes", &rdl2::SceneClass::showAllAttributes)
        .def("getSceneContext",
             (const rdl2::SceneContext* (rdl2::SceneClass::*)() const)
             &rdl2::SceneClass::getSceneContext,
             py::rv_policy::reference)
        .def("__repr__", [](const rdl2::SceneClass& sc) {
            return "<SceneClass name='" + sc.getName() + "'>";
        });
}
