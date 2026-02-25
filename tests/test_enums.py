#!/usr/bin/env python3
# Copyright (c) 2026 Alan Blevins
# SPDX-License-Identifier: MIT
"""Tests for enums and the attributeTypeName free function."""

import unittest

from .helpers import rdl2


class TestAttributeTypeEnum(unittest.TestCase):
    def test_scalar_types_exist(self):
        for name in ["TYPE_BOOL", "TYPE_INT", "TYPE_LONG", "TYPE_FLOAT",
                     "TYPE_DOUBLE", "TYPE_STRING", "TYPE_RGB", "TYPE_RGBA",
                     "TYPE_VEC2F", "TYPE_VEC2D", "TYPE_VEC3F", "TYPE_VEC3D",
                     "TYPE_VEC4F", "TYPE_VEC4D", "TYPE_MAT4F", "TYPE_MAT4D",
                     "TYPE_SCENE_OBJECT"]:
            with self.subTest(name=name):
                self.assertTrue(hasattr(rdl2.AttributeType, name))

    def test_vector_types_exist(self):
        for name in ["TYPE_BOOL_VECTOR", "TYPE_INT_VECTOR", "TYPE_FLOAT_VECTOR",
                     "TYPE_STRING_VECTOR", "TYPE_RGB_VECTOR",
                     "TYPE_VEC3F_VECTOR", "TYPE_MAT4F_VECTOR",
                     "TYPE_SCENE_OBJECT_VECTOR", "TYPE_SCENE_OBJECT_INDEXABLE"]:
            with self.subTest(name=name):
                self.assertTrue(hasattr(rdl2.AttributeType, name))

    def test_all_scalar_types_are_distinct(self):
        scalars = [rdl2.AttributeType.TYPE_BOOL, rdl2.AttributeType.TYPE_INT,
                   rdl2.AttributeType.TYPE_FLOAT, rdl2.AttributeType.TYPE_DOUBLE,
                   rdl2.AttributeType.TYPE_STRING, rdl2.AttributeType.TYPE_RGB]
        self.assertEqual(len(scalars), len(set(scalars)))


class TestAttributeFlagsEnum(unittest.TestCase):
    def test_values_exist(self):
        for name in ["FLAGS_NONE", "FLAGS_BINDABLE", "FLAGS_BLURRABLE",
                     "FLAGS_ENUMERABLE", "FLAGS_FILENAME", "FLAGS_CAN_SKIP_GEOM_RELOAD"]:
            with self.subTest(name=name):
                self.assertTrue(hasattr(rdl2, name))

    def test_flags_none_is_zero(self):
        self.assertEqual(int(rdl2.FLAGS_NONE), 0)

    def test_arithmetic(self):
        combined = rdl2.FLAGS_BINDABLE | rdl2.FLAGS_BLURRABLE
        self.assertNotEqual(int(combined), 0)
        self.assertNotEqual(int(combined), int(rdl2.FLAGS_BINDABLE))


class TestAttributeTimestepEnum(unittest.TestCase):
    def test_values_exist(self):
        self.assertIsNotNone(rdl2.TIMESTEP_BEGIN)
        self.assertIsNotNone(rdl2.TIMESTEP_END)
        self.assertIsNotNone(rdl2.NUM_TIMESTEPS)

    def test_begin_before_end(self):
        self.assertLess(int(rdl2.TIMESTEP_BEGIN), int(rdl2.TIMESTEP_END))


class TestSceneObjectInterfaceEnum(unittest.TestCase):
    def test_values_exist(self):
        for name in ["INTERFACE_GENERIC", "INTERFACE_GEOMETRYSET", "INTERFACE_LAYER",
                     "INTERFACE_LIGHTSET", "INTERFACE_NODE", "INTERFACE_CAMERA",
                     "INTERFACE_ENVMAP", "INTERFACE_GEOMETRY", "INTERFACE_LIGHT",
                     "INTERFACE_SHADER", "INTERFACE_MATERIAL", "INTERFACE_RENDEROUTPUT",
                     "INTERFACE_USERDATA", "INTERFACE_METADATA", "INTERFACE_TRACESET",
                     "INTERFACE_JOINT", "INTERFACE_LIGHTFILTERSET",
                     "INTERFACE_SHADOWSET", "INTERFACE_SHADOWRECEIVERSET",
                     "INTERFACE_DISPLAYFILTER"]:
            with self.subTest(name=name):
                self.assertTrue(hasattr(rdl2, name), f"Missing {name}")

    def test_generic_is_one(self):
        # INTERFACE_GENERIC = 1 << 0 = 1 in the C++ enum
        self.assertEqual(int(rdl2.INTERFACE_GENERIC), 1)

    def test_arithmetic(self):
        # Interfaces are bitmasks — combining two distinct ones should be nonzero
        combined = rdl2.INTERFACE_NODE | rdl2.INTERFACE_CAMERA
        self.assertNotEqual(int(combined), 0)


class TestMiscEnums(unittest.TestCase):
    def test_motion_blur_type(self):
        for name in ["STATIC", "VELOCITY", "FRAME_DELTA", "ACCELERATION",
                     "HERMITE", "STATIC_DUPLICATE", "BEST"]:
            with self.subTest(name=name):
                self.assertTrue(hasattr(rdl2.MotionBlurType, name))

    def test_primitive_attribute_frame(self):
        self.assertTrue(hasattr(rdl2.PrimitiveAttributeFrame, "FIRST_MOTION_STEP"))
        self.assertTrue(hasattr(rdl2.PrimitiveAttributeFrame, "BOTH_MOTION_STEPS"))

    def test_pixel_filter_type(self):
        self.assertTrue(hasattr(rdl2.PixelFilterType, "box"))
        self.assertTrue(hasattr(rdl2.PixelFilterType, "cubicBSpline"))
        self.assertTrue(hasattr(rdl2.PixelFilterType, "quadraticBSpline"))

    def test_task_distribution_type(self):
        self.assertTrue(hasattr(rdl2.TaskDistributionType, "NON_OVERLAPPED_TILE"))
        self.assertTrue(hasattr(rdl2.TaskDistributionType, "MULTIPLEX_PIXEL"))

    def test_volume_overlap_mode(self):
        for name in ["SUM", "MAX", "RND"]:
            self.assertTrue(hasattr(rdl2.VolumeOverlapMode, name))

    def test_shadow_terminator_fix(self):
        for name in ["OFF", "CUSTOM", "SINE_COMPENSATION", "GGX", "COSINE_COMPENSATION"]:
            self.assertTrue(hasattr(rdl2.ShadowTerminatorFix, name))

    def test_texture_filter_type(self):
        self.assertTrue(hasattr(rdl2.TextureFilterType, "TEXTURE_FILTER_NEAREST"))
        self.assertTrue(hasattr(rdl2.TextureFilterType, "TEXTURE_FILTER_BILINEAR"))
        self.assertTrue(hasattr(rdl2.TextureFilterType, "TEXTURE_FILTER_NEAREST_MIP_NEAREST"))
        self.assertTrue(hasattr(rdl2.TextureFilterType, "TEXTURE_FILTER_BILINEAR_MIP_NEAREST"))

    def test_geometry_side_type(self):
        self.assertTrue(hasattr(rdl2.GeometrySideType, "TWO_SIDED"))
        self.assertTrue(hasattr(rdl2.GeometrySideType, "SINGLE_SIDED"))
        self.assertTrue(hasattr(rdl2.GeometrySideType, "MESH_DEFAULT_SIDED"))


class TestRenderOutputEnums(unittest.TestCase):
    def test_channel_format(self):
        self.assertTrue(hasattr(rdl2.ChannelFormat, "CHANNEL_FORMAT_FLOAT"))
        self.assertTrue(hasattr(rdl2.ChannelFormat, "CHANNEL_FORMAT_HALF"))

    def test_compression(self):
        for name in ["COMPRESSION_NONE", "COMPRESSION_ZIP", "COMPRESSION_RLE",
                     "COMPRESSION_ZIPS", "COMPRESSION_PIZ", "COMPRESSION_PXR24",
                     "COMPRESSION_B44", "COMPRESSION_B44A",
                     "COMPRESSION_DWAA", "COMPRESSION_DWAB"]:
            with self.subTest(name=name):
                self.assertTrue(hasattr(rdl2.Compression, name))

    def test_result(self):
        for name in ["RESULT_BEAUTY", "RESULT_ALPHA", "RESULT_DEPTH",
                     "RESULT_STATE_VARIABLE", "RESULT_PRIMITIVE_ATTRIBUTE",
                     "RESULT_HEAT_MAP", "RESULT_MATERIAL_AOV", "RESULT_LIGHT_AOV",
                     "RESULT_CRYPTOMATTE", "RESULT_DISPLAY_FILTER"]:
            with self.subTest(name=name):
                self.assertTrue(hasattr(rdl2.Result, name))

    def test_state_variable(self):
        for name in ["STATE_VARIABLE_P", "STATE_VARIABLE_N", "STATE_VARIABLE_ST",
                     "STATE_VARIABLE_WP", "STATE_VARIABLE_DEPTH", "STATE_VARIABLE_MOTION"]:
            with self.subTest(name=name):
                self.assertTrue(hasattr(rdl2.StateVariable, name))

    def test_math_filter(self):
        for name in ["MATH_FILTER_AVG", "MATH_FILTER_SUM", "MATH_FILTER_MIN",
                     "MATH_FILTER_MAX", "MATH_FILTER_CLOSEST"]:
            with self.subTest(name=name):
                self.assertTrue(hasattr(rdl2.MathFilter, name))

    def test_suffix_mode(self):
        for name in ["SUFFIX_MODE_AUTO", "SUFFIX_MODE_RGB",
                     "SUFFIX_MODE_XYZ", "SUFFIX_MODE_UVW"]:
            with self.subTest(name=name):
                self.assertTrue(hasattr(rdl2.SuffixMode, name))

    def test_denoiser_input(self):
        for name in ["DENOISER_INPUT_NONE", "DENOISER_INPUT_ALBEDO",
                     "DENOISER_INPUT_NORMAL"]:
            with self.subTest(name=name):
                self.assertTrue(hasattr(rdl2.DenoiserInput, name))


class TestAttributeTypeName(unittest.TestCase):
    def test_returns_string(self):
        name = rdl2.attributeTypeName(rdl2.AttributeType.TYPE_BOOL)
        self.assertIsInstance(name, str)
        self.assertGreater(len(name), 0)

    def test_known_values(self):
        cases = {
            rdl2.AttributeType.TYPE_BOOL:   "Bool",
            rdl2.AttributeType.TYPE_INT:    "Int",
            rdl2.AttributeType.TYPE_LONG:   "Long",
            rdl2.AttributeType.TYPE_FLOAT:  "Float",
            rdl2.AttributeType.TYPE_STRING: "String",
            rdl2.AttributeType.TYPE_RGB:    "Rgb",
            rdl2.AttributeType.TYPE_VEC3F:  "Vec3f",
            rdl2.AttributeType.TYPE_MAT4F:  "Mat4f",
        }
        for attr_type, expected in cases.items():
            with self.subTest(type=expected):
                self.assertEqual(rdl2.attributeTypeName(attr_type), expected)

    def test_different_types_give_different_names(self):
        names = [rdl2.attributeTypeName(t) for t in [
            rdl2.AttributeType.TYPE_BOOL, rdl2.AttributeType.TYPE_INT,
            rdl2.AttributeType.TYPE_FLOAT, rdl2.AttributeType.TYPE_STRING,
            rdl2.AttributeType.TYPE_RGB, rdl2.AttributeType.TYPE_VEC3F,
        ]]
        self.assertEqual(len(names), len(set(names)))


if __name__ == "__main__":
    unittest.main()
