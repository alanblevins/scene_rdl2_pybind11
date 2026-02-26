// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for scene_rdl2 standalone enum types:
//   AttributeType, AttributeFlags, AttributeTimestep, SceneObjectInterface,
//   MotionBlurType, PrimitiveAttributeFrame, PixelFilterType,
//   TaskDistributionType, VolumeOverlapMode, ShadowTerminatorFix,
//   TextureFilterType

#include "bindings.h"

void bind_types(py::module_& m)
{
    py::enum_<rdl2::AttributeType>(m, "AttributeType")
        .value("TYPE_UNKNOWN",                rdl2::TYPE_UNKNOWN)
        .value("TYPE_BOOL",                   rdl2::TYPE_BOOL)
        .value("TYPE_INT",                    rdl2::TYPE_INT)
        .value("TYPE_LONG",                   rdl2::TYPE_LONG)
        .value("TYPE_FLOAT",                  rdl2::TYPE_FLOAT)
        .value("TYPE_DOUBLE",                 rdl2::TYPE_DOUBLE)
        .value("TYPE_STRING",                 rdl2::TYPE_STRING)
        .value("TYPE_RGB",                    rdl2::TYPE_RGB)
        .value("TYPE_RGBA",                   rdl2::TYPE_RGBA)
        .value("TYPE_VEC2F",                  rdl2::TYPE_VEC2F)
        .value("TYPE_VEC2D",                  rdl2::TYPE_VEC2D)
        .value("TYPE_VEC3F",                  rdl2::TYPE_VEC3F)
        .value("TYPE_VEC3D",                  rdl2::TYPE_VEC3D)
        .value("TYPE_VEC4F",                  rdl2::TYPE_VEC4F)
        .value("TYPE_VEC4D",                  rdl2::TYPE_VEC4D)
        .value("TYPE_MAT4F",                  rdl2::TYPE_MAT4F)
        .value("TYPE_MAT4D",                  rdl2::TYPE_MAT4D)
        .value("TYPE_SCENE_OBJECT",           rdl2::TYPE_SCENE_OBJECT)
        .value("TYPE_BOOL_VECTOR",            rdl2::TYPE_BOOL_VECTOR)
        .value("TYPE_INT_VECTOR",             rdl2::TYPE_INT_VECTOR)
        .value("TYPE_LONG_VECTOR",            rdl2::TYPE_LONG_VECTOR)
        .value("TYPE_FLOAT_VECTOR",           rdl2::TYPE_FLOAT_VECTOR)
        .value("TYPE_DOUBLE_VECTOR",          rdl2::TYPE_DOUBLE_VECTOR)
        .value("TYPE_STRING_VECTOR",          rdl2::TYPE_STRING_VECTOR)
        .value("TYPE_RGB_VECTOR",             rdl2::TYPE_RGB_VECTOR)
        .value("TYPE_RGBA_VECTOR",            rdl2::TYPE_RGBA_VECTOR)
        .value("TYPE_VEC2F_VECTOR",           rdl2::TYPE_VEC2F_VECTOR)
        .value("TYPE_VEC2D_VECTOR",           rdl2::TYPE_VEC2D_VECTOR)
        .value("TYPE_VEC3F_VECTOR",           rdl2::TYPE_VEC3F_VECTOR)
        .value("TYPE_VEC3D_VECTOR",           rdl2::TYPE_VEC3D_VECTOR)
        .value("TYPE_VEC4F_VECTOR",           rdl2::TYPE_VEC4F_VECTOR)
        .value("TYPE_VEC4D_VECTOR",           rdl2::TYPE_VEC4D_VECTOR)
        .value("TYPE_MAT4F_VECTOR",           rdl2::TYPE_MAT4F_VECTOR)
        .value("TYPE_MAT4D_VECTOR",           rdl2::TYPE_MAT4D_VECTOR)
        .value("TYPE_SCENE_OBJECT_VECTOR",    rdl2::TYPE_SCENE_OBJECT_VECTOR)
        .value("TYPE_SCENE_OBJECT_INDEXABLE", rdl2::TYPE_SCENE_OBJECT_INDEXABLE)
        .export_values();

    py::enum_<rdl2::AttributeFlags>(m, "AttributeFlags", py::is_arithmetic{})
        .value("FLAGS_NONE",                rdl2::FLAGS_NONE)
        .value("FLAGS_BINDABLE",            rdl2::FLAGS_BINDABLE)
        .value("FLAGS_BLURRABLE",           rdl2::FLAGS_BLURRABLE)
        .value("FLAGS_ENUMERABLE",          rdl2::FLAGS_ENUMERABLE)
        .value("FLAGS_FILENAME",            rdl2::FLAGS_FILENAME)
        .value("FLAGS_CAN_SKIP_GEOM_RELOAD",rdl2::FLAGS_CAN_SKIP_GEOM_RELOAD)
        .export_values();

    py::enum_<rdl2::AttributeTimestep>(m, "AttributeTimestep")
        .value("TIMESTEP_BEGIN", rdl2::TIMESTEP_BEGIN)
        .value("TIMESTEP_END",   rdl2::TIMESTEP_END)
        .value("NUM_TIMESTEPS",  rdl2::NUM_TIMESTEPS)
        .export_values()
        .def("__int__",   [](rdl2::AttributeTimestep e){ return static_cast<int>(e); })
        .def("__index__", [](rdl2::AttributeTimestep e){ return static_cast<int>(e); });

    py::enum_<rdl2::SceneObjectInterface>(m, "SceneObjectInterface", py::is_arithmetic{})
        .value("INTERFACE_GENERIC",           rdl2::INTERFACE_GENERIC)
        .value("INTERFACE_GEOMETRYSET",       rdl2::INTERFACE_GEOMETRYSET)
        .value("INTERFACE_LAYER",             rdl2::INTERFACE_LAYER)
        .value("INTERFACE_LIGHTSET",          rdl2::INTERFACE_LIGHTSET)
        .value("INTERFACE_NODE",              rdl2::INTERFACE_NODE)
        .value("INTERFACE_CAMERA",            rdl2::INTERFACE_CAMERA)
        .value("INTERFACE_ENVMAP",            rdl2::INTERFACE_ENVMAP)
        .value("INTERFACE_GEOMETRY",          rdl2::INTERFACE_GEOMETRY)
        .value("INTERFACE_LIGHT",             rdl2::INTERFACE_LIGHT)
        .value("INTERFACE_SHADER",            rdl2::INTERFACE_SHADER)
        .value("INTERFACE_DISPLACEMENT",      rdl2::INTERFACE_DISPLACEMENT)
        .value("INTERFACE_MAP",               rdl2::INTERFACE_MAP)
        .value("INTERFACE_ROOTSHADER",        rdl2::INTERFACE_ROOTSHADER)
        .value("INTERFACE_MATERIAL",          rdl2::INTERFACE_MATERIAL)
        .value("INTERFACE_VOLUMESHADER",      rdl2::INTERFACE_VOLUMESHADER)
        .value("INTERFACE_RENDEROUTPUT",      rdl2::INTERFACE_RENDEROUTPUT)
        .value("INTERFACE_USERDATA",          rdl2::INTERFACE_USERDATA)
        .value("INTERFACE_METADATA",          rdl2::INTERFACE_METADATA)
        .value("INTERFACE_LIGHTFILTER",       rdl2::INTERFACE_LIGHTFILTER)
        .value("INTERFACE_TRACESET",          rdl2::INTERFACE_TRACESET)
        .value("INTERFACE_JOINT",             rdl2::INTERFACE_JOINT)
        .value("INTERFACE_LIGHTFILTERSET",    rdl2::INTERFACE_LIGHTFILTERSET)
        .value("INTERFACE_SHADOWSET",         rdl2::INTERFACE_SHADOWSET)
        .value("INTERFACE_NORMALMAP",         rdl2::INTERFACE_NORMALMAP)
        .value("INTERFACE_DISPLAYFILTER",     rdl2::INTERFACE_DISPLAYFILTER)
        .value("INTERFACE_SHADOWRECEIVERSET", rdl2::INTERFACE_SHADOWRECEIVERSET)
        .export_values();

    // Geometry enums
    py::enum_<rdl2::MotionBlurType>(m, "MotionBlurType")
        .value("STATIC",           rdl2::MotionBlurType::STATIC)
        .value("VELOCITY",         rdl2::MotionBlurType::VELOCITY)
        .value("FRAME_DELTA",      rdl2::MotionBlurType::FRAME_DELTA)
        .value("ACCELERATION",     rdl2::MotionBlurType::ACCELERATION)
        .value("HERMITE",          rdl2::MotionBlurType::HERMITE)
        .value("STATIC_DUPLICATE", rdl2::MotionBlurType::STATIC_DUPLICATE)
        .value("BEST",             rdl2::MotionBlurType::BEST)
        .export_values();

    py::enum_<rdl2::PrimitiveAttributeFrame>(m, "PrimitiveAttributeFrame")
        .value("FIRST_MOTION_STEP",  rdl2::PrimitiveAttributeFrame::FIRST_MOTION_STEP)
        .value("SECOND_MOTION_STEP", rdl2::PrimitiveAttributeFrame::SECOND_MOTION_STEP)
        .value("BOTH_MOTION_STEPS",  rdl2::PrimitiveAttributeFrame::BOTH_MOTION_STEPS)
        .export_values();

    // SceneVariables enums
    py::enum_<rdl2::PixelFilterType>(m, "PixelFilterType")
        .value("box",              rdl2::PixelFilterType::box)
        .value("cubicBSpline",     rdl2::PixelFilterType::cubicBSpline)
        .value("quadraticBSpline", rdl2::PixelFilterType::quadraticBSpline)
        .export_values();

    py::enum_<rdl2::TaskDistributionType>(m, "TaskDistributionType")
        .value("NON_OVERLAPPED_TILE", rdl2::TaskDistributionType::NON_OVERLAPPED_TILE)
        .value("MULTIPLEX_PIXEL",     rdl2::TaskDistributionType::MULTIPLEX_PIXEL)
        .export_values();

    py::enum_<rdl2::VolumeOverlapMode>(m, "VolumeOverlapMode")
        .value("SUM", rdl2::VolumeOverlapMode::SUM)
        .value("MAX", rdl2::VolumeOverlapMode::MAX)
        .value("RND", rdl2::VolumeOverlapMode::RND)
        .export_values();

    py::enum_<rdl2::ShadowTerminatorFix>(m, "ShadowTerminatorFix")
        .value("OFF",                rdl2::ShadowTerminatorFix::OFF)
        .value("CUSTOM",             rdl2::ShadowTerminatorFix::CUSTOM)
        .value("SINE_COMPENSATION",  rdl2::ShadowTerminatorFix::SINE_COMPENSATION)
        .value("GGX",                rdl2::ShadowTerminatorFix::GGX)
        .value("COSINE_COMPENSATION",rdl2::ShadowTerminatorFix::COSINE_COMPENSATION)
        .export_values();

    // Light enum
    py::enum_<rdl2::TextureFilterType>(m, "TextureFilterType")
        .value("TEXTURE_FILTER_NEAREST",             rdl2::TextureFilterType::TEXTURE_FILTER_NEAREST)
        .value("TEXTURE_FILTER_BILINEAR",            rdl2::TextureFilterType::TEXTURE_FILTER_BILINEAR)
        .value("TEXTURE_FILTER_NEAREST_MIP_NEAREST", rdl2::TextureFilterType::TEXTURE_FILTER_NEAREST_MIP_NEAREST)
        .value("TEXTURE_FILTER_BILINEAR_MIP_NEAREST",rdl2::TextureFilterType::TEXTURE_FILTER_BILINEAR_MIP_NEAREST)
        .export_values();
}
