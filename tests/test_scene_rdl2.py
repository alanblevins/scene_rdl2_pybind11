#!/usr/bin/env python3
# Copyright (c) 2026 Alan Blevins
# SPDX-License-Identifier: MIT
"""
Comprehensive unit tests for the scene_rdl2 Python bindings.

Run all tests:
    python3.13 -m unittest discover tests/ -v

Run a specific class:
    python3.13 -m unittest tests.test_scene_rdl2.TestMath -v
"""

import os
import sys
import math
import tempfile
import unittest

# ---------------------------------------------------------------------------
# Path bootstrap — locate the extension module in build/
# ---------------------------------------------------------------------------
_HERE = os.path.dirname(os.path.abspath(__file__))
_ROOT = os.path.dirname(_HERE)
_BUILD = os.path.join(_ROOT, "build")
if os.path.isdir(_BUILD):
    sys.path.insert(0, _BUILD)

import scene_rdl2 as rdl2

DSO_PATH = "/Applications/MoonRay/installs/openmoonray/rdl2dso"


# ---------------------------------------------------------------------------
# Shared helpers
# ---------------------------------------------------------------------------

def _make_ctx(load_dsos=False):
    """Create a proxy-mode SceneContext, optionally loading all DSOs."""
    ctx = rdl2.SceneContext()
    ctx.setProxyModeEnabled(True)
    ctx.setDsoPath(DSO_PATH)
    if load_dsos:
        ctx.loadAllSceneClasses()
    return ctx


def _first_class_name(ctx, iface):
    """Return the name of the first SceneClass that matches *iface*, or None."""
    for sc in ctx.getAllSceneClasses():
        if sc.getDeclaredInterface() & iface:
            return sc.getName()
    return None


def _first_attr(scene_class, attr_type):
    """Return the first Attribute of *attr_type* in *scene_class*, or None."""
    for attr in scene_class.getAttributes():
        if attr.getType() == attr_type:
            return attr
    return None


# ---------------------------------------------------------------------------
# Shared DSO-loaded fixture (expensive — built once for the whole process)
# ---------------------------------------------------------------------------

class _WithDsos(unittest.TestCase):
    """Base class: provides a shared proxy-mode context with all DSOs loaded."""
    _ctx = None

    @classmethod
    def setUpClass(cls):
        if _WithDsos._ctx is None:
            _WithDsos._ctx = _make_ctx(load_dsos=True)
        cls.ctx = _WithDsos._ctx


# ===========================================================================
# Math types
# ===========================================================================

class TestRgb(unittest.TestCase):
    def test_default_ctor(self):
        c = rdl2.Rgb()
        self.assertEqual(c.r, 0.0)
        self.assertEqual(c.g, 0.0)
        self.assertEqual(c.b, 0.0)

    def test_uniform_ctor(self):
        c = rdl2.Rgb(0.5)
        self.assertAlmostEqual(c.r, 0.5)
        self.assertAlmostEqual(c.g, 0.5)
        self.assertAlmostEqual(c.b, 0.5)

    def test_component_ctor(self):
        c = rdl2.Rgb(0.1, 0.2, 0.3)
        self.assertAlmostEqual(c.r, 0.1)
        self.assertAlmostEqual(c.g, 0.2)
        self.assertAlmostEqual(c.b, 0.3)

    def test_field_mutation(self):
        c = rdl2.Rgb()
        c.r = 1.0
        c.g = 0.5
        c.b = 0.25
        self.assertAlmostEqual(c.r, 1.0)
        self.assertAlmostEqual(c.g, 0.5)
        self.assertAlmostEqual(c.b, 0.25)

    def test_eq(self):
        self.assertEqual(rdl2.Rgb(1, 0, 0), rdl2.Rgb(1, 0, 0))
        self.assertNotEqual(rdl2.Rgb(1, 0, 0), rdl2.Rgb(0, 0, 0))

    def test_iter(self):
        vals = list(rdl2.Rgb(0.1, 0.2, 0.3))
        self.assertEqual(len(vals), 3)
        self.assertAlmostEqual(vals[0], 0.1)
        self.assertAlmostEqual(vals[1], 0.2)
        self.assertAlmostEqual(vals[2], 0.3)

    def test_repr(self):
        r = repr(rdl2.Rgb(1.0, 0.0, 0.0))
        self.assertTrue(r.startswith("Rgb("))
        self.assertIn("1", r)

    def test_from_list(self):
        c = rdl2.Rgb([0.2, 0.4, 0.6])
        self.assertAlmostEqual(c.r, 0.2)
        self.assertAlmostEqual(c.g, 0.4)
        self.assertAlmostEqual(c.b, 0.6)

    def test_from_tuple(self):
        c = rdl2.Rgb((0.1, 0.5, 0.9))
        self.assertAlmostEqual(c.r, 0.1)
        self.assertAlmostEqual(c.g, 0.5)
        self.assertAlmostEqual(c.b, 0.9)

    def test_wrong_length_raises(self):
        with self.assertRaises(ValueError):
            rdl2.Rgb([1.0, 2.0])
        with self.assertRaises(ValueError):
            rdl2.Rgb([1.0, 2.0, 3.0, 4.0])


class TestRgba(unittest.TestCase):
    def test_default_ctor(self):
        c = rdl2.Rgba()
        self.assertEqual(c.r, 0.0)
        self.assertEqual(c.a, 0.0)

    def test_uniform_ctor(self):
        c = rdl2.Rgba(0.5)
        self.assertAlmostEqual(c.r, 0.5)
        self.assertAlmostEqual(c.a, 0.5)

    def test_component_ctor(self):
        c = rdl2.Rgba(0.1, 0.2, 0.3, 1.0)
        self.assertAlmostEqual(c.r, 0.1)
        self.assertAlmostEqual(c.g, 0.2)
        self.assertAlmostEqual(c.b, 0.3)
        self.assertAlmostEqual(c.a, 1.0)

    def test_eq(self):
        self.assertEqual(rdl2.Rgba(1, 0, 0, 1), rdl2.Rgba(1, 0, 0, 1))
        self.assertNotEqual(rdl2.Rgba(1, 0, 0, 1), rdl2.Rgba(1, 0, 0, 0))

    def test_repr(self):
        self.assertTrue(repr(rdl2.Rgba(1, 0, 0, 1)).startswith("Rgba("))

    def test_from_list(self):
        c = rdl2.Rgba([0.2, 0.4, 0.6, 0.8])
        self.assertAlmostEqual(c.r, 0.2)
        self.assertAlmostEqual(c.g, 0.4)
        self.assertAlmostEqual(c.b, 0.6)
        self.assertAlmostEqual(c.a, 0.8)

    def test_from_tuple(self):
        c = rdl2.Rgba((0.1, 0.2, 0.3, 1.0))
        self.assertAlmostEqual(c.r, 0.1)
        self.assertAlmostEqual(c.a, 1.0)

    def test_wrong_length_raises(self):
        with self.assertRaises(ValueError):
            rdl2.Rgba([1.0, 2.0, 3.0])
        with self.assertRaises(ValueError):
            rdl2.Rgba([1.0, 2.0, 3.0, 4.0, 5.0])


class TestVec2f(unittest.TestCase):
    def test_ctors(self):
        self.assertEqual(rdl2.Vec2f().x, 0.0)
        v = rdl2.Vec2f(3.0)
        self.assertAlmostEqual(v.x, 3.0)
        self.assertAlmostEqual(v.y, 3.0)
        v = rdl2.Vec2f(1.0, 2.0)
        self.assertAlmostEqual(v.x, 1.0)
        self.assertAlmostEqual(v.y, 2.0)

    def test_mutation(self):
        v = rdl2.Vec2f()
        v.x = 4.0
        v.y = 5.0
        self.assertAlmostEqual(v.x, 4.0)
        self.assertAlmostEqual(v.y, 5.0)

    def test_eq(self):
        self.assertEqual(rdl2.Vec2f(1.0, 2.0), rdl2.Vec2f(1.0, 2.0))
        self.assertNotEqual(rdl2.Vec2f(1.0, 2.0), rdl2.Vec2f(1.0, 3.0))

    def test_repr(self):
        self.assertTrue(repr(rdl2.Vec2f(1.0, 2.0)).startswith("Vec2f("))

    def test_from_list(self):
        v = rdl2.Vec2f([3.0, 4.0])
        self.assertAlmostEqual(v.x, 3.0)
        self.assertAlmostEqual(v.y, 4.0)

    def test_from_tuple(self):
        v = rdl2.Vec2f((5.0, 6.0))
        self.assertAlmostEqual(v.x, 5.0)
        self.assertAlmostEqual(v.y, 6.0)

    def test_wrong_length_raises(self):
        with self.assertRaises(ValueError):
            rdl2.Vec2f([1.0])
        with self.assertRaises(ValueError):
            rdl2.Vec2f([1.0, 2.0, 3.0])


class TestVec2d(unittest.TestCase):
    def test_ctors(self):
        self.assertEqual(rdl2.Vec2d().x, 0.0)
        v = rdl2.Vec2d(5.0)
        self.assertAlmostEqual(v.x, 5.0)
        self.assertAlmostEqual(v.y, 5.0)
        v = rdl2.Vec2d(3.0, 4.0)
        self.assertAlmostEqual(v.x, 3.0)
        self.assertAlmostEqual(v.y, 4.0)

    def test_repr(self):
        self.assertTrue(repr(rdl2.Vec2d(1.0, 2.0)).startswith("Vec2d("))

    def test_from_list(self):
        v = rdl2.Vec2d([1.5, 2.5])
        self.assertAlmostEqual(v.x, 1.5)
        self.assertAlmostEqual(v.y, 2.5)

    def test_from_tuple(self):
        v = rdl2.Vec2d((3.5, 4.5))
        self.assertAlmostEqual(v.x, 3.5)
        self.assertAlmostEqual(v.y, 4.5)

    def test_wrong_length_raises(self):
        with self.assertRaises(ValueError):
            rdl2.Vec2d([1.0, 2.0, 3.0])
        with self.assertRaises(ValueError):
            rdl2.Vec2d([1.0])


class TestVec3f(unittest.TestCase):
    def test_ctors(self):
        self.assertEqual(rdl2.Vec3f().z, 0.0)
        v = rdl2.Vec3f(7.0)
        self.assertAlmostEqual(v.x, 7.0)
        self.assertAlmostEqual(v.y, 7.0)
        self.assertAlmostEqual(v.z, 7.0)
        v = rdl2.Vec3f(1.0, 2.0, 3.0)
        self.assertAlmostEqual(v.z, 3.0)

    def test_mutation(self):
        v = rdl2.Vec3f()
        v.x = 1.0
        v.y = 2.0
        v.z = 3.0
        self.assertAlmostEqual(v.z, 3.0)

    def test_eq(self):
        self.assertEqual(rdl2.Vec3f(1, 2, 3), rdl2.Vec3f(1, 2, 3))
        self.assertNotEqual(rdl2.Vec3f(1, 2, 3), rdl2.Vec3f(1, 2, 4))

    def test_iter(self):
        vals = list(rdl2.Vec3f(1.0, 2.0, 3.0))
        self.assertEqual(len(vals), 3)
        self.assertAlmostEqual(vals[0], 1.0)
        self.assertAlmostEqual(vals[2], 3.0)

    def test_repr(self):
        self.assertTrue(repr(rdl2.Vec3f(1, 2, 3)).startswith("Vec3f("))

    def test_from_list(self):
        v = rdl2.Vec3f([1.0, 2.0, 3.0])
        self.assertAlmostEqual(v.x, 1.0)
        self.assertAlmostEqual(v.y, 2.0)
        self.assertAlmostEqual(v.z, 3.0)

    def test_from_tuple(self):
        v = rdl2.Vec3f((4.0, 5.0, 6.0))
        self.assertAlmostEqual(v.x, 4.0)
        self.assertAlmostEqual(v.z, 6.0)

    def test_wrong_length_raises(self):
        with self.assertRaises(ValueError):
            rdl2.Vec3f([1.0, 2.0])
        with self.assertRaises(ValueError):
            rdl2.Vec3f([1.0, 2.0, 3.0, 4.0])


class TestVec3d(unittest.TestCase):
    def test_ctors(self):
        v = rdl2.Vec3d(1.0, 2.0, 3.0)
        self.assertAlmostEqual(v.x, 1.0)
        self.assertAlmostEqual(v.y, 2.0)
        self.assertAlmostEqual(v.z, 3.0)

    def test_uniform_ctor(self):
        v = rdl2.Vec3d(5.0)
        self.assertAlmostEqual(v.x, 5.0)
        self.assertAlmostEqual(v.z, 5.0)

    def test_repr(self):
        self.assertTrue(repr(rdl2.Vec3d(1, 2, 3)).startswith("Vec3d("))

    def test_from_list(self):
        v = rdl2.Vec3d([7.0, 8.0, 9.0])
        self.assertAlmostEqual(v.x, 7.0)
        self.assertAlmostEqual(v.y, 8.0)
        self.assertAlmostEqual(v.z, 9.0)

    def test_from_tuple(self):
        v = rdl2.Vec3d((1.1, 2.2, 3.3))
        self.assertAlmostEqual(v.x, 1.1)
        self.assertAlmostEqual(v.z, 3.3)

    def test_wrong_length_raises(self):
        with self.assertRaises(ValueError):
            rdl2.Vec3d([1.0, 2.0])
        with self.assertRaises(ValueError):
            rdl2.Vec3d([1.0, 2.0, 3.0, 4.0])


class TestVec4f(unittest.TestCase):
    def test_ctors(self):
        self.assertEqual(rdl2.Vec4f().w, 0.0)
        v = rdl2.Vec4f(2.0)
        self.assertAlmostEqual(v.x, 2.0)
        self.assertAlmostEqual(v.w, 2.0)
        v = rdl2.Vec4f(1.0, 2.0, 3.0, 4.0)
        self.assertAlmostEqual(v.w, 4.0)

    def test_repr(self):
        self.assertTrue(repr(rdl2.Vec4f(1, 2, 3, 4)).startswith("Vec4f("))

    def test_from_list(self):
        v = rdl2.Vec4f([1.0, 2.0, 3.0, 4.0])
        self.assertAlmostEqual(v.x, 1.0)
        self.assertAlmostEqual(v.y, 2.0)
        self.assertAlmostEqual(v.z, 3.0)
        self.assertAlmostEqual(v.w, 4.0)

    def test_from_tuple(self):
        v = rdl2.Vec4f((5.0, 6.0, 7.0, 8.0))
        self.assertAlmostEqual(v.x, 5.0)
        self.assertAlmostEqual(v.w, 8.0)

    def test_wrong_length_raises(self):
        with self.assertRaises(ValueError):
            rdl2.Vec4f([1.0, 2.0, 3.0])
        with self.assertRaises(ValueError):
            rdl2.Vec4f([1.0, 2.0, 3.0, 4.0, 5.0])


class TestVec4d(unittest.TestCase):
    def test_ctors(self):
        v = rdl2.Vec4d(1.0, 2.0, 3.0, 4.0)
        self.assertAlmostEqual(v.x, 1.0)
        self.assertAlmostEqual(v.w, 4.0)

    def test_repr(self):
        self.assertTrue(repr(rdl2.Vec4d(1, 2, 3, 4)).startswith("Vec4d("))

    def test_from_list(self):
        v = rdl2.Vec4d([1.0, 2.0, 3.0, 4.0])
        self.assertAlmostEqual(v.x, 1.0)
        self.assertAlmostEqual(v.w, 4.0)

    def test_from_tuple(self):
        v = rdl2.Vec4d((5.0, 6.0, 7.0, 8.0))
        self.assertAlmostEqual(v.x, 5.0)
        self.assertAlmostEqual(v.w, 8.0)

    def test_wrong_length_raises(self):
        with self.assertRaises(ValueError):
            rdl2.Vec4d([1.0, 2.0, 3.0])
        with self.assertRaises(ValueError):
            rdl2.Vec4d([1.0, 2.0, 3.0, 4.0, 5.0])


class TestMat4f(unittest.TestCase):
    def test_default_ctor(self):
        m = rdl2.Mat4f()
        self.assertIsInstance(m.vx, rdl2.Vec4f)
        self.assertIsInstance(m.vy, rdl2.Vec4f)
        self.assertIsInstance(m.vz, rdl2.Vec4f)
        self.assertIsInstance(m.vw, rdl2.Vec4f)

    def test_row_ctor(self):
        vx = rdl2.Vec4f(1, 0, 0, 0)
        vy = rdl2.Vec4f(0, 1, 0, 0)
        vz = rdl2.Vec4f(0, 0, 1, 0)
        vw = rdl2.Vec4f(0, 0, 0, 1)
        m = rdl2.Mat4f(vx, vy, vz, vw)
        self.assertAlmostEqual(m.vx.x, 1.0)
        self.assertAlmostEqual(m.vy.y, 1.0)
        self.assertAlmostEqual(m.vz.z, 1.0)
        self.assertAlmostEqual(m.vw.w, 1.0)

    def test_repr(self):
        self.assertTrue(repr(rdl2.Mat4f()).startswith("Mat4f("))

    def test_from_nested_lists(self):
        m = rdl2.Mat4f([[1, 0, 0, 0],
                        [0, 1, 0, 0],
                        [0, 0, 1, 0],
                        [0, 0, 0, 1]])
        self.assertAlmostEqual(m.vx.x, 1.0)
        self.assertAlmostEqual(m.vy.y, 1.0)
        self.assertAlmostEqual(m.vz.z, 1.0)
        self.assertAlmostEqual(m.vw.w, 1.0)

    def test_from_nested_tuples(self):
        m = rdl2.Mat4f(((2, 0, 0, 0),
                        (0, 3, 0, 0),
                        (0, 0, 4, 0),
                        (0, 0, 0, 1)))
        self.assertAlmostEqual(m.vx.x, 2.0)
        self.assertAlmostEqual(m.vy.y, 3.0)
        self.assertAlmostEqual(m.vz.z, 4.0)

    def test_from_mixed_vec4_and_list_rows(self):
        m = rdl2.Mat4f([rdl2.Vec4f(1, 0, 0, 0),
                        [0, 1, 0, 0],
                        rdl2.Vec4f(0, 0, 1, 0),
                        [0, 0, 0, 1]])
        self.assertAlmostEqual(m.vx.x, 1.0)
        self.assertAlmostEqual(m.vy.y, 1.0)
        self.assertAlmostEqual(m.vz.z, 1.0)
        self.assertAlmostEqual(m.vw.w, 1.0)

    def test_wrong_outer_length_raises(self):
        with self.assertRaises(ValueError):
            rdl2.Mat4f([[1, 0, 0, 0],
                        [0, 1, 0, 0],
                        [0, 0, 1, 0]])

    def test_wrong_inner_length_raises(self):
        with self.assertRaises(ValueError):
            rdl2.Mat4f([[1, 0, 0],
                        [0, 1, 0, 0],
                        [0, 0, 1, 0],
                        [0, 0, 0, 1]])


class TestMat4d(unittest.TestCase):
    def test_default_ctor(self):
        m = rdl2.Mat4d()
        self.assertIsInstance(m.vx, rdl2.Vec4d)

    def test_row_ctor(self):
        vx = rdl2.Vec4d(2, 0, 0, 0)
        vy = rdl2.Vec4d(0, 3, 0, 0)
        vz = rdl2.Vec4d(0, 0, 4, 0)
        vw = rdl2.Vec4d(0, 0, 0, 1)
        m = rdl2.Mat4d(vx, vy, vz, vw)
        self.assertAlmostEqual(m.vx.x, 2.0)
        self.assertAlmostEqual(m.vy.y, 3.0)
        self.assertAlmostEqual(m.vz.z, 4.0)

    def test_mutation(self):
        m = rdl2.Mat4d()
        m.vx = rdl2.Vec4d(9, 0, 0, 0)
        self.assertAlmostEqual(m.vx.x, 9.0)

    def test_repr(self):
        self.assertTrue(repr(rdl2.Mat4d()).startswith("Mat4d("))

    def test_from_nested_lists(self):
        m = rdl2.Mat4d([[1, 0, 0, 0],
                        [0, 1, 0, 0],
                        [0, 0, 1, 0],
                        [5, 6, 7, 1]])
        self.assertAlmostEqual(m.vw.x, 5.0)
        self.assertAlmostEqual(m.vw.y, 6.0)
        self.assertAlmostEqual(m.vw.z, 7.0)
        self.assertAlmostEqual(m.vw.w, 1.0)

    def test_from_nested_tuples(self):
        m = rdl2.Mat4d(((1, 0, 0, 0),
                        (0, 2, 0, 0),
                        (0, 0, 3, 0),
                        (0, 0, 0, 1)))
        self.assertAlmostEqual(m.vx.x, 1.0)
        self.assertAlmostEqual(m.vy.y, 2.0)
        self.assertAlmostEqual(m.vz.z, 3.0)

    def test_from_mixed_vec4d_and_list_rows(self):
        m = rdl2.Mat4d([rdl2.Vec4d(1, 0, 0, 0),
                        [0, 2, 0, 0],
                        rdl2.Vec4d(0, 0, 3, 0),
                        [0, 0, 0, 1]])
        self.assertAlmostEqual(m.vx.x, 1.0)
        self.assertAlmostEqual(m.vy.y, 2.0)
        self.assertAlmostEqual(m.vz.z, 3.0)

    def test_wrong_outer_length_raises(self):
        with self.assertRaises(ValueError):
            rdl2.Mat4d([[1, 0, 0, 0],
                        [0, 1, 0, 0]])

    def test_wrong_inner_length_raises(self):
        with self.assertRaises(ValueError):
            rdl2.Mat4d([[1, 0, 0],
                        [0, 1, 0, 0],
                        [0, 0, 1, 0],
                        [0, 0, 0, 1]])


# ===========================================================================
# Implicit conversion tests
# ===========================================================================

class TestMathImplicitConversion(_WithDsos):
    """Verify math types are implicitly constructed from lists/tuples at C++ boundaries."""

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.sv = cls.ctx.getSceneVariables()
        geo_name = _first_class_name(cls.ctx, rdl2.INTERFACE_GEOMETRY)
        cls.geo = cls.ctx.createSceneObject(geo_name, "/test/implicit/geo").asGeometry()

    def test_rgb_from_list_at_cpp_boundary(self):
        self.sv.beginUpdate()
        self.sv["fatal_color"] = [1.0, 0.0, 0.5]
        self.sv.endUpdate()
        result = self.sv["fatal_color"]
        self.assertIsInstance(result, rdl2.Rgb)
        self.assertAlmostEqual(result.r, 1.0)
        self.assertAlmostEqual(result.g, 0.0)
        self.assertAlmostEqual(result.b, 0.5)

    def test_rgb_from_tuple_at_cpp_boundary(self):
        self.sv.beginUpdate()
        self.sv["fatal_color"] = (0.2, 0.4, 0.6)
        self.sv.endUpdate()
        result = self.sv["fatal_color"]
        self.assertIsInstance(result, rdl2.Rgb)
        self.assertAlmostEqual(result.r, 0.2)
        self.assertAlmostEqual(result.b, 0.6)

    def test_mat4d_from_nested_list_at_cpp_boundary(self):
        # setNodeXform takes const Mat4d& — implicit conversion fires for lists
        self.geo.beginUpdate()
        self.geo.setNodeXform([[1, 0, 0, 0],
                               [0, 1, 0, 0],
                               [0, 0, 1, 0],
                               [5, 6, 7, 1]])
        self.geo.endUpdate()
        result = self.geo.getNodeXform()
        self.assertIsInstance(result, rdl2.Mat4d)
        self.assertAlmostEqual(result.vw.x, 5.0)
        self.assertAlmostEqual(result.vw.y, 6.0)
        self.assertAlmostEqual(result.vw.z, 7.0)

    def test_mat4d_from_nested_tuple_at_cpp_boundary(self):
        self.geo.beginUpdate()
        self.geo.setNodeXform(((1, 0, 0, 0),
                               (0, 1, 0, 0),
                               (0, 0, 1, 0),
                               (10, 20, 30, 1)))
        self.geo.endUpdate()
        result = self.geo.getNodeXform()
        self.assertAlmostEqual(result.vw.x, 10.0)
        self.assertAlmostEqual(result.vw.y, 20.0)
        self.assertAlmostEqual(result.vw.z, 30.0)


# ===========================================================================
# Enums
# ===========================================================================

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
                     "INTERFACE_GEOMETRY", "INTERFACE_LIGHT", "INTERFACE_SHADER",
                     "INTERFACE_MATERIAL", "INTERFACE_RENDEROUTPUT",
                     "INTERFACE_USERDATA", "INTERFACE_LIGHTFILTERSET",
                     "INTERFACE_SHADOWSET", "INTERFACE_SHADOWRECEIVERSET"]:
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


# ===========================================================================
# Free functions
# ===========================================================================

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


# ===========================================================================
# SceneContext
# ===========================================================================

class TestSceneContext(unittest.TestCase):
    def setUp(self):
        self.ctx = _make_ctx()

    def test_construction(self):
        self.assertIsInstance(self.ctx, rdl2.SceneContext)

    def test_dso_path_roundtrip(self):
        self.ctx.setDsoPath("/some/path")
        self.assertEqual(self.ctx.getDsoPath(), "/some/path")

    def test_proxy_mode_roundtrip(self):
        self.ctx.setProxyModeEnabled(True)
        self.assertTrue(self.ctx.getProxyModeEnabled())
        self.ctx.setProxyModeEnabled(False)
        self.assertFalse(self.ctx.getProxyModeEnabled())

    def test_get_scene_variables(self):
        sv = self.ctx.getSceneVariables()
        self.assertIsInstance(sv, rdl2.SceneVariables)

    def test_scene_variables_is_scene_object(self):
        sv = self.ctx.getSceneVariables()
        self.assertIsInstance(sv, rdl2.SceneObject)

    def test_load_all_scene_classes(self):
        self.ctx.setDsoPath(DSO_PATH)
        self.ctx.loadAllSceneClasses()
        classes = self.ctx.getAllSceneClasses()
        self.assertGreater(len(classes), 0)

    def test_get_all_scene_classes_type(self):
        self.ctx.setDsoPath(DSO_PATH)
        self.ctx.loadAllSceneClasses()
        for sc in self.ctx.getAllSceneClasses():
            self.assertIsInstance(sc, rdl2.SceneClass)

    def test_create_and_retrieve_scene_object(self):
        self.ctx.setDsoPath(DSO_PATH)
        self.ctx.loadAllSceneClasses()
        obj = self.ctx.createSceneObject("BoxGeometry", "/ctx/box")
        self.assertIsNotNone(obj)
        self.assertTrue(self.ctx.sceneObjectExists("/ctx/box"))
        retrieved = self.ctx.getSceneObject("/ctx/box")
        self.assertEqual(retrieved.getName(), "/ctx/box")

    def test_scene_object_not_found_raises(self):
        with self.assertRaises((ValueError, RuntimeError, KeyError)):
            self.ctx.getSceneObject("/nonexistent")

    def test_scene_object_exists_false(self):
        self.assertFalse(self.ctx.sceneObjectExists("/does/not/exist"))

    def test_get_all_scene_objects(self):
        self.ctx.setDsoPath(DSO_PATH)
        self.ctx.loadAllSceneClasses()
        self.ctx.createSceneObject("BoxGeometry", "/ctx/box1")
        self.ctx.createSceneObject("BoxGeometry", "/ctx/box2")
        objs = self.ctx.getAllSceneObjects()
        names = [o.getName() for o in objs]
        self.assertIn("/ctx/box1", names)
        self.assertIn("/ctx/box2", names)

    def test_create_and_retrieve_scene_class(self):
        self.ctx.setDsoPath(DSO_PATH)
        self.ctx.loadAllSceneClasses()
        sc = self.ctx.createSceneClass("BoxGeometry")
        self.assertIsNotNone(sc)
        self.assertTrue(self.ctx.sceneClassExists("BoxGeometry"))
        sc2 = self.ctx.getSceneClass("BoxGeometry")
        self.assertEqual(sc2.getName(), "BoxGeometry")

    def test_scene_class_not_found_raises(self):
        with self.assertRaises((ValueError, RuntimeError, KeyError)):
            self.ctx.getSceneClass("NonExistentClass")

    def test_checkpoint_flags_are_bool(self):
        self.assertIsInstance(self.ctx.getCheckpointActive(), bool)
        self.assertIsInstance(self.ctx.getResumableOutput(), bool)
        self.assertIsInstance(self.ctx.getResumeRender(), bool)

    def test_render2world_roundtrip(self):
        xform = rdl2.Mat4d(
            rdl2.Vec4d(1, 0, 0, 0),
            rdl2.Vec4d(0, 1, 0, 0),
            rdl2.Vec4d(0, 0, 1, 0),
            rdl2.Vec4d(5, 6, 7, 1),
        )
        self.ctx.setRender2World(xform)
        result = self.ctx.getRender2World()
        self.assertAlmostEqual(result.vw.x, 5.0)
        self.assertAlmostEqual(result.vw.y, 6.0)
        self.assertAlmostEqual(result.vw.z, 7.0)

    def test_commit_all_changes_no_crash(self):
        self.ctx.commitAllChanges()  # should not raise


# ===========================================================================
# SceneClass and Attribute
# ===========================================================================

class TestSceneClass(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.sc = cls.ctx.getSceneClass("BoxGeometry")

    def test_get_name(self):
        self.assertEqual(self.sc.getName(), "BoxGeometry")

    def test_repr(self):
        self.assertIn("BoxGeometry", repr(self.sc))

    def test_declared_interface(self):
        iface = self.sc.getDeclaredInterface()
        self.assertTrue(iface & rdl2.INTERFACE_GEOMETRY)

    def test_has_attribute_true(self):
        # All geometry objects have node_xform from Node
        self.assertTrue(self.sc.hasAttribute("node_xform"))

    def test_has_attribute_false(self):
        self.assertFalse(self.sc.hasAttribute("nonexistent_attr_xyz"))

    def test_get_attribute_returns_attribute(self):
        attr = self.sc.getAttribute("node_xform")
        self.assertIsInstance(attr, rdl2.Attribute)

    def test_get_attributes_non_empty(self):
        attrs = self.sc.getAttributes()
        self.assertIsInstance(attrs, list)
        self.assertGreater(len(attrs), 0)
        for a in attrs:
            self.assertIsInstance(a, rdl2.Attribute)

    def test_get_group_names(self):
        groups = self.sc.getGroupNames()
        self.assertIsInstance(groups, list)

    def test_get_source_path(self):
        path = self.sc.getSourcePath()
        self.assertIsInstance(path, str)

    def test_get_scene_context_back_reference(self):
        ctx = self.sc.getSceneContext()
        self.assertIsInstance(ctx, rdl2.SceneContext)


class TestAttribute(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.sc = cls.ctx.getSceneClass("BoxGeometry")
        cls.xform_attr = cls.sc.getAttribute("node_xform")
        # SceneVariables has a wide variety of types
        sv_sc = cls.ctx.getSceneVariables().getSceneClass()
        cls.sv_attrs = {a.getType(): a for a in sv_sc.getAttributes()}

    def test_get_name(self):
        self.assertEqual(self.xform_attr.getName(), "node_xform")

    def test_repr(self):
        r = repr(self.xform_attr)
        self.assertIn("node_xform", r)

    def test_get_type(self):
        self.assertEqual(self.xform_attr.getType(), rdl2.AttributeType.TYPE_MAT4D)

    def test_get_flags(self):
        flags = self.xform_attr.getFlags()
        self.assertIsInstance(flags, rdl2.AttributeFlags)

    def test_is_blurrable(self):
        self.assertIsInstance(self.xform_attr.isBlurrable(), bool)

    def test_is_bindable(self):
        self.assertIsInstance(self.xform_attr.isBindable(), bool)

    def test_is_enumerable(self):
        self.assertIsInstance(self.xform_attr.isEnumerable(), bool)

    def test_is_filename(self):
        self.assertIsInstance(self.xform_attr.isFilename(), bool)

    def test_metadata_dict_is_dict(self):
        md = self.xform_attr.getMetadataDict()
        self.assertIsInstance(md, dict)

    def test_update_requires_geom_reload(self):
        result = self.xform_attr.updateRequiresGeomReload()
        self.assertIsInstance(result, bool)

    def test_get_aliases(self):
        aliases = self.xform_attr.getAliases()
        self.assertIsInstance(aliases, list)

    def test_type_coverage_in_scene_variables(self):
        # Verify we have attributes of the main scalar types in SceneVariables
        expected_types = [
            rdl2.AttributeType.TYPE_BOOL,
            rdl2.AttributeType.TYPE_INT,
            rdl2.AttributeType.TYPE_FLOAT,
            rdl2.AttributeType.TYPE_STRING,
            rdl2.AttributeType.TYPE_RGB,
        ]
        for t in expected_types:
            with self.subTest(type=rdl2.attributeTypeName(t)):
                self.assertIn(t, self.sv_attrs, f"No attribute of type {t} in SceneVariables")


# ===========================================================================
# SceneObject — get/set/tracking
# ===========================================================================

class TestSceneObject(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.box = cls.ctx.createSceneObject("BoxGeometry", "/test/so/box")
        cls.sv = cls.ctx.getSceneVariables()

    def test_get_name(self):
        self.assertEqual(self.box.getName(), "/test/so/box")

    def test_get_scene_class(self):
        sc = self.box.getSceneClass()
        self.assertIsInstance(sc, rdl2.SceneClass)
        self.assertEqual(sc.getName(), "BoxGeometry")

    def test_get_type_is_interface_enum(self):
        t = self.box.getType()
        self.assertIsInstance(t, rdl2.SceneObjectInterface)

    def test_repr(self):
        r = repr(self.box)
        self.assertIn("BoxGeometry", r)
        self.assertIn("/test/so/box", r)

    def test_is_a_geometry(self):
        self.assertTrue(self.box.isA(rdl2.INTERFACE_GEOMETRY))

    def test_is_a_node(self):
        self.assertTrue(self.box.isA(rdl2.INTERFACE_NODE))

    def test_is_not_a_camera(self):
        self.assertFalse(self.box.isA(rdl2.INTERFACE_CAMERA))

    def test_is_geometry_predicate(self):
        self.assertTrue(self.box.isGeometry())

    def test_is_node_predicate(self):
        self.assertTrue(self.box.isNode())

    def test_is_not_camera_predicate(self):
        self.assertFalse(self.box.isCamera())

    def test_is_not_light_predicate(self):
        self.assertFalse(self.box.isLight())

    # ---- get/set scalar types via SceneVariables ----

    def _sv_set_get(self, attr_name, value):
        """Helper: set then get an attribute on SceneVariables and return the result."""
        self.sv.beginUpdate()
        self.sv[attr_name] = value
        self.sv.endUpdate()
        return self.sv[attr_name]

    def test_get_set_bool(self):
        orig = self.sv["slerp_xforms"]
        result = self._sv_set_get("slerp_xforms", True)
        self.assertIs(result, True)
        # restore
        self._sv_set_get("slerp_xforms", orig)

    def test_get_set_int(self):
        orig = self.sv["image_width"]
        result = self._sv_set_get("image_width", 1280)
        self.assertEqual(result, 1280)
        self._sv_set_get("image_width", orig)

    def test_get_set_float(self):
        orig = self.sv["frame"]
        result = self._sv_set_get("frame", 42.5)
        self.assertAlmostEqual(result, 42.5)
        self._sv_set_get("frame", orig)

    def test_get_set_string(self):
        orig = self.sv["output_file"]
        result = self._sv_set_get("output_file", "/tmp/test.exr")
        self.assertEqual(result, "/tmp/test.exr")
        self._sv_set_get("output_file", orig)

    def test_get_set_rgb(self):
        orig = self.sv["fatal_color"]
        new_val = rdl2.Rgb(1.0, 0.0, 0.5)
        result = self._sv_set_get("fatal_color", new_val)
        self.assertIsInstance(result, rdl2.Rgb)
        self.assertAlmostEqual(result.r, 1.0)
        self.assertAlmostEqual(result.b, 0.5)
        self._sv_set_get("fatal_color", orig)

    def test_get_with_timestep(self):
        # tuple key: (attr_name, timestep)
        val = self.sv["frame", rdl2.TIMESTEP_BEGIN]
        self.assertIsInstance(val, float)

    def test_get_returns_correct_types(self):
        self.assertIsInstance(self.sv["image_width"], int)
        self.assertIsInstance(self.sv["frame"], float)
        self.assertIsInstance(self.sv["slerp_xforms"], bool)
        self.assertIsInstance(self.sv["output_file"], str)
        self.assertIsInstance(self.sv["fatal_color"], rdl2.Rgb)

    # ---- update guard ----

    def test_begin_end_update(self):
        self.sv.beginUpdate()
        self.sv["image_width"] = 640
        self.sv.endUpdate()
        self.assertEqual(self.sv["image_width"], 640)

    def test_update_guard_context_manager(self):
        with rdl2.UpdateGuard(self.sv):
            self.sv["image_height"] = 480
        self.assertEqual(self.sv["image_height"], 480)

    # ---- change tracking ----

    def test_has_changed(self):
        self.sv.beginUpdate()
        self.sv["frame"] = 999.0
        self.sv.endUpdate()
        self.assertTrue(self.sv.hasChanged("frame"))

    def test_is_dirty(self):
        self.sv.beginUpdate()
        self.sv["frame"] = 1.0
        self.sv.endUpdate()
        # After a set, isDirty should be true (commitAllChanges clears it)
        self.assertTrue(self.sv.isDirty())

    def test_request_update(self):
        self.sv.requestUpdate()  # should not raise

    # ---- default / reset ----

    def test_is_default_after_reset(self):
        self.sv.beginUpdate()
        self.sv["fps"] = 30.0
        self.sv.resetToDefault("fps")
        self.sv.endUpdate()
        self.assertTrue(self.sv.isDefault("fps"))

    def test_reset_all_to_default(self):
        self.sv.beginUpdate()
        self.sv["image_width"] = 9999
        self.sv.resetAllToDefault()
        self.sv.endUpdate()
        self.assertTrue(self.sv.isDefault("image_width"))

    def test_is_default_and_unbound(self):
        self.sv.beginUpdate()
        self.sv.resetToDefault("output_file")
        self.sv.endUpdate()
        result = self.sv.isDefaultAndUnbound("output_file")
        self.assertIsInstance(result, bool)

    # ---- copy ----

    def test_copy_all(self):
        src = self.ctx.createSceneObject("BoxGeometry", "/test/so/box_src")
        dst = self.ctx.createSceneObject("BoxGeometry", "/test/so/box_dst")
        dst.beginUpdate()
        dst.copyAll(src)
        dst.endUpdate()

    # ---- dict-style access ----

    def test_contains_known_attr(self):
        self.assertIn("image_width", self.sv)

    def test_contains_unknown_attr(self):
        self.assertNotIn("nonexistent_attribute_xyz", self.sv)

    def test_getitem_string_key(self):
        val = self.sv["image_width"]
        self.assertIsInstance(val, int)

    def test_setitem_string_key(self):
        orig = self.sv["image_width"]
        self.sv.beginUpdate()
        self.sv["image_width"] = 512
        self.sv.endUpdate()
        self.assertEqual(self.sv["image_width"], 512)
        self.sv.beginUpdate()
        self.sv["image_width"] = orig
        self.sv.endUpdate()

    def test_getitem_timestep_tuple_key(self):
        val = self.sv["frame", rdl2.TIMESTEP_BEGIN]
        self.assertIsInstance(val, float)

    def test_setitem_timestep_tuple_key(self):
        orig = self.sv["frame", rdl2.TIMESTEP_BEGIN]
        self.sv.beginUpdate()
        self.sv["frame", rdl2.TIMESTEP_BEGIN] = 77.0
        self.sv.endUpdate()
        self.assertAlmostEqual(self.sv["frame"], 77.0)
        self.sv.beginUpdate()
        self.sv["frame"] = orig
        self.sv.endUpdate()

    def test_getitem_bad_key_type_raises(self):
        with self.assertRaises((KeyError, TypeError)):
            _ = self.sv[42]

    def test_getitem_bad_tuple_length_raises(self):
        with self.assertRaises((KeyError, TypeError)):
            _ = self.sv["frame", rdl2.TIMESTEP_BEGIN, "extra"]

    def test_getitem_unknown_attr_raises(self):
        with self.assertRaises(Exception):
            _ = self.sv["this_attr_does_not_exist"]


# ===========================================================================
# SceneVariables
# ===========================================================================

class TestSceneVariables(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.sv = cls.ctx.getSceneVariables()

    def test_is_scene_object(self):
        self.assertIsInstance(self.sv, rdl2.SceneObject)

    def test_is_scene_variables(self):
        self.assertIsInstance(self.sv, rdl2.SceneVariables)

    def test_rezed_width_is_int(self):
        self.assertIsInstance(self.sv.getRezedWidth(), int)

    def test_rezed_height_is_int(self):
        self.assertIsInstance(self.sv.getRezedHeight(), int)

    def test_machine_id_is_int(self):
        self.assertIsInstance(self.sv.getMachineId(), int)

    def test_num_machines_is_int(self):
        self.assertIsInstance(self.sv.getNumMachines(), int)

    def test_num_machines_positive(self):
        self.assertGreater(self.sv.getNumMachines(), 0)

    def test_get_tmp_dir_is_str(self):
        self.assertIsInstance(self.sv.getTmpDir(), str)

    def test_get_camera_is_none_or_camera(self):
        cam = self.sv.getCamera()
        # getCamera() returns Camera* — None if unset, or a Camera (or SceneObject
        # wrapping a Camera from a DSO) if set. Use isCamera() as the predicate.
        self.assertTrue(cam is None or cam.isCamera())

    def test_get_layer_is_none_or_layer(self):
        layer = self.sv.getLayer()
        self.assertTrue(layer is None or isinstance(layer, rdl2.Layer))


# ===========================================================================
# Node / Camera / Geometry hierarchy
# ===========================================================================

class TestNode(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        # Find a Camera and a Geometry DSO class
        cam_name = _first_class_name(cls.ctx, rdl2.INTERFACE_CAMERA)
        geo_name = _first_class_name(cls.ctx, rdl2.INTERFACE_GEOMETRY)
        cls.assertIsNotNone(cls, cam_name, "No Camera DSO found")
        cls.assertIsNotNone(cls, geo_name, "No Geometry DSO found")
        # Use as*() to get the correctly typed Python objects
        cls.cam = cls.ctx.createSceneObject(cam_name, "/test/node/cam").asCamera()
        cls.geo = cls.ctx.createSceneObject(geo_name, "/test/node/geo").asGeometry()

    def test_camera_is_node(self):
        # createSceneObject returns SceneObject; as*() gives the derived type
        self.assertIsInstance(self.cam, rdl2.Camera)  # asCamera() already applied in setUpClass

    def test_camera_is_camera(self):
        self.assertIsInstance(self.cam, rdl2.Camera)

    def test_geometry_is_node(self):
        self.assertIsInstance(self.geo, rdl2.Geometry)  # asGeometry() applied in setUpClass

    def test_geometry_is_geometry(self):
        self.assertIsInstance(self.geo, rdl2.Geometry)

    def test_get_node_xform_returns_mat4d(self):
        xform = self.cam.getNodeXform()
        self.assertIsInstance(xform, rdl2.Mat4d)

    def test_set_get_node_xform(self):
        xform = rdl2.Mat4d(
            rdl2.Vec4d(1, 0, 0, 0),
            rdl2.Vec4d(0, 1, 0, 0),
            rdl2.Vec4d(0, 0, 1, 0),
            rdl2.Vec4d(10, 20, 30, 1),
        )
        self.cam.beginUpdate()
        self.cam.setNodeXform(xform)
        self.cam.endUpdate()
        result = self.cam.getNodeXform()
        self.assertAlmostEqual(result.vw.x, 10.0)
        self.assertAlmostEqual(result.vw.y, 20.0)
        self.assertAlmostEqual(result.vw.z, 30.0)

    def test_camera_near_far_default(self):
        near = self.cam.getNear()
        far = self.cam.getFar()
        self.assertIsInstance(near, float)
        self.assertIsInstance(far, float)
        self.assertGreater(far, near)

    def test_camera_set_near_far(self):
        self.cam.beginUpdate()
        self.cam.setNear(0.1)
        self.cam.setFar(5000.0)
        self.cam.endUpdate()
        self.assertAlmostEqual(self.cam.getNear(), 0.1)
        self.assertAlmostEqual(self.cam.getFar(), 5000.0)

    def test_geometry_is_static(self):
        self.assertIsInstance(self.geo.isStatic(), bool)

    def test_geometry_get_side_type(self):
        st = self.geo.getSideType()
        self.assertIsInstance(st, rdl2.GeometrySideType)

    def test_geometry_get_reverse_normals(self):
        self.assertIsInstance(self.geo.getReverseNormals(), bool)

    def test_geometry_get_visibility_mask(self):
        self.assertIsInstance(self.geo.getVisibilityMask(), int)


# ===========================================================================
# Type hierarchy — isA / asA / isinstance
# ===========================================================================

class TestTypeHierarchy(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cam_name = _first_class_name(cls.ctx, rdl2.INTERFACE_CAMERA)
        geo_name = _first_class_name(cls.ctx, rdl2.INTERFACE_GEOMETRY)
        cls.cam_obj = cls.ctx.createSceneObject(cam_name, "/test/hier/cam")
        cls.geo_obj = cls.ctx.createSceneObject(geo_name, "/test/hier/geo")

    def test_camera_predicates(self):
        cam = self.cam_obj
        self.assertTrue(cam.isCamera())
        self.assertTrue(cam.isNode())
        self.assertFalse(cam.isGeometry())
        self.assertFalse(cam.isLight())
        self.assertFalse(cam.isLayer())

    def test_geometry_predicates(self):
        geo = self.geo_obj
        self.assertTrue(geo.isGeometry())
        self.assertTrue(geo.isNode())
        self.assertFalse(geo.isCamera())
        self.assertFalse(geo.isLight())

    def test_as_camera_returns_camera(self):
        cam = self.cam_obj.asCamera()
        self.assertIsNotNone(cam)
        self.assertIsInstance(cam, rdl2.Camera)

    def test_as_geometry_returns_geometry(self):
        geo = self.geo_obj.asGeometry()
        self.assertIsNotNone(geo)
        self.assertIsInstance(geo, rdl2.Geometry)

    def test_as_camera_on_geometry_returns_none(self):
        result = self.geo_obj.asCamera()
        self.assertIsNone(result)

    def test_as_node_on_camera(self):
        node = self.cam_obj.asNode()
        self.assertIsNotNone(node)
        self.assertIsInstance(node, rdl2.Node)

    def test_isinstance_chain_camera(self):
        # createSceneObject returns SceneObject; asCamera() gives the Camera-typed object
        cam = self.cam_obj
        self.assertIsInstance(cam, rdl2.SceneObject)
        cam_typed = cam.asCamera()
        self.assertIsInstance(cam_typed, rdl2.Camera)
        self.assertIsInstance(cam_typed, rdl2.Node)

    def test_isinstance_chain_geometry(self):
        geo = self.geo_obj
        self.assertIsInstance(geo, rdl2.SceneObject)
        geo_typed = geo.asGeometry()
        self.assertIsInstance(geo_typed, rdl2.Geometry)
        self.assertIsInstance(geo_typed, rdl2.Node)


# ===========================================================================
# GeometrySet and LightSet
# ===========================================================================

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
        gset.beginUpdate()
        gset.add(self.geo1)
        gset.endUpdate()
        self.assertTrue(gset.contains(self.geo1))

    def test_remove(self):
        gset = self.ctx.createSceneObject("GeometrySet", "/test/geoset/rem")
        gset.beginUpdate()
        gset.add(self.geo1)
        gset.remove(self.geo1)
        gset.endUpdate()
        self.assertFalse(gset.contains(self.geo1))

    def test_clear(self):
        gset = self.ctx.createSceneObject("GeometrySet", "/test/geoset/clr")
        gset.beginUpdate()
        gset.add(self.geo1)
        gset.add(self.geo2)
        gset.endUpdate()
        gset.beginUpdate()
        gset.clear()
        gset.endUpdate()
        self.assertEqual(len(gset.getGeometries()), 0)

    def test_get_geometries_returns_list(self):
        gset = self.ctx.createSceneObject("GeometrySet", "/test/geoset/lst")
        gset.beginUpdate()
        gset.add(self.geo1)
        gset.add(self.geo2)
        gset.endUpdate()
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
        lset.beginUpdate()
        lset.add(self.light1)
        lset.endUpdate()
        self.assertTrue(lset.contains(self.light1))

    def test_remove(self):
        lset = self.ctx.createSceneObject("LightSet", "/test/lightset/rem")
        lset.beginUpdate()
        lset.add(self.light1)
        lset.remove(self.light1)
        lset.endUpdate()
        self.assertFalse(lset.contains(self.light1))

    def test_clear(self):
        lset = self.ctx.createSceneObject("LightSet", "/test/lightset/clr")
        lset.beginUpdate()
        lset.add(self.light1)
        lset.add(self.light2)
        lset.endUpdate()
        lset.beginUpdate()
        lset.clear()
        lset.endUpdate()
        self.assertEqual(len(lset.getLights()), 0)


# ===========================================================================
# LayerAssignment and Layer
# ===========================================================================

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
        geo_name  = _first_class_name(cls.ctx, rdl2.INTERFACE_GEOMETRY)
        mat_name  = _first_class_name(cls.ctx, rdl2.INTERFACE_MATERIAL)
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
        self.layer.beginUpdate()
        aid = self.layer.assign(self.geo, "", self.mat, self.lset)
        self.layer.endUpdate()
        self.assertIsInstance(aid, int)

    def test_lookup_after_assign(self):
        self.layer.beginUpdate()
        aid = self.layer.assign(self.geo, "", self.mat, self.lset)
        self.layer.endUpdate()
        mat = self.layer.lookupMaterial(aid)
        lset = self.layer.lookupLightSet(aid)
        self.assertIsNotNone(mat)
        self.assertIsNotNone(lset)

    def test_clear(self):
        self.layer.beginUpdate()
        self.layer.clear()
        self.layer.endUpdate()

    def test_light_sets_changed_is_bool(self):
        self.assertIsInstance(self.layer.lightSetsChanged(), bool)


# ===========================================================================
# RenderOutput
# ===========================================================================

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


# ===========================================================================
# AsciiReader / AsciiWriter
# ===========================================================================

class TestAsciiWriter(unittest.TestCase):
    def setUp(self):
        self.ctx = _make_ctx()

    def test_construction(self):
        writer = rdl2.AsciiWriter(self.ctx)
        self.assertIsInstance(writer, rdl2.AsciiWriter)

    def test_to_string_returns_str(self):
        writer = rdl2.AsciiWriter(self.ctx)
        s = writer.toString()
        self.assertIsInstance(s, str)

    def test_to_string_non_empty(self):
        writer = rdl2.AsciiWriter(self.ctx)
        s = writer.toString()
        self.assertGreater(len(s), 0)

    def test_to_string_contains_scene_variables(self):
        writer = rdl2.AsciiWriter(self.ctx)
        s = writer.toString()
        self.assertIn("SceneVariables", s)

    def test_set_skip_defaults(self):
        writer = rdl2.AsciiWriter(self.ctx)
        writer.setSkipDefaults(True)
        writer.setSkipDefaults(False)  # both should not raise

    def test_set_delta_encoding(self):
        writer = rdl2.AsciiWriter(self.ctx)
        writer.setDeltaEncoding(True)
        writer.setDeltaEncoding(False)

    def test_set_elements_per_line(self):
        writer = rdl2.AsciiWriter(self.ctx)
        writer.setElementsPerLine(4)

    def test_to_file(self):
        writer = rdl2.AsciiWriter(self.ctx)
        with tempfile.NamedTemporaryFile(suffix=".rdla", delete=False) as f:
            tmp_path = f.name
        try:
            writer.toFile(tmp_path)
            self.assertTrue(os.path.exists(tmp_path))
            self.assertGreater(os.path.getsize(tmp_path), 0)
        finally:
            os.unlink(tmp_path)


class TestAsciiReader(unittest.TestCase):
    def setUp(self):
        self.ctx = _make_ctx()

    def test_construction(self):
        reader = rdl2.AsciiReader(self.ctx)
        self.assertIsInstance(reader, rdl2.AsciiReader)

    def test_set_warnings_as_errors(self):
        reader = rdl2.AsciiReader(self.ctx)
        reader.setWarningsAsErrors(True)
        reader.setWarningsAsErrors(False)


class TestAsciiRoundTrip(unittest.TestCase):
    """Write a context to RDLA, read it back into a new context, verify content."""

    def test_round_trip_scene_variables(self):
        # Write
        write_ctx = _make_ctx()
        write_ctx.getSceneVariables().beginUpdate()
        write_ctx.getSceneVariables()["image_width"] = 1234
        write_ctx.getSceneVariables().endUpdate()

        writer = rdl2.AsciiWriter(write_ctx)
        writer.setSkipDefaults(False)
        rdla = writer.toString()
        self.assertIn("SceneVariables", rdla)

        # Read back
        read_ctx = _make_ctx()
        reader = rdl2.AsciiReader(read_ctx)
        reader.fromString(rdla)
        self.assertEqual(read_ctx.getSceneVariables()["image_width"], 1234)

    def test_round_trip_preserves_object_names(self):
        write_ctx = _make_ctx(load_dsos=True)
        write_ctx.createSceneObject("RenderOutput", "/test/rt/ro1")

        writer = rdl2.AsciiWriter(write_ctx)
        rdla = writer.toString()

        read_ctx = _make_ctx(load_dsos=True)
        reader = rdl2.AsciiReader(read_ctx)
        reader.fromString(rdla)
        self.assertTrue(read_ctx.sceneObjectExists("/test/rt/ro1"))


if __name__ == "__main__":
    unittest.main()
