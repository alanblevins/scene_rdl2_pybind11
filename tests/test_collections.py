#!/usr/bin/env python3
# Copyright (c) 2026 Alan Blevins
# SPDX-License-Identifier: MIT
"""Tests for collection types: GeometrySet, LightSet, Layer, and RenderOutput."""

import unittest

from .helpers import rdl2, _WithDsos, _first_class_name


class TestGeometrySet(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        geo_name = _first_class_name(cls.ctx, rdl2.INTERFACE_GEOMETRY)
        cls.geo1 = cls.ctx.createSceneObject(geo_name, "/test/geoset/geo1").asGeometry()
        cls.geo2 = cls.ctx.createSceneObject(geo_name, "/test/geoset/geo2").asGeometry()
        cls.gset = cls.ctx.createSceneObject("GeometrySet", "/test/geoset/set")

    def test_is_geometry_set(self):
        self.assertIsInstance(self.gset, rdl2.GeometrySet)
        self.assertTrue(self.gset.isGeometrySet())

    def test_empty_initially(self):
        gset = self.ctx.createSceneObject("GeometrySet", "/test/geoset/empty")
        self.assertEqual(len(gset.getGeometries()), 0)

    def test_add_and_contains(self):
        gset = self.ctx.createSceneObject("GeometrySet", "/test/geoset/add")
        gset.add(self.geo1)
        self.assertTrue(gset.contains(self.geo1))

    def test_remove(self):
        gset = self.ctx.createSceneObject("GeometrySet", "/test/geoset/rem")
        gset.add(self.geo1)
        gset.remove(self.geo1)
        self.assertFalse(gset.contains(self.geo1))

    def test_clear(self):
        gset = self.ctx.createSceneObject("GeometrySet", "/test/geoset/clr")
        gset.add(self.geo1)
        gset.add(self.geo2)
        gset.clear()
        self.assertEqual(len(gset.getGeometries()), 0)

    def test_get_geometries_returns_list(self):
        gset = self.ctx.createSceneObject("GeometrySet", "/test/geoset/lst")
        gset.add(self.geo1)
        gset.add(self.geo2)
        geoms = gset.getGeometries()
        self.assertIsInstance(geoms, list)
        self.assertEqual(len(geoms), 2)

    def test_is_static(self):
        self.assertIsInstance(self.gset.isStatic(), bool)


class TestLightSet(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        light_name = _first_class_name(cls.ctx, rdl2.INTERFACE_LIGHT)
        cls.assertIsNotNone(cls, light_name, "No Light DSO found")
        cls.light1 = cls.ctx.createSceneObject(light_name, "/test/lightset/light1").asLight()
        cls.light2 = cls.ctx.createSceneObject(light_name, "/test/lightset/light2").asLight()

    def test_is_light_set(self):
        lset = self.ctx.createSceneObject("LightSet", "/test/lightset/set")
        self.assertIsInstance(lset, rdl2.LightSet)
        self.assertTrue(lset.isLightSet())

    def test_empty_initially(self):
        lset = self.ctx.createSceneObject("LightSet", "/test/lightset/empty")
        self.assertEqual(len(lset.getLights()), 0)

    def test_add_and_contains(self):
        lset = self.ctx.createSceneObject("LightSet", "/test/lightset/add")
        lset.add(self.light1)
        self.assertTrue(lset.contains(self.light1))

    def test_remove(self):
        lset = self.ctx.createSceneObject("LightSet", "/test/lightset/rem")
        lset.add(self.light1)
        lset.remove(self.light1)
        self.assertFalse(lset.contains(self.light1))

    def test_clear(self):
        lset = self.ctx.createSceneObject("LightSet", "/test/lightset/clr")
        lset.add(self.light1)
        lset.add(self.light2)
        lset.clear()
        self.assertEqual(len(lset.getLights()), 0)


class TestLayerAssignment(unittest.TestCase):
    def test_default_construction(self):
        la = rdl2.LayerAssignment()
        self.assertIsInstance(la, rdl2.LayerAssignment)

    def test_default_fields_none(self):
        la = rdl2.LayerAssignment()
        self.assertIsNone(la.material)
        self.assertIsNone(la.lightSet)
        self.assertIsNone(la.displacement)
        self.assertIsNone(la.volumeShader)
        self.assertIsNone(la.lightFilterSet)
        self.assertIsNone(la.shadowSet)
        self.assertIsNone(la.shadowReceiverSet)


class TestLayer(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        geo_name = _first_class_name(cls.ctx, rdl2.INTERFACE_GEOMETRY)
        mat_name = _first_class_name(cls.ctx, rdl2.INTERFACE_MATERIAL)
        cls.assertIsNotNone(cls, geo_name, "No Geometry DSO found")
        cls.assertIsNotNone(cls, mat_name, "No Material DSO found")
        cls.geo   = cls.ctx.createSceneObject(geo_name,  "/test/layer/geo").asGeometry()
        cls.mat   = cls.ctx.createSceneObject(mat_name,  "/test/layer/mat").asMaterial()
        cls.lset  = cls.ctx.createSceneObject("LightSet", "/test/layer/lset").asLightSet()
        cls.layer = cls.ctx.createSceneObject("Layer",   "/test/layer/layer")

    def test_is_layer(self):
        self.assertIsInstance(self.layer, rdl2.Layer)
        self.assertTrue(self.layer.isLayer())

    def test_assign_4arg(self):
        aid = self.layer.assign(self.geo, "", self.mat, self.lset)
        self.assertIsInstance(aid, int)

    def test_lookup_after_assign(self):
        aid = self.layer.assign(self.geo, "", self.mat, self.lset)
        mat = self.layer.lookupMaterial(aid)
        lset = self.layer.lookupLightSet(aid)
        self.assertIsNotNone(mat)
        self.assertIsNotNone(lset)

    def test_clear(self):
        self.layer.clear()

    def test_light_sets_changed_is_bool(self):
        self.assertIsInstance(self.layer.lightSetsChanged(), bool)


class TestRenderOutput(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.ro = cls.ctx.createSceneObject("RenderOutput", "/test/ro/beauty")

    def test_is_render_output(self):
        self.assertIsInstance(self.ro, rdl2.RenderOutput)
        self.assertTrue(self.ro.isRenderOutput())

    def test_get_active_is_bool(self):
        self.assertIsInstance(self.ro.getActive(), bool)

    def test_get_result_is_result_enum(self):
        result = self.ro.getResult()
        self.assertIsInstance(result, rdl2.Result)

    def test_get_file_name_is_str(self):
        self.assertIsInstance(self.ro.getFileName(), str)

    def test_get_channel_name_is_str(self):
        self.assertIsInstance(self.ro.getChannelName(), str)

    def test_get_compression_is_compression_enum(self):
        c = self.ro.getCompression()
        self.assertIsInstance(c, rdl2.Compression)

    def test_get_channel_format_is_channel_format_enum(self):
        cf = self.ro.getChannelFormat()
        self.assertIsInstance(cf, rdl2.ChannelFormat)

    def test_get_math_filter_is_math_filter_enum(self):
        mf = self.ro.getMathFilter()
        self.assertIsInstance(mf, rdl2.MathFilter)

    def test_get_denoise_is_bool(self):
        self.assertIsInstance(self.ro.getDenoise(), bool)

    def test_get_cryptomatte_depth_is_int(self):
        self.assertIsInstance(self.ro.getCryptomatteDepth(), int)

    def test_get_camera_is_none_or_camera(self):
        cam = self.ro.getCamera()
        self.assertTrue(cam is None or isinstance(cam, rdl2.Camera))

    def test_get_lpe_is_str(self):
        self.assertIsInstance(self.ro.getLpe(), str)

    def test_get_checkpoint_file_name_is_str(self):
        self.assertIsInstance(self.ro.getCheckpointFileName(), str)


if __name__ == "__main__":
    unittest.main()
