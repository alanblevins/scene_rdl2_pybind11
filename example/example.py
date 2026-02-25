#!/usr/bin/env python3
# Copyright (c) 2026 Alan Blevins
# SPDX-License-Identifier: MIT
"""
example.py — a complete, render-ready MoonRay scene built with scene_rdl2.

The scene contains one of each supported primitive geometry type lined up in a
row along the X axis.  Each primitive gets its own coloured BaseMaterial.  A
single SphereLight key light illuminates the row and a PerspectiveCamera frames
the whole composition.  Everything is tied together with a Layer and exported
as an ASCII .rdla file.

Render the result:
    moonray_gui -scene scene.rdla
or:
    moonray -scene scene.rdla -out beauty.exr
"""

import math
import os
import sys

# ── path bootstrap ─────────────────────────────────────────────────────────────
_HERE = os.path.dirname(os.path.abspath(__file__))
_BUILD = os.path.join(_HERE, "build")
if os.path.isdir(_BUILD):
    sys.path.insert(0, _BUILD)

import scene_rdl2 as rdl2

DSO_PATH = os.environ.get('RDL2_DSO_PATH')
if not DSO_PATH:
    sys.exit("Error: RDL2_DSO_PATH is not set. Source MoonRay's setup.sh before running.")
OUTPUT   = "scene.rdla"


# ── math helpers ───────────────────────────────────────────────────────────────

def translation(tx, ty, tz):
    """Return a Mat4d representing a pure translation."""
    return rdl2.Mat4d(
        rdl2.Vec4d(1,  0,  0,  0),
        rdl2.Vec4d(0,  1,  0,  0),
        rdl2.Vec4d(0,  0,  1,  0),
        rdl2.Vec4d(tx, ty, tz, 1),
    )


def lookat(pos, target, world_up=(0.0, 1.0, 0.0)):
    """
    Return a camera-to-world Mat4d suitable for node_xform.

    Builds a right-handed, Y-up camera basis: +X is right, +Y is up, +Z is
    behind the camera (the camera looks down its own −Z axis into the scene).
    """
    def sub(a, b):   return (a[0]-b[0], a[1]-b[1], a[2]-b[2])
    def dot(a, b):   return a[0]*b[0]  + a[1]*b[1]  + a[2]*b[2]
    def cross(a, b): return (a[1]*b[2]-a[2]*b[1],
                             a[2]*b[0]-a[0]*b[2],
                             a[0]*b[1]-a[1]*b[0])
    def norm(a):
        l = math.sqrt(dot(a, a))
        return (a[0]/l, a[1]/l, a[2]/l)

    back = norm(sub(pos, target))         # camera +Z (points behind camera)
    rgt  = norm(cross(world_up, back))    # camera +X (right)
    up   = cross(back, rgt)               # camera +Y (already normalised)

    return rdl2.Mat4d(
        rdl2.Vec4d(rgt[0], rgt[1], rgt[2], 0),
        rdl2.Vec4d(up[0],  up[1],  up[2],  0),
        rdl2.Vec4d(back[0],back[1],back[2],0),
        rdl2.Vec4d(pos[0], pos[1], pos[2], 1),
    )


# ── scene construction ─────────────────────────────────────────────────────────

def build_scene(ctx):
    """Populate *ctx* with a render-ready scene and return the context."""

    # ------------------------------------------------------------------
    # Materials — one coloured BaseMaterial per geometry
    # ------------------------------------------------------------------
    palette = [
        ("red",    rdl2.Rgb(0.85, 0.12, 0.12)),
        ("orange", rdl2.Rgb(0.90, 0.50, 0.08)),
        ("green",  rdl2.Rgb(0.18, 0.72, 0.18)),
        ("blue",   rdl2.Rgb(0.10, 0.25, 0.90)),
        ("purple", rdl2.Rgb(0.60, 0.10, 0.85)),
    ]
    materials = {}
    for name, color in palette:
        mat = ctx.createSceneObject("BaseMaterial", f"/mat/{name}").asMaterial()
        mat["diffuse_color"]      = color
        mat["specular_color"]     = rdl2.Rgb(1.0, 1.0, 1.0)
        mat["specular_factor"]    = 0.5
        mat["specular_roughness"] = 0.25
        materials[name] = mat

    # ------------------------------------------------------------------
    # Geometry — five primitive types, spaced 3 units apart along X
    # ------------------------------------------------------------------

    # SphereGeometry ── a unit sphere (red)
    sphere = ctx.createSceneObject("SphereGeometry", "/geo/sphere").asGeometry()
    sphere["radius"] = 1.0
    sphere.setNodeXform(translation(-6, 1, 0))   # raised so it sits on y = 0

    # BoxGeometry ── a 2×2×2 cube (orange)
    box = ctx.createSceneObject("BoxGeometry", "/geo/box").asGeometry()
    box["size"] = rdl2.Vec3f(2.0, 2.0, 2.0)
    box.setNodeXform(translation(-3, 1, 0))

    # RdlMeshGeometry ── a flat quad panel, 2 units wide × 2 units tall (green)
    quad = ctx.createSceneObject("RdlMeshGeometry", "/geo/quad").asGeometry()
    quad["vertex_list_0"] = [
        rdl2.Vec3f(-1.0, 0.0, 0.0),
        rdl2.Vec3f( 1.0, 0.0, 0.0),
        rdl2.Vec3f( 1.0, 2.0, 0.0),
        rdl2.Vec3f(-1.0, 2.0, 0.0),
    ]
    quad["face_vertex_count"] = [4]
    quad["vertices_by_index"] = [0, 1, 2, 3]
    quad["is_subd"]           = False     # plain polygon, not a subdivision surface
    quad.setNodeXform(translation(0, 0, 0))

    # RdlCurveGeometry ── three cubic Bézier strands (blue)
    # curve_type 1 = bezier_cubic; each strand needs 4 control points.
    curves = ctx.createSceneObject("RdlCurveGeometry", "/geo/curves").asGeometry()
    curves["curves_vertex_count"] = [4, 4, 4]
    curves["vertex_list_0"] = [
        # left strand — arcs gently inward
        rdl2.Vec3f(-0.6, 0.0, 0.0), rdl2.Vec3f(-0.4, 0.65, 0.0),
        rdl2.Vec3f(-0.4, 1.35, 0.0), rdl2.Vec3f(-0.6, 2.0, 0.0),
        # centre strand — straight
        rdl2.Vec3f( 0.0, 0.0, 0.0), rdl2.Vec3f( 0.0, 0.65, 0.0),
        rdl2.Vec3f( 0.0, 1.35, 0.0), rdl2.Vec3f( 0.0, 2.0, 0.0),
        # right strand — mirror of left
        rdl2.Vec3f( 0.6, 0.0, 0.0), rdl2.Vec3f( 0.4, 0.65, 0.0),
        rdl2.Vec3f( 0.4, 1.35, 0.0), rdl2.Vec3f( 0.6, 2.0, 0.0),
    ]
    curves["radius_list"] = [0.05] * 12   # constant width along all strands
    curves.setNodeXform(translation(3, 0, 0))

    # RdlPointGeometry ── a small cluster of point-spheres (purple)
    points = ctx.createSceneObject("RdlPointGeometry", "/geo/points").asGeometry()
    points["vertex_list_0"] = [
        rdl2.Vec3f( 0.0,  0.3,  0.0),
        rdl2.Vec3f(-0.55, 0.3,  0.0),
        rdl2.Vec3f( 0.55, 0.3,  0.0),
        rdl2.Vec3f( 0.0,  1.1,  0.0),
        rdl2.Vec3f(-0.35, 0.9,  0.35),
        rdl2.Vec3f( 0.35, 0.9, -0.35),
        rdl2.Vec3f( 0.0,  1.75, 0.0),
    ]
    points["radius_list"] = [0.30, 0.28, 0.28, 0.25, 0.22, 0.22, 0.18]
    points.setNodeXform(translation(6, 0, 0))

    geos      = [sphere, box,      quad,    curves, points ]
    mat_names = ["red",  "orange", "green", "blue", "purple"]

    # ------------------------------------------------------------------
    # GeometrySet — collect all primitives into one set
    # ------------------------------------------------------------------
    geoset = ctx.createSceneObject("GeometrySet", "/geoset/main")
    for geo in geos:
        geoset.add(geo)

    # ------------------------------------------------------------------
    # Light — one SphereLight as the key light
    # ------------------------------------------------------------------
    light = ctx.createSceneObject("SphereLight", "/light/key")
    light["color"]     = rdl2.Rgb(1.0, 0.96, 0.88)   # warm white
    light["intensity"] = 20.0
    light["radius"]    = 2.0
    # Set both motion-blur timesteps to the same matrix (static light).
    _light_xf = translation(-5, 12, 8)
    light["node_xform"] = _light_xf
    light["node_xform", rdl2.TIMESTEP_END] = _light_xf

    # ------------------------------------------------------------------
    # LightSet — referenced by the Layer so geometry knows what to sample
    # ------------------------------------------------------------------
    lset = ctx.createSceneObject("LightSet", "/lightset/main").asLightSet()
    lset.add(light.asLight())

    # ------------------------------------------------------------------
    # Layer — binds each geometry to its material and the shared LightSet
    # ------------------------------------------------------------------
    layer = ctx.createSceneObject("Layer", "/layer/main")
    for geo, mat_name in zip(geos, mat_names):
        layer.assign(geo, "", materials[mat_name], lset)

    # ------------------------------------------------------------------
    # Camera — PerspectiveCamera framing the full row from slightly above
    # ------------------------------------------------------------------
    cam = ctx.createSceneObject("PerspectiveCamera", "/cam/main").asCamera()
    # Set both motion-blur timesteps to the same value (no rack-focus blur).
    cam["focal"]               = 24.0   # 24 mm ≈ 74° hFoV on a 36 mm gate
    cam["focal", rdl2.TIMESTEP_END] = 24.0
    cam["film_width_aperture"] = 36.0   # full-frame width
    cam["near"]                = 0.1
    cam["far"]                 = 1000.0
    cam.setNodeXform(lookat(
        pos    = (0.0, 5.0, 20.0),   # back and slightly above the row
        target = (0.0, 1.0,  0.0),   # look at the mid-height of the row
    ))

    # ------------------------------------------------------------------
    # RenderOutput — beauty pass to scene.exr
    # ------------------------------------------------------------------
    ro = ctx.createSceneObject("RenderOutput", "/output/beauty")
    ro["file_name"]    = "scene.exr"
    ro["channel_name"] = "beauty"

    # ------------------------------------------------------------------
    # SceneVariables — global render settings
    # ------------------------------------------------------------------
    sv = ctx.getSceneVariables()
    sv["camera"]        = cam
    sv["layer"]         = layer
    sv["image_width"]   = 1280
    sv["image_height"]  = 720
    sv["pixel_samples"] = 4    # quick render; increase for final quality


# ── main ───────────────────────────────────────────────────────────────────────

def main():
    ctx = rdl2.SceneContext()
    ctx.setProxyModeEnabled(True)   # safe on macOS without a full render daemon
    ctx.setDsoPath(DSO_PATH)
    ctx.loadAllSceneClasses()

    build_scene(ctx)

    writer = rdl2.AsciiWriter(ctx)
    writer.setSkipDefaults(True)    # keep the file concise
    writer.toFile(OUTPUT)

    print(f"Scene written to  {OUTPUT}")
    print(f"Render with:      moonray_gui -scene {OUTPUT}")
    print(f"            or:   moonray -scene {OUTPUT} -out beauty.exr")


if __name__ == "__main__":
    main()
