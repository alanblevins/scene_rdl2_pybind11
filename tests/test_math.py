#!/usr/bin/env python3
# Copyright (c) 2026 Alan Blevins
# SPDX-License-Identifier: MIT
"""Tests for math types: Rgb, Rgba, Vec2f/d, Vec3f/d, Vec4f/d, Mat4f/d."""

import unittest

from .helpers import rdl2, _WithDsos, _first_class_name


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


class TestMathImplicitConversion(_WithDsos):
    """Verify math types are implicitly constructed from lists/tuples at C++ boundaries."""

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.sv = cls.ctx.getSceneVariables()
        geo_name = _first_class_name(cls.ctx, rdl2.INTERFACE_GEOMETRY)
        cls.geo = cls.ctx.createSceneObject(geo_name, "/test/implicit/geo").asGeometry()

    def test_rgb_from_list_at_cpp_boundary(self):
        self.sv["fatal_color"] = [1.0, 0.0, 0.5]
        result = self.sv["fatal_color"]
        self.assertIsInstance(result, rdl2.Rgb)
        self.assertAlmostEqual(result.r, 1.0)
        self.assertAlmostEqual(result.g, 0.0)
        self.assertAlmostEqual(result.b, 0.5)

    def test_rgb_from_tuple_at_cpp_boundary(self):
        self.sv["fatal_color"] = (0.2, 0.4, 0.6)
        result = self.sv["fatal_color"]
        self.assertIsInstance(result, rdl2.Rgb)
        self.assertAlmostEqual(result.r, 0.2)
        self.assertAlmostEqual(result.b, 0.6)

    def test_mat4d_from_nested_list_at_cpp_boundary(self):
        # setNodeXform takes const Mat4d& — implicit conversion fires for lists
        self.geo.setNodeXform([[1, 0, 0, 0],
                               [0, 1, 0, 0],
                               [0, 0, 1, 0],
                               [5, 6, 7, 1]])
        result = self.geo.getNodeXform()
        self.assertIsInstance(result, rdl2.Mat4d)
        self.assertAlmostEqual(result.vw.x, 5.0)
        self.assertAlmostEqual(result.vw.y, 6.0)
        self.assertAlmostEqual(result.vw.z, 7.0)

    def test_mat4d_from_nested_tuple_at_cpp_boundary(self):
        self.geo.setNodeXform(((1, 0, 0, 0),
                               (0, 1, 0, 0),
                               (0, 0, 1, 0),
                               (10, 20, 30, 1)))
        result = self.geo.getNodeXform()
        self.assertAlmostEqual(result.vw.x, 10.0)
        self.assertAlmostEqual(result.vw.y, 20.0)
        self.assertAlmostEqual(result.vw.z, 30.0)


if __name__ == "__main__":
    unittest.main()
