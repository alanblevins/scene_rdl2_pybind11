#!/usr/bin/env python3
# Copyright (c) 2026 Alan Blevins
# SPDX-License-Identifier: MIT
"""Shared fixtures and helpers for the scene_rdl2 test suite."""

import os
import sys
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

DSO_PATH = os.environ.get('RDL2_DSO_PATH')
if not DSO_PATH:
    sys.exit("Error: RDL2_DSO_PATH is not set. Source MoonRay's setup.sh before running.")

_FIXTURE_DIR = os.path.join(_HERE, "fixtures")


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


class _WithDsos(unittest.TestCase):
    """Base class: provides a shared proxy-mode context with all DSOs loaded."""
    _ctx = None

    @classmethod
    def setUpClass(cls):
        if _WithDsos._ctx is None:
            _WithDsos._ctx = _make_ctx(load_dsos=True)
        cls.ctx = _WithDsos._ctx
