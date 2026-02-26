#!/usr/bin/env python3
# Copyright (c) 2026 Alan Blevins
# SPDX-License-Identifier: MIT
"""Tests for core scene types: SceneContext, SceneClass, Attribute, SceneObject,
SceneVariables, Node/Camera/Geometry, and the type hierarchy."""

import unittest

from .helpers import rdl2, DSO_PATH, _make_ctx, _first_class_name, _WithDsos


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


class TestSceneClass(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.sc = cls.ctx.getSceneClass("BoxGeometry")

    def test_get_name(self):
        self.assertEqual(self.sc.getName(), "BoxGeometry")

    def test_repr(self):
        self.assertIn("BoxGeometry", repr(self.sc))

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
        self.sv[attr_name] = value
        return self.sv[attr_name]

    def test_get_set_bool(self):
        orig = self.sv["slerp_xforms"]
        result = self._sv_set_get("slerp_xforms", True)
        self.assertIs(result, True)
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
        val = self.sv["frame", rdl2.TIMESTEP_BEGIN]
        self.assertIsInstance(val, float)

    def test_get_returns_correct_types(self):
        self.assertIsInstance(self.sv["image_width"], int)
        self.assertIsInstance(self.sv["frame"], float)
        self.assertIsInstance(self.sv["slerp_xforms"], bool)
        self.assertIsInstance(self.sv["output_file"], str)
        self.assertIsInstance(self.sv["fatal_color"], rdl2.Rgb)

    def test_has_changed(self):
        self.sv["frame"] = 999.0
        self.assertTrue(self.sv.hasChanged("frame"))

    def test_is_dirty(self):
        self.sv["frame"] = 1.0
        self.assertTrue(self.sv.isDirty())

    def test_request_update(self):
        self.sv.requestUpdate()  # should not raise

    def test_is_default_after_reset(self):
        self.sv["fps"] = 30.0
        self.sv.resetToDefault("fps")
        self.assertTrue(self.sv.isDefault("fps"))

    def test_reset_all_to_default(self):
        self.sv["image_width"] = 9999
        self.sv.resetAllToDefault()
        self.assertTrue(self.sv.isDefault("image_width"))

    def test_is_default_and_unbound(self):
        self.sv.resetToDefault("output_file")
        result = self.sv.isDefaultAndUnbound("output_file")
        self.assertIsInstance(result, bool)

    def test_copy_all(self):
        src = self.ctx.createSceneObject("BoxGeometry", "/test/so/box_src")
        dst = self.ctx.createSceneObject("BoxGeometry", "/test/so/box_dst")
        dst.copyAll(src)

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
        self.sv["image_width"] = 512
        self.assertEqual(self.sv["image_width"], 512)
        self.sv["image_width"] = orig

    def test_getitem_timestep_tuple_key(self):
        val = self.sv["frame", rdl2.TIMESTEP_BEGIN]
        self.assertIsInstance(val, float)

    def test_setitem_timestep_tuple_key(self):
        orig = self.sv["frame", rdl2.TIMESTEP_BEGIN]
        self.sv["frame", rdl2.TIMESTEP_BEGIN] = 77.0
        self.assertAlmostEqual(self.sv["frame"], 77.0)
        self.sv["frame"] = orig

    def test_getitem_bad_key_type_raises(self):
        with self.assertRaises((KeyError, TypeError)):
            _ = self.sv[42]

    def test_getitem_bad_tuple_length_raises(self):
        with self.assertRaises((KeyError, TypeError)):
            _ = self.sv["frame", rdl2.TIMESTEP_BEGIN, "extra"]

    def test_getitem_unknown_attr_raises(self):
        with self.assertRaises(Exception):
            _ = self.sv["this_attr_does_not_exist"]


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
        self.assertTrue(cam is None or cam.isCamera())

    def test_get_layer_is_none_or_layer(self):
        layer = self.sv.getLayer()
        self.assertTrue(layer is None or isinstance(layer, rdl2.Layer))


class TestNode(_WithDsos):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cam_name = _first_class_name(cls.ctx, rdl2.INTERFACE_CAMERA)
        geo_name = _first_class_name(cls.ctx, rdl2.INTERFACE_GEOMETRY)
        cls.assertIsNotNone(cls, cam_name, "No Camera DSO found")
        cls.assertIsNotNone(cls, geo_name, "No Geometry DSO found")
        cls.cam = cls.ctx.createSceneObject(cam_name, "/test/node/cam").asCamera()
        cls.geo = cls.ctx.createSceneObject(geo_name, "/test/node/geo").asGeometry()

    def test_camera_is_camera(self):
        self.assertIsInstance(self.cam, rdl2.Camera)

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
        self.cam.setNodeXform(xform)
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
        self.cam.setNear(0.1)
        self.cam.setFar(5000.0)
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


if __name__ == "__main__":
    unittest.main()
