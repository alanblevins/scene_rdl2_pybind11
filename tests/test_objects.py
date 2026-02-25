#!/usr/bin/env python3
# Copyright (c) 2026 Alan Blevins
# SPDX-License-Identifier: MIT
"""Tests for scene object types: DisplayFilter, EnvMap, Joint, Metadata, TraceSet."""

import unittest

from .helpers import rdl2, _WithDsos, _first_class_name


class TestDisplayFilter(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        name = _first_class_name(cls.ctx, rdl2.INTERFACE_DISPLAYFILTER)
        cls.assertIsNotNone(cls, name, "No DisplayFilter DSO found")
        cls.obj = cls.ctx.createSceneObject(name, "/test/df/df")

    def test_is_display_filter(self):
        self.assertTrue(self.obj.isDisplayFilter())

    def test_as_display_filter_returns_instance(self):
        df = self.obj.asDisplayFilter()
        self.assertIsNotNone(df)
        self.assertIsInstance(df, rdl2.DisplayFilter)

    def test_is_scene_object(self):
        self.assertIsInstance(self.obj, rdl2.SceneObject)

    def test_as_display_filter_wrong_type_returns_none(self):
        box = self.ctx.createSceneObject("BoxGeometry", "/test/df/box")
        self.assertIsNone(box.asDisplayFilter())


class TestEnvMap(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        name = _first_class_name(cls.ctx, rdl2.INTERFACE_ENVMAP)
        if name is None:
            raise unittest.SkipTest("No EnvMap DSO available in DSO_PATH")
        cls.obj = cls.ctx.createSceneObject(name, "/test/envmap/em")

    def test_is_env_map(self):
        self.assertTrue(self.obj.isEnvMap())

    def test_as_env_map_returns_instance(self):
        em = self.obj.asEnvMap()
        self.assertIsNotNone(em)
        self.assertIsInstance(em, rdl2.EnvMap)

    def test_env_map_is_node(self):
        # EnvMap inherits Node — asEnvMap() result should be an instance of Node
        em = self.obj.asEnvMap()
        self.assertIsInstance(em, rdl2.Node)

    def test_as_env_map_wrong_type_returns_none(self):
        box = self.ctx.createSceneObject("BoxGeometry", "/test/envmap/box")
        self.assertIsNone(box.asEnvMap())


class TestJoint(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        name = _first_class_name(cls.ctx, rdl2.INTERFACE_JOINT)
        cls.assertIsNotNone(cls, name, "No Joint DSO found")
        cls.obj = cls.ctx.createSceneObject(name, "/test/joint/j")

    def test_is_joint(self):
        self.assertTrue(self.obj.isJoint())

    def test_as_joint_returns_instance(self):
        j = self.obj.asJoint()
        self.assertIsNotNone(j)
        self.assertIsInstance(j, rdl2.Joint)

    def test_joint_is_node(self):
        j = self.obj.asJoint()
        self.assertIsInstance(j, rdl2.Node)

    def test_as_joint_wrong_type_returns_none(self):
        box = self.ctx.createSceneObject("BoxGeometry", "/test/joint/box")
        self.assertIsNone(box.asJoint())


class TestMetadata(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.md = cls.ctx.createSceneObject("Metadata", "/test/metadata/md")

    def test_is_metadata(self):
        self.assertTrue(self.md.isMetadata())

    def test_as_metadata_returns_instance(self):
        md = self.md.asMetadata()
        self.assertIsNotNone(md)
        self.assertIsInstance(md, rdl2.Metadata)

    def test_empty_by_default(self):
        md = self.md.asMetadata()
        self.assertEqual(md.getAttributeNames(), [])
        self.assertEqual(md.getAttributeTypes(), [])
        self.assertEqual(md.getAttributeValues(), [])

    def test_set_and_get_attributes(self):
        md = self.md.asMetadata()
        md.setAttributes(
            ["author", "frame"],
            ["string", "int"],
            ["Alice",  "42"])
        self.assertEqual(md.getAttributeNames(),  ["author", "frame"])
        self.assertEqual(md.getAttributeTypes(),  ["string", "int"])
        self.assertEqual(md.getAttributeValues(), ["Alice",  "42"])

    def test_set_attributes_overwrites(self):
        md = self.ctx.createSceneObject("Metadata", "/test/metadata/md2").asMetadata()
        md.setAttributes(["k"], ["string"], ["v1"])
        md.setAttributes(["k2"], ["float"], ["3.14"])
        # Second call replaces everything
        self.assertEqual(md.getAttributeNames(), ["k2"])

    def test_as_metadata_wrong_type_returns_none(self):
        box = self.ctx.createSceneObject("BoxGeometry", "/test/metadata/box")
        self.assertIsNone(box.asMetadata())


class TestTraceSet(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        geo_name = _first_class_name(cls.ctx, rdl2.INTERFACE_GEOMETRY)
        cls.geo1 = cls.ctx.createSceneObject(geo_name, "/test/traceset/geo1").asGeometry()
        cls.geo2 = cls.ctx.createSceneObject(geo_name, "/test/traceset/geo2").asGeometry()
        cls.ts   = cls.ctx.createSceneObject("TraceSet", "/test/traceset/ts")

    def test_is_trace_set(self):
        self.assertTrue(self.ts.isTraceSet())

    def test_as_trace_set_returns_instance(self):
        ts = self.ts.asTraceSet()
        self.assertIsNotNone(ts)
        self.assertIsInstance(ts, rdl2.TraceSet)

    def test_empty_initially(self):
        ts = self.ctx.createSceneObject("TraceSet", "/test/traceset/empty").asTraceSet()
        self.assertEqual(ts.getAssignmentCount(), 0)

    def test_assign_returns_id(self):
        ts = self.ctx.createSceneObject("TraceSet", "/test/traceset/ts_a").asTraceSet()
        aid = ts.assign(self.geo1, "partA")
        self.assertIsInstance(aid, int)
        self.assertGreaterEqual(aid, 0)

    def test_assignment_count_increments(self):
        ts = self.ctx.createSceneObject("TraceSet", "/test/traceset/ts_b").asTraceSet()
        ts.assign(self.geo1, "part1")
        ts.assign(self.geo1, "part2")
        ts.assign(self.geo2, "part1")
        self.assertEqual(ts.getAssignmentCount(), 3)

    def test_assign_same_pair_returns_same_id(self):
        ts = self.ctx.createSceneObject("TraceSet", "/test/traceset/ts_c").asTraceSet()
        aid1 = ts.assign(self.geo1, "part")
        aid2 = ts.assign(self.geo1, "part")
        self.assertEqual(aid1, aid2)
        self.assertEqual(ts.getAssignmentCount(), 1)

    def test_contains(self):
        ts = self.ctx.createSceneObject("TraceSet", "/test/traceset/ts_d").asTraceSet()
        ts.assign(self.geo1, "part")
        self.assertTrue(ts.contains(self.geo1))
        self.assertFalse(ts.contains(self.geo2))

    def test_lookup_geom_and_part(self):
        ts = self.ctx.createSceneObject("TraceSet", "/test/traceset/ts_e").asTraceSet()
        aid = ts.assign(self.geo1, "myPart")
        geom, part = ts.lookupGeomAndPart(aid)
        self.assertIsNotNone(geom)
        self.assertEqual(part, "myPart")

    def test_get_assignment_id(self):
        ts = self.ctx.createSceneObject("TraceSet", "/test/traceset/ts_f").asTraceSet()
        aid = ts.assign(self.geo1, "part")
        self.assertEqual(ts.getAssignmentId(self.geo1, "part"), aid)
        self.assertEqual(ts.getAssignmentId(self.geo2, "part"), -1)

    def test_get_assignment_ids(self):
        ts = self.ctx.createSceneObject("TraceSet", "/test/traceset/ts_g").asTraceSet()
        aid1 = ts.assign(self.geo1, "p1")
        aid2 = ts.assign(self.geo1, "p2")
        ids = ts.getAssignmentIds(self.geo1)
        self.assertIsInstance(ids, list)
        self.assertIn(aid1, ids)
        self.assertIn(aid2, ids)

    def test_as_trace_set_wrong_type_returns_none(self):
        box = self.ctx.createSceneObject("BoxGeometry", "/test/traceset/box")
        self.assertIsNone(box.asTraceSet())


if __name__ == "__main__":
    unittest.main()
