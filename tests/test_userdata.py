#!/usr/bin/env python3
# Copyright (c) 2026 Alan Blevins
# SPDX-License-Identifier: MIT
"""Tests for UserData typed key/value channel bindings."""

import unittest

from .helpers import rdl2, _WithDsos, _make_ctx


class TestUserData(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.ud_obj = cls.ctx.createSceneObject("UserData", "/test/userdata/ud1")

    def _ud(self):
        """Return a fresh UserData view (re-cast each test to get a clean state)."""
        return self.ud_obj.asUserData()

    # --- identity / downcast ------------------------------------------------

    def test_is_user_data(self):
        self.assertTrue(self.ud_obj.isUserData())

    def test_as_user_data(self):
        ud = self._ud()
        self.assertIsNotNone(ud)
        self.assertIsInstance(ud, rdl2.UserData)

    def test_as_user_data_wrong_type_returns_none(self):
        ctx = _make_ctx(load_dsos=True)
        non_ud = ctx.createSceneObject("RenderOutput", "/test/userdata/not_ud")
        self.assertIsNone(non_ud.asUserData())

    # --- Rate enum ----------------------------------------------------------

    def test_rate_enum_values(self):
        self.assertEqual(int(rdl2.UserData.Rate.AUTO),         0)
        self.assertEqual(int(rdl2.UserData.Rate.CONSTANT),     1)
        self.assertEqual(int(rdl2.UserData.Rate.PART),         2)
        self.assertEqual(int(rdl2.UserData.Rate.UNIFORM),      3)
        self.assertEqual(int(rdl2.UserData.Rate.VERTEX),       4)
        self.assertEqual(int(rdl2.UserData.Rate.VARYING),      5)
        self.assertEqual(int(rdl2.UserData.Rate.FACE_VARYING), 6)

    def test_set_get_rate(self):
        ud = self._ud()
        ud.setRate(rdl2.UserData.Rate.VERTEX)
        self.assertEqual(ud.getRate(), rdl2.UserData.Rate.VERTEX)

    def test_set_rate_roundtrip_all_values(self):
        ud = self._ud()
        for rate in (rdl2.UserData.Rate.AUTO, rdl2.UserData.Rate.CONSTANT,
                     rdl2.UserData.Rate.FACE_VARYING):
            ud.setRate(rate)
            self.assertEqual(ud.getRate(), rate)

    # --- Bool channel -------------------------------------------------------

    def test_has_bool_data_initially_false(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_bool0").asUserData()
        self.assertFalse(ud.hasBoolData())

    def test_set_get_bool_data(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_bool1").asUserData()
        ud.setBoolData("my_bool", [True, False, True])
        self.assertTrue(ud.hasBoolData())
        self.assertEqual(ud.getBoolKey(), "my_bool")
        self.assertEqual(list(ud.getBoolValues()), [True, False, True])

    # --- Int channel --------------------------------------------------------

    def test_has_int_data_initially_false(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_int0").asUserData()
        self.assertFalse(ud.hasIntData())

    def test_set_get_int_data(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_int1").asUserData()
        ud.setIntData("my_int", [10, 20, 30])
        self.assertTrue(ud.hasIntData())
        self.assertEqual(ud.getIntKey(), "my_int")
        self.assertEqual(list(ud.getIntValues()), [10, 20, 30])

    # --- Float channel (single and dual timestep) ---------------------------

    def test_has_float_data_initially_false(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_flt0").asUserData()
        self.assertFalse(ud.hasFloatData())

    def test_set_get_float_data_single(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_flt1").asUserData()
        ud.setFloatData("my_float", [1.0, 2.0, 3.0])
        self.assertTrue(ud.hasFloatData())
        self.assertTrue(ud.hasFloatData0())
        self.assertFalse(ud.hasFloatData1())
        self.assertEqual(ud.getFloatKey(), "my_float")
        vals = list(ud.getFloatValues())
        self.assertAlmostEqual(vals[0], 1.0)
        self.assertAlmostEqual(vals[1], 2.0)
        self.assertAlmostEqual(vals[2], 3.0)

    def test_set_get_float_data_blur(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_flt2").asUserData()
        ud.setFloatData("blur_float", [1.0, 2.0], [3.0, 4.0])
        self.assertTrue(ud.hasFloatData())
        self.assertTrue(ud.hasFloatData0())
        self.assertTrue(ud.hasFloatData1())
        self.assertAlmostEqual(list(ud.getFloatValues0())[0], 1.0)
        self.assertAlmostEqual(list(ud.getFloatValues1())[0], 3.0)

    # --- String channel -----------------------------------------------------

    def test_has_string_data_initially_false(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_str0").asUserData()
        self.assertFalse(ud.hasStringData())

    def test_set_get_string_data(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_str1").asUserData()
        ud.setStringData("my_str", ["hello", "world"])
        self.assertTrue(ud.hasStringData())
        self.assertEqual(ud.getStringKey(), "my_str")
        self.assertEqual(list(ud.getStringValues()), ["hello", "world"])

    # --- Color / Rgb channel (single and dual timestep) --------------------

    def test_has_color_data_initially_false(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_clr0").asUserData()
        self.assertFalse(ud.hasColorData())

    def test_set_get_color_data_single(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_clr1").asUserData()
        c0 = rdl2.Rgb(1.0, 0.0, 0.0)
        c1 = rdl2.Rgb(0.0, 1.0, 0.0)
        ud.setColorData("my_color", [c0, c1])
        self.assertTrue(ud.hasColorData())
        self.assertTrue(ud.hasColorData0())
        self.assertFalse(ud.hasColorData1())
        self.assertEqual(ud.getColorKey(), "my_color")

    def test_set_get_color_data_blur(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_clr2").asUserData()
        c0 = rdl2.Rgb(1.0, 0.0, 0.0)
        c1 = rdl2.Rgb(0.0, 1.0, 0.0)
        ud.setColorData("blur_color", [c0], [c1])
        self.assertTrue(ud.hasColorData0())
        self.assertTrue(ud.hasColorData1())
        result0 = list(ud.getColorValues0())
        self.assertAlmostEqual(result0[0].r, 1.0)
        result1 = list(ud.getColorValues1())
        self.assertAlmostEqual(result1[0].g, 1.0)

    # --- Vec2f channel (single and dual timestep) --------------------------

    def test_has_vec2f_data_initially_false(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_v2f0").asUserData()
        self.assertFalse(ud.hasVec2fData())

    def test_set_get_vec2f_data_single(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_v2f1").asUserData()
        v0 = rdl2.Vec2f(1.0, 2.0)
        v1 = rdl2.Vec2f(3.0, 4.0)
        ud.setVec2fData("my_vec2f", [v0, v1])
        self.assertTrue(ud.hasVec2fData())
        self.assertEqual(ud.getVec2fKey(), "my_vec2f")
        vals = list(ud.getVec2fValues())
        self.assertAlmostEqual(vals[0].x, 1.0)
        self.assertAlmostEqual(vals[1].y, 4.0)

    def test_set_get_vec2f_data_blur(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_v2f2").asUserData()
        v0 = rdl2.Vec2f(1.0, 0.0)
        v1 = rdl2.Vec2f(0.0, 1.0)
        ud.setVec2fData("blur_vec2f", [v0], [v1])
        self.assertTrue(ud.hasVec2fData0())
        self.assertTrue(ud.hasVec2fData1())

    # --- Vec3f channel (single and dual timestep) --------------------------

    def test_has_vec3f_data_initially_false(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_v3f0").asUserData()
        self.assertFalse(ud.hasVec3fData())

    def test_set_get_vec3f_data_single(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_v3f1").asUserData()
        v = rdl2.Vec3f(1.0, 2.0, 3.0)
        ud.setVec3fData("my_vec3f", [v])
        self.assertTrue(ud.hasVec3fData())
        self.assertEqual(ud.getVec3fKey(), "my_vec3f")
        vals = list(ud.getVec3fValues())
        self.assertAlmostEqual(vals[0].z, 3.0)

    def test_set_get_vec3f_data_blur(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_v3f2").asUserData()
        v0 = rdl2.Vec3f(1.0, 0.0, 0.0)
        v1 = rdl2.Vec3f(0.0, 0.0, 1.0)
        ud.setVec3fData("blur_vec3f", [v0], [v1])
        self.assertTrue(ud.hasVec3fData0())
        self.assertTrue(ud.hasVec3fData1())

    # --- Mat4f channel (single and dual timestep) --------------------------

    def test_has_mat4f_data_initially_false(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_m4f0").asUserData()
        self.assertFalse(ud.hasMat4fData())

    def test_set_get_mat4f_data_single(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_m4f1").asUserData()
        m = rdl2.Mat4f()
        ud.setMat4fData("my_mat4f", [m])
        self.assertTrue(ud.hasMat4fData())
        self.assertTrue(ud.hasMat4fData0())
        self.assertFalse(ud.hasMat4fData1())
        self.assertEqual(ud.getMat4fKey(), "my_mat4f")

    def test_set_get_mat4f_data_blur(self):
        ud = self.ctx.createSceneObject("UserData", "/test/userdata/ud_m4f2").asUserData()
        m0 = rdl2.Mat4f()
        m1 = rdl2.Mat4f()
        ud.setMat4fData("blur_mat4f", [m0], [m1])
        self.assertTrue(ud.hasMat4fData0())
        self.assertTrue(ud.hasMat4fData1())


if __name__ == "__main__":
    unittest.main()
