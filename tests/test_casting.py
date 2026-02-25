#!/usr/bin/env python3
# Copyright (c) 2026 Alan Blevins
# SPDX-License-Identifier: MIT
"""Tests for constructor-based casting: Camera(obj), Geometry(obj), etc."""

import unittest

from .helpers import rdl2, _WithDsos, _first_class_name


class TestCastConstructor(_WithDsos):
    """Test constructor-based casting: Camera(scene_obj), Geometry(scene_obj), etc."""

    # -----------------------------------------------------------------------
    # Built-in (non-DSO) types — always available
    # -----------------------------------------------------------------------

    def test_render_output_constructor(self):
        obj = self.ctx.createSceneObject("RenderOutput", "/cast/ro")
        ro = rdl2.RenderOutput(obj)
        self.assertIsInstance(ro, rdl2.RenderOutput)

    def test_user_data_constructor(self):
        obj = self.ctx.createSceneObject("UserData", "/cast/ud")
        ud = rdl2.UserData(obj)
        self.assertIsInstance(ud, rdl2.UserData)

    def test_metadata_constructor(self):
        obj = self.ctx.createSceneObject("Metadata", "/cast/meta")
        md = rdl2.Metadata(obj)
        self.assertIsInstance(md, rdl2.Metadata)

    def test_layer_constructor(self):
        obj = self.ctx.createSceneObject("Layer", "/cast/layer")
        layer = rdl2.Layer(obj)
        self.assertIsInstance(layer, rdl2.Layer)

    def test_geometry_set_constructor(self):
        obj = self.ctx.createSceneObject("GeometrySet", "/cast/gset")
        gs = rdl2.GeometrySet(obj)
        self.assertIsInstance(gs, rdl2.GeometrySet)

    def test_light_set_constructor(self):
        obj = self.ctx.createSceneObject("LightSet", "/cast/lset")
        ls = rdl2.LightSet(obj)
        self.assertIsInstance(ls, rdl2.LightSet)

    def test_trace_set_constructor(self):
        obj = self.ctx.createSceneObject("TraceSet", "/cast/tset")
        ts = rdl2.TraceSet(obj)
        self.assertIsInstance(ts, rdl2.TraceSet)

    # -----------------------------------------------------------------------
    # DSO-loaded types — use first available class of each interface
    # -----------------------------------------------------------------------

    def test_camera_constructor(self):
        class_name = _first_class_name(self.ctx, rdl2.INTERFACE_CAMERA)
        if class_name is None:
            self.skipTest("No Camera DSO available")
        obj = self.ctx.createSceneObject(class_name, "/cast/cam")
        cam = rdl2.Camera(obj)
        self.assertIsInstance(cam, rdl2.Camera)

    def test_node_from_camera(self):
        """Camera IS-A Node, so Node(cam_obj) must succeed."""
        class_name = _first_class_name(self.ctx, rdl2.INTERFACE_CAMERA)
        if class_name is None:
            self.skipTest("No Camera DSO available")
        obj = self.ctx.createSceneObject(class_name, "/cast/cam_as_node")
        node = rdl2.Node(obj)
        self.assertIsInstance(node, rdl2.Node)

    def test_geometry_constructor(self):
        class_name = _first_class_name(self.ctx, rdl2.INTERFACE_GEOMETRY)
        if class_name is None:
            self.skipTest("No Geometry DSO available")
        obj = self.ctx.createSceneObject(class_name, "/cast/geo")
        geo = rdl2.Geometry(obj)
        self.assertIsInstance(geo, rdl2.Geometry)

    def test_light_constructor(self):
        class_name = _first_class_name(self.ctx, rdl2.INTERFACE_LIGHT)
        if class_name is None:
            self.skipTest("No Light DSO available")
        obj = self.ctx.createSceneObject(class_name, "/cast/light")
        light = rdl2.Light(obj)
        self.assertIsInstance(light, rdl2.Light)

    def test_material_constructor(self):
        class_name = _first_class_name(self.ctx, rdl2.INTERFACE_MATERIAL)
        if class_name is None:
            self.skipTest("No Material DSO available")
        obj = self.ctx.createSceneObject(class_name, "/cast/mat")
        mat = rdl2.Material(obj)
        self.assertIsInstance(mat, rdl2.Material)

    def test_shader_from_material(self):
        """Material IS-A Shader, so Shader(mat_obj) must succeed."""
        class_name = _first_class_name(self.ctx, rdl2.INTERFACE_MATERIAL)
        if class_name is None:
            self.skipTest("No Material DSO available")
        obj = self.ctx.createSceneObject(class_name, "/cast/mat_as_shader")
        shader = rdl2.Shader(obj)
        self.assertIsInstance(shader, rdl2.Shader)

    def test_root_shader_from_material(self):
        """Material IS-A RootShader, so RootShader(mat_obj) must succeed."""
        class_name = _first_class_name(self.ctx, rdl2.INTERFACE_MATERIAL)
        if class_name is None:
            self.skipTest("No Material DSO available")
        obj = self.ctx.createSceneObject(class_name, "/cast/mat_as_rootshader")
        rs = rdl2.RootShader(obj)
        self.assertIsInstance(rs, rdl2.RootShader)

    # -----------------------------------------------------------------------
    # Type mismatch — must raise TypeError
    # -----------------------------------------------------------------------

    def test_wrong_type_raises_type_error(self):
        """Casting a UserData as Camera must raise TypeError."""
        obj = self.ctx.createSceneObject("UserData", "/cast/wrong_type_ud")
        with self.assertRaises(TypeError):
            rdl2.Camera(obj)

    def test_render_output_wrong_type_raises(self):
        """Casting a Layer as RenderOutput must raise TypeError."""
        obj = self.ctx.createSceneObject("Layer", "/cast/wrong_type_layer")
        with self.assertRaises(TypeError):
            rdl2.RenderOutput(obj)

    # -----------------------------------------------------------------------
    # Cast preserves callable methods
    # -----------------------------------------------------------------------

    def test_cast_render_output_methods_work(self):
        """Casting to RenderOutput gives access to RenderOutput-specific methods."""
        obj = self.ctx.createSceneObject("RenderOutput", "/cast/ro_methods")
        ro = rdl2.RenderOutput(obj)
        self.assertIsInstance(ro.getActive(), bool)

    def test_cast_user_data_methods_work(self):
        """Casting to UserData gives access to UserData-specific methods."""
        obj = self.ctx.createSceneObject("UserData", "/cast/ud_methods")
        ud = rdl2.UserData(obj)
        self.assertFalse(ud.hasBoolData())


if __name__ == "__main__":
    unittest.main()
