#!/usr/bin/env python3
# Copyright (c) 2026 Alan Blevins
# SPDX-License-Identifier: MIT
"""Tests for I/O: BinaryWriter/Reader, AsciiWriter/Reader, and file persistence."""

import os
import tempfile
import unittest

from .helpers import rdl2, _make_ctx, _FIXTURE_DIR


class TestBinaryWriter(unittest.TestCase):
    def setUp(self):
        self.ctx = _make_ctx()

    def test_construction(self):
        self.assertIsInstance(rdl2.BinaryWriter(self.ctx), rdl2.BinaryWriter)

    def test_set_transient_encoding(self):
        w = rdl2.BinaryWriter(self.ctx)
        w.setTransientEncoding(True)
        w.setTransientEncoding(False)

    def test_set_delta_encoding(self):
        w = rdl2.BinaryWriter(self.ctx)
        w.setDeltaEncoding(True)
        w.setDeltaEncoding(False)

    def test_set_skip_defaults(self):
        w = rdl2.BinaryWriter(self.ctx)
        w.setSkipDefaults(True)
        w.setSkipDefaults(False)

    def test_split_mode(self):
        w = rdl2.BinaryWriter(self.ctx)
        w.setSplitMode(100)
        w.clearSplitMode()

    def test_to_bytes_returns_tuple_of_bytes(self):
        manifest, payload = rdl2.BinaryWriter(self.ctx).toBytes()
        self.assertIsInstance(manifest, bytes)
        self.assertIsInstance(payload, bytes)

    def test_to_bytes_non_empty(self):
        manifest, payload = rdl2.BinaryWriter(self.ctx).toBytes()
        self.assertGreater(len(manifest) + len(payload), 0)

    def test_to_file(self):
        w = rdl2.BinaryWriter(self.ctx)
        with tempfile.NamedTemporaryFile(suffix=".rdlb", delete=False) as f:
            tmp_path = f.name
        try:
            w.toFile(tmp_path)
            self.assertTrue(os.path.exists(tmp_path))
            self.assertGreater(os.path.getsize(tmp_path), 0)
        finally:
            os.unlink(tmp_path)

    def test_show_returns_str(self):
        s = rdl2.BinaryWriter(self.ctx).show("", False)
        self.assertIsInstance(s, str)


class TestBinaryReader(unittest.TestCase):
    def setUp(self):
        self.ctx = _make_ctx()

    def test_construction(self):
        self.assertIsInstance(rdl2.BinaryReader(self.ctx), rdl2.BinaryReader)

    def test_set_warnings_as_errors(self):
        r = rdl2.BinaryReader(self.ctx)
        r.setWarningsAsErrors(True)
        r.setWarningsAsErrors(False)

    def test_show_manifest_returns_str(self):
        manifest, _ = rdl2.BinaryWriter(self.ctx).toBytes()
        s = rdl2.BinaryReader.showManifest(manifest)
        self.assertIsInstance(s, str)


class TestBinaryRoundTrip(unittest.TestCase):
    def test_round_trip_scene_variables(self):
        write_ctx = _make_ctx()
        write_ctx.getSceneVariables()["image_width"] = 5678

        writer = rdl2.BinaryWriter(write_ctx)
        writer.setSkipDefaults(False)
        manifest, payload = writer.toBytes()

        read_ctx = _make_ctx()
        rdl2.BinaryReader(read_ctx).fromBytes(manifest, payload)
        self.assertEqual(read_ctx.getSceneVariables()["image_width"], 5678)

    def test_round_trip_to_file(self):
        write_ctx = _make_ctx(load_dsos=True)
        write_ctx.createSceneObject("RenderOutput", "/test/bin/ro")

        writer = rdl2.BinaryWriter(write_ctx)
        with tempfile.NamedTemporaryFile(suffix=".rdlb", delete=False) as f:
            tmp_path = f.name
        try:
            writer.toFile(tmp_path)
            read_ctx = _make_ctx(load_dsos=True)
            rdl2.BinaryReader(read_ctx).fromFile(tmp_path)
            self.assertTrue(read_ctx.sceneObjectExists("/test/bin/ro"))
        finally:
            os.unlink(tmp_path)

    def test_round_trip_with_delta_encoding(self):
        write_ctx = _make_ctx()
        write_ctx.getSceneVariables()["image_height"] = 720

        writer = rdl2.BinaryWriter(write_ctx)
        writer.setDeltaEncoding(True)
        manifest, payload = writer.toBytes()

        read_ctx = _make_ctx()
        rdl2.BinaryReader(read_ctx).fromBytes(manifest, payload)
        self.assertEqual(read_ctx.getSceneVariables()["image_height"], 720)


class TestRdlaFilePersistence(unittest.TestCase):
    """Writes a populated scene to tests/fixtures/test_scene.rdla, then reads it back."""

    @classmethod
    def setUpClass(cls):
        os.makedirs(_FIXTURE_DIR, exist_ok=True)
        cls.rdla_path = os.path.join(_FIXTURE_DIR, "test_scene.rdla")

        write_ctx = _make_ctx(load_dsos=True)
        sv = write_ctx.getSceneVariables()
        sv["image_width"]  = 1280
        sv["image_height"] = 720
        sv["frame"]        = 12.5

        write_ctx.createSceneObject("RenderOutput", "/fixtures/ro")

        md = write_ctx.createSceneObject("Metadata", "/fixtures/meta").asMetadata()
        md.setAttributes(["title", "author"],
                         ["string", "string"],
                         ["test scene", "rdl2py"])

        rdl2.AsciiWriter(write_ctx).toFile(cls.rdla_path)

        cls.ctx_sv = _make_ctx()
        rdl2.AsciiReader(cls.ctx_sv).fromFile(cls.rdla_path)

        cls.ctx_full = _make_ctx(load_dsos=True)
        rdl2.AsciiReader(cls.ctx_full).fromFile(cls.rdla_path)

    # --- file-level checks --------------------------------------------------

    def test_file_exists(self):
        self.assertTrue(os.path.exists(self.rdla_path))

    def test_file_has_rdla_extension(self):
        self.assertTrue(self.rdla_path.endswith(".rdla"))

    def test_file_is_non_empty(self):
        self.assertGreater(os.path.getsize(self.rdla_path), 0)

    def test_file_is_text(self):
        with open(self.rdla_path) as f:
            text = f.read()
        self.assertIsInstance(text, str)

    def test_file_contains_scene_variables_marker(self):
        with open(self.rdla_path) as f:
            text = f.read()
        self.assertIn("SceneVariables", text)

    def test_file_contains_render_output_marker(self):
        with open(self.rdla_path) as f:
            text = f.read()
        self.assertIn("RenderOutput", text)

    # --- SceneVariables read-back -------------------------------------------

    def test_read_image_width(self):
        self.assertEqual(self.ctx_sv.getSceneVariables()["image_width"], 1280)

    def test_read_image_height(self):
        self.assertEqual(self.ctx_sv.getSceneVariables()["image_height"], 720)

    def test_read_frame(self):
        self.assertAlmostEqual(self.ctx_sv.getSceneVariables()["frame"], 12.5)

    # --- object existence read-back -----------------------------------------

    def test_render_output_exists(self):
        self.assertTrue(self.ctx_full.sceneObjectExists("/fixtures/ro"))

    def test_metadata_exists(self):
        self.assertTrue(self.ctx_full.sceneObjectExists("/fixtures/meta"))

    # --- object content read-back -------------------------------------------

    def test_metadata_attribute_names(self):
        md = self.ctx_full.getSceneObject("/fixtures/meta").asMetadata()
        self.assertIsNotNone(md)
        names = md.getAttributeNames()
        self.assertIn("title", names)
        self.assertIn("author", names)

    def test_metadata_attribute_values(self):
        md = self.ctx_full.getSceneObject("/fixtures/meta").asMetadata()
        values = md.getAttributeValues()
        self.assertIn("test scene", values)
        self.assertIn("rdl2py", values)

    def test_metadata_attribute_types(self):
        md = self.ctx_full.getSceneObject("/fixtures/meta").asMetadata()
        types = md.getAttributeTypes()
        self.assertEqual(types.count("string"), 2)

    # --- independent second read --------------------------------------------

    def test_read_into_fresh_context_agrees(self):
        ctx2 = _make_ctx()
        rdl2.AsciiReader(ctx2).fromFile(self.rdla_path)
        self.assertEqual(ctx2.getSceneVariables()["image_width"], 1280)
        self.assertEqual(ctx2.getSceneVariables()["image_height"], 720)


class TestRdlbFilePersistence(unittest.TestCase):
    """Writes a populated scene to tests/fixtures/test_scene.rdlb, then reads it back."""

    @classmethod
    def setUpClass(cls):
        os.makedirs(_FIXTURE_DIR, exist_ok=True)
        cls.rdlb_path = os.path.join(_FIXTURE_DIR, "test_scene.rdlb")

        write_ctx = _make_ctx(load_dsos=True)
        sv = write_ctx.getSceneVariables()
        sv["image_width"]  = 1280
        sv["image_height"] = 720
        sv["frame"]        = 12.5

        write_ctx.createSceneObject("RenderOutput", "/fixtures/ro")

        ud = write_ctx.createSceneObject("UserData", "/fixtures/ud").asUserData()
        ud.setRate(rdl2.UserData.Rate.VERTEX)
        ud.setFloatData("Cd", [0.1, 0.2, 0.3, 0.4])

        writer = rdl2.BinaryWriter(write_ctx)
        writer.setSkipDefaults(False)
        writer.toFile(cls.rdlb_path)

        cls.ctx_sv = _make_ctx()
        rdl2.BinaryReader(cls.ctx_sv).fromFile(cls.rdlb_path)

        cls.ctx_full = _make_ctx(load_dsos=True)
        rdl2.BinaryReader(cls.ctx_full).fromFile(cls.rdlb_path)

    # --- file-level checks --------------------------------------------------

    def test_file_exists(self):
        self.assertTrue(os.path.exists(self.rdlb_path))

    def test_file_has_rdlb_extension(self):
        self.assertTrue(self.rdlb_path.endswith(".rdlb"))

    def test_file_is_non_empty(self):
        self.assertGreater(os.path.getsize(self.rdlb_path), 0)

    # --- SceneVariables read-back -------------------------------------------

    def test_read_image_width(self):
        self.assertEqual(self.ctx_sv.getSceneVariables()["image_width"], 1280)

    def test_read_image_height(self):
        self.assertEqual(self.ctx_sv.getSceneVariables()["image_height"], 720)

    def test_read_frame(self):
        self.assertAlmostEqual(self.ctx_sv.getSceneVariables()["frame"], 12.5)

    # --- object existence read-back -----------------------------------------

    def test_render_output_exists(self):
        self.assertTrue(self.ctx_full.sceneObjectExists("/fixtures/ro"))

    def test_user_data_exists(self):
        self.assertTrue(self.ctx_full.sceneObjectExists("/fixtures/ud"))

    # --- UserData content read-back -----------------------------------------

    def test_user_data_float_key(self):
        ud = self.ctx_full.getSceneObject("/fixtures/ud").asUserData()
        self.assertIsNotNone(ud)
        self.assertEqual(ud.getFloatKey(), "Cd")

    def test_user_data_float_values(self):
        ud = self.ctx_full.getSceneObject("/fixtures/ud").asUserData()
        vals = list(ud.getFloatValues())
        self.assertEqual(len(vals), 4)
        self.assertAlmostEqual(vals[0], 0.1, places=5)
        self.assertAlmostEqual(vals[1], 0.2, places=5)
        self.assertAlmostEqual(vals[2], 0.3, places=5)
        self.assertAlmostEqual(vals[3], 0.4, places=5)

    def test_user_data_rate(self):
        ud = self.ctx_full.getSceneObject("/fixtures/ud").asUserData()
        self.assertEqual(ud.getRate(), rdl2.UserData.Rate.VERTEX)

    # --- manifest inspection ------------------------------------------------

    def test_show_manifest_returns_str(self):
        manifest, _ = rdl2.BinaryWriter(self.ctx_sv).toBytes()
        result = rdl2.BinaryReader.showManifest(manifest)
        self.assertIsInstance(result, str)

    # --- independent second read --------------------------------------------

    def test_read_into_fresh_context_agrees(self):
        ctx2 = _make_ctx()
        rdl2.BinaryReader(ctx2).fromFile(self.rdlb_path)
        self.assertEqual(ctx2.getSceneVariables()["image_width"], 1280)
        self.assertEqual(ctx2.getSceneVariables()["image_height"], 720)


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
        writer.setSkipDefaults(False)

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
        write_ctx = _make_ctx()
        write_ctx.getSceneVariables()["image_width"] = 1234

        writer = rdl2.AsciiWriter(write_ctx)
        writer.setSkipDefaults(False)
        rdla = writer.toString()
        self.assertIn("SceneVariables", rdla)

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
