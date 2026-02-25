// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for RenderOutput and its nested enums.

#include "bindings.h"

void bind_render_output(py::module_& m)
{
    // -----------------------------------------------------------------------
    // RenderOutput (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::RenderOutput, rdl2::SceneObject,
               std::unique_ptr<rdl2::RenderOutput, py::nodelete>>(m, "RenderOutput")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::RenderOutput* {
            auto* r = obj->asA<rdl2::RenderOutput>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to RenderOutput");
            return r;
        }), py::arg("scene_object"))
        .def("getActive",               &rdl2::RenderOutput::getActive)
        .def("getResult",               &rdl2::RenderOutput::getResult)
        .def("getOutputType",           &rdl2::RenderOutput::getOutputType)
        .def("getStateVariable",        &rdl2::RenderOutput::getStateVariable)
        .def("getPrimitiveAttribute",   &rdl2::RenderOutput::getPrimitiveAttribute)
        .def("getPrimitiveAttributeType",&rdl2::RenderOutput::getPrimitiveAttributeType)
        .def("getMaterialAov",          &rdl2::RenderOutput::getMaterialAov)
        .def("getLpe",                  &rdl2::RenderOutput::getLpe)
        .def("getVisibilityAov",        &rdl2::RenderOutput::getVisibilityAov)
        .def("getFileName",             &rdl2::RenderOutput::getFileName)
        .def("getFilePart",             &rdl2::RenderOutput::getFilePart)
        .def("getCompression",          &rdl2::RenderOutput::getCompression)
        .def("getCompressionLevel",     &rdl2::RenderOutput::getCompressionLevel)
        .def("getChannelName",          &rdl2::RenderOutput::getChannelName)
        .def("getChannelSuffixMode",    &rdl2::RenderOutput::getChannelSuffixMode)
        .def("getChannelFormat",        &rdl2::RenderOutput::getChannelFormat)
        .def("getMathFilter",           &rdl2::RenderOutput::getMathFilter)
        .def("getExrHeaderAttributes",  &rdl2::RenderOutput::getExrHeaderAttributes)
        .def("getDenoiserInput",        &rdl2::RenderOutput::getDenoiserInput)
        .def("getDenoise",              &rdl2::RenderOutput::getDenoise)
        .def("getCheckpointFileName",   &rdl2::RenderOutput::getCheckpointFileName)
        .def("getCheckpointMultiVersionFileName", &rdl2::RenderOutput::getCheckpointMultiVersionFileName)
        .def("getResumeFileName",       &rdl2::RenderOutput::getResumeFileName)
        .def("getCryptomatteDepth",     &rdl2::RenderOutput::getCryptomatteDepth)
        .def("getCryptomatteNumLayers", &rdl2::RenderOutput::getCryptomatteNumLayers)
        .def("getCryptomatteOutputPositions", &rdl2::RenderOutput::getCryptomatteOutputPositions)
        .def("getCryptomatteOutputP0",        &rdl2::RenderOutput::getCryptomatteOutputP0)
        .def("getCryptomatteOutputNormals",   &rdl2::RenderOutput::getCryptomatteOutputNormals)
        .def("getCryptomatteOutputBeauty",    &rdl2::RenderOutput::getCryptomatteOutputBeauty)
        .def("getCryptomatteOutputRefP",      &rdl2::RenderOutput::getCryptomatteOutputRefP)
        .def("getCryptomatteOutputRefN",      &rdl2::RenderOutput::getCryptomatteOutputRefN)
        .def("getCryptomatteOutputUV",        &rdl2::RenderOutput::getCryptomatteOutputUV)
        .def("getCryptomatteSupportResumeRender", &rdl2::RenderOutput::getCryptomatteSupportResumeRender)
        .def("getCryptomatteRecordReflected", &rdl2::RenderOutput::getCryptomatteRecordReflected)
        .def("getCryptomatteRecordRefracted", &rdl2::RenderOutput::getCryptomatteRecordRefracted)
        .def("getCryptomatteNumExtraChannels",&rdl2::RenderOutput::getCryptomatteNumExtraChannels)
        .def("cryptomatteHasExtraOutput",    &rdl2::RenderOutput::cryptomatteHasExtraOutput)
        .def("getCamera", &rdl2::RenderOutput::getCamera,
             py::return_value_policy::reference);

    // -----------------------------------------------------------------------
    // RenderOutput nested enums
    // -----------------------------------------------------------------------
    py::enum_<rdl2::RenderOutput::ChannelFormat>(m, "ChannelFormat")
        .value("CHANNEL_FORMAT_FLOAT", rdl2::RenderOutput::CHANNEL_FORMAT_FLOAT)
        .value("CHANNEL_FORMAT_HALF",  rdl2::RenderOutput::CHANNEL_FORMAT_HALF)
        .export_values();

    py::enum_<rdl2::RenderOutput::Compression>(m, "Compression")
        .value("COMPRESSION_NONE",  rdl2::RenderOutput::COMPRESSION_NONE)
        .value("COMPRESSION_ZIP",   rdl2::RenderOutput::COMPRESSION_ZIP)
        .value("COMPRESSION_RLE",   rdl2::RenderOutput::COMPRESSION_RLE)
        .value("COMPRESSION_ZIPS",  rdl2::RenderOutput::COMPRESSION_ZIPS)
        .value("COMPRESSION_PIZ",   rdl2::RenderOutput::COMPRESSION_PIZ)
        .value("COMPRESSION_PXR24", rdl2::RenderOutput::COMPRESSION_PXR24)
        .value("COMPRESSION_B44",   rdl2::RenderOutput::COMPRESSION_B44)
        .value("COMPRESSION_B44A",  rdl2::RenderOutput::COMPRESSION_B44A)
        .value("COMPRESSION_DWAA",  rdl2::RenderOutput::COMPRESSION_DWAA)
        .value("COMPRESSION_DWAB",  rdl2::RenderOutput::COMPRESSION_DWAB)
        .export_values();

    py::enum_<rdl2::RenderOutput::Result>(m, "Result")
        .value("RESULT_BEAUTY",            rdl2::RenderOutput::RESULT_BEAUTY)
        .value("RESULT_ALPHA",             rdl2::RenderOutput::RESULT_ALPHA)
        .value("RESULT_DEPTH",             rdl2::RenderOutput::RESULT_DEPTH)
        .value("RESULT_STATE_VARIABLE",    rdl2::RenderOutput::RESULT_STATE_VARIABLE)
        .value("RESULT_PRIMITIVE_ATTRIBUTE",rdl2::RenderOutput::RESULT_PRIMITIVE_ATTRIBUTE)
        .value("RESULT_HEAT_MAP",          rdl2::RenderOutput::RESULT_HEAT_MAP)
        .value("RESULT_WIREFRAME",         rdl2::RenderOutput::RESULT_WIREFRAME)
        .value("RESULT_MATERIAL_AOV",      rdl2::RenderOutput::RESULT_MATERIAL_AOV)
        .value("RESULT_LIGHT_AOV",         rdl2::RenderOutput::RESULT_LIGHT_AOV)
        .value("RESULT_VISIBILITY_AOV",    rdl2::RenderOutput::RESULT_VISIBILITY_AOV)
        .value("RESULT_WEIGHT",            rdl2::RenderOutput::RESULT_WEIGHT)
        .value("RESULT_BEAUTY_AUX",        rdl2::RenderOutput::RESULT_BEAUTY_AUX)
        .value("RESULT_CRYPTOMATTE",       rdl2::RenderOutput::RESULT_CRYPTOMATTE)
        .value("RESULT_ALPHA_AUX",         rdl2::RenderOutput::RESULT_ALPHA_AUX)
        .value("RESULT_DISPLAY_FILTER",    rdl2::RenderOutput::RESULT_DISPLAY_FILTER)
        .export_values();

    py::enum_<rdl2::RenderOutput::StateVariable>(m, "StateVariable")
        .value("STATE_VARIABLE_P",      rdl2::RenderOutput::STATE_VARIABLE_P)
        .value("STATE_VARIABLE_NG",     rdl2::RenderOutput::STATE_VARIABLE_NG)
        .value("STATE_VARIABLE_N",      rdl2::RenderOutput::STATE_VARIABLE_N)
        .value("STATE_VARIABLE_ST",     rdl2::RenderOutput::STATE_VARIABLE_ST)
        .value("STATE_VARIABLE_DPDS",   rdl2::RenderOutput::STATE_VARIABLE_DPDS)
        .value("STATE_VARIABLE_DPDT",   rdl2::RenderOutput::STATE_VARIABLE_DPDT)
        .value("STATE_VARIABLE_DSDX",   rdl2::RenderOutput::STATE_VARIABLE_DSDX)
        .value("STATE_VARIABLE_DSDY",   rdl2::RenderOutput::STATE_VARIABLE_DSDY)
        .value("STATE_VARIABLE_DTDX",   rdl2::RenderOutput::STATE_VARIABLE_DTDX)
        .value("STATE_VARIABLE_DTDY",   rdl2::RenderOutput::STATE_VARIABLE_DTDY)
        .value("STATE_VARIABLE_WP",     rdl2::RenderOutput::STATE_VARIABLE_WP)
        .value("STATE_VARIABLE_DEPTH",  rdl2::RenderOutput::STATE_VARIABLE_DEPTH)
        .value("STATE_VARIABLE_MOTION", rdl2::RenderOutput::STATE_VARIABLE_MOTION)
        .export_values();

    py::enum_<rdl2::RenderOutput::PrimitiveAttributeType>(m, "PrimitiveAttributeType")
        .value("PRIMITIVE_ATTRIBUTE_TYPE_FLOAT", rdl2::RenderOutput::PRIMITIVE_ATTRIBUTE_TYPE_FLOAT)
        .value("PRIMITIVE_ATTRIBUTE_TYPE_VEC2F", rdl2::RenderOutput::PRIMITIVE_ATTRIBUTE_TYPE_VEC2F)
        .value("PRIMITIVE_ATTRIBUTE_TYPE_VEC3F", rdl2::RenderOutput::PRIMITIVE_ATTRIBUTE_TYPE_VEC3F)
        .value("PRIMITIVE_ATTRIBUTE_TYPE_RGB",   rdl2::RenderOutput::PRIMITIVE_ATTRIBUTE_TYPE_RGB)
        .export_values();

    py::enum_<rdl2::RenderOutput::MathFilter>(m, "MathFilter")
        .value("MATH_FILTER_AVG",                       rdl2::RenderOutput::MATH_FILTER_AVG)
        .value("MATH_FILTER_SUM",                       rdl2::RenderOutput::MATH_FILTER_SUM)
        .value("MATH_FILTER_MIN",                       rdl2::RenderOutput::MATH_FILTER_MIN)
        .value("MATH_FILTER_MAX",                       rdl2::RenderOutput::MATH_FILTER_MAX)
        .value("MATH_FILTER_FORCE_CONSISTENT_SAMPLING", rdl2::RenderOutput::MATH_FILTER_FORCE_CONSISTENT_SAMPLING)
        .value("MATH_FILTER_CLOSEST",                   rdl2::RenderOutput::MATH_FILTER_CLOSEST)
        .export_values();

    py::enum_<rdl2::RenderOutput::SuffixMode>(m, "SuffixMode")
        .value("SUFFIX_MODE_AUTO",      rdl2::RenderOutput::SUFFIX_MODE_AUTO)
        .value("SUFFIX_MODE_RGB",       rdl2::RenderOutput::SUFFIX_MODE_RGB)
        .value("SUFFIX_MODE_XYZ",       rdl2::RenderOutput::SUFFIX_MODE_XYZ)
        .value("SUFFIX_MODE_UVW",       rdl2::RenderOutput::SUFFIX_MODE_UVW)
        .value("SUFFIX_MODE_NUM_MODES", rdl2::RenderOutput::SUFFIX_MODE_NUM_MODES)
        .export_values();

    py::enum_<rdl2::RenderOutput::DenoiserInput>(m, "DenoiserInput")
        .value("DENOISER_INPUT_NONE",   rdl2::RenderOutput::DENOISER_INPUT_NONE)
        .value("DENOISER_INPUT_ALBEDO", rdl2::RenderOutput::DENOISER_INPUT_ALBEDO)
        .value("DENOISER_INPUT_NORMAL", rdl2::RenderOutput::DENOISER_INPUT_NORMAL)
        .export_values();
}
