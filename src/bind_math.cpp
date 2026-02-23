// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for scene_rdl2 math types:
//   Rgb, Rgba, Vec2f, Vec2d, Vec3f, Vec3d, Vec4f, Vec4d, Mat4f, Mat4d

#include "bindings.h"

void bind_math(py::module_& m)
{
    // Rgb (Col3f): r, g, b
    py::class_<rdl2::Rgb>(m, "Rgb")
        .def(py::init<>())
        .def(py::init<float>(), py::arg("v"))
        .def(py::init<float, float, float>(), py::arg("r"), py::arg("g"), py::arg("b"))
        .def_readwrite("r", &rdl2::Rgb::r)
        .def_readwrite("g", &rdl2::Rgb::g)
        .def_readwrite("b", &rdl2::Rgb::b)
        .def("__repr__", [](const rdl2::Rgb& c) {
            return "Rgb(" + std::to_string(c.r) + ", " + std::to_string(c.g) + ", " + std::to_string(c.b) + ")";
        })
        .def("__eq__", [](const rdl2::Rgb& a, const rdl2::Rgb& b) {
            return a.r == b.r && a.g == b.g && a.b == b.b;
        })
        .def("__iter__", [](const rdl2::Rgb& c) {
            return py::iter(py::make_tuple(c.r, c.g, c.b));
        });

    // Rgba (Col4f): r, g, b, a
    py::class_<rdl2::Rgba>(m, "Rgba")
        .def(py::init<>())
        .def(py::init<float>(), py::arg("v"))
        .def(py::init<float, float, float, float>(), py::arg("r"), py::arg("g"), py::arg("b"), py::arg("a"))
        .def_readwrite("r", &rdl2::Rgba::r)
        .def_readwrite("g", &rdl2::Rgba::g)
        .def_readwrite("b", &rdl2::Rgba::b)
        .def_readwrite("a", &rdl2::Rgba::a)
        .def("__repr__", [](const rdl2::Rgba& c) {
            return "Rgba(" + std::to_string(c.r) + ", " + std::to_string(c.g) + ", " + std::to_string(c.b) + ", " + std::to_string(c.a) + ")";
        })
        .def("__eq__", [](const rdl2::Rgba& a, const rdl2::Rgba& b) {
            return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
        });

    // Vec2f
    py::class_<rdl2::Vec2f>(m, "Vec2f")
        .def(py::init<>())
        .def(py::init<float>(), py::arg("v"))
        .def(py::init<float, float>(), py::arg("x"), py::arg("y"))
        .def_readwrite("x", &rdl2::Vec2f::x)
        .def_readwrite("y", &rdl2::Vec2f::y)
        .def("__repr__", [](const rdl2::Vec2f& v) {
            return "Vec2f(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        })
        .def("__eq__", [](const rdl2::Vec2f& a, const rdl2::Vec2f& b) {
            return a.x == b.x && a.y == b.y;
        });

    // Vec2d
    py::class_<rdl2::Vec2d>(m, "Vec2d")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("v"))
        .def(py::init<double, double>(), py::arg("x"), py::arg("y"))
        .def_readwrite("x", &rdl2::Vec2d::x)
        .def_readwrite("y", &rdl2::Vec2d::y)
        .def("__repr__", [](const rdl2::Vec2d& v) {
            return "Vec2d(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        });

    // Vec3f
    py::class_<rdl2::Vec3f>(m, "Vec3f")
        .def(py::init<>())
        .def(py::init<float>(), py::arg("v"))
        .def(py::init<float, float, float>(), py::arg("x"), py::arg("y"), py::arg("z"))
        .def_readwrite("x", &rdl2::Vec3f::x)
        .def_readwrite("y", &rdl2::Vec3f::y)
        .def_readwrite("z", &rdl2::Vec3f::z)
        .def("__repr__", [](const rdl2::Vec3f& v) {
            return "Vec3f(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
        })
        .def("__eq__", [](const rdl2::Vec3f& a, const rdl2::Vec3f& b) {
            return a.x == b.x && a.y == b.y && a.z == b.z;
        })
        .def("__iter__", [](const rdl2::Vec3f& v) {
            return py::iter(py::make_tuple(v.x, v.y, v.z));
        });

    // Vec3d
    py::class_<rdl2::Vec3d>(m, "Vec3d")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("v"))
        .def(py::init<double, double, double>(), py::arg("x"), py::arg("y"), py::arg("z"))
        .def_readwrite("x", &rdl2::Vec3d::x)
        .def_readwrite("y", &rdl2::Vec3d::y)
        .def_readwrite("z", &rdl2::Vec3d::z)
        .def("__repr__", [](const rdl2::Vec3d& v) {
            return "Vec3d(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
        });

    // Vec4f
    py::class_<rdl2::Vec4f>(m, "Vec4f")
        .def(py::init<>())
        .def(py::init<float>(), py::arg("v"))
        .def(py::init<float, float, float, float>(), py::arg("x"), py::arg("y"), py::arg("z"), py::arg("w"))
        .def_readwrite("x", &rdl2::Vec4f::x)
        .def_readwrite("y", &rdl2::Vec4f::y)
        .def_readwrite("z", &rdl2::Vec4f::z)
        .def_readwrite("w", &rdl2::Vec4f::w)
        .def("__repr__", [](const rdl2::Vec4f& v) {
            return "Vec4f(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")";
        });

    // Vec4d
    py::class_<rdl2::Vec4d>(m, "Vec4d")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("v"))
        .def(py::init<double, double, double, double>(), py::arg("x"), py::arg("y"), py::arg("z"), py::arg("w"))
        .def_readwrite("x", &rdl2::Vec4d::x)
        .def_readwrite("y", &rdl2::Vec4d::y)
        .def_readwrite("z", &rdl2::Vec4d::z)
        .def_readwrite("w", &rdl2::Vec4d::w)
        .def("__repr__", [](const rdl2::Vec4d& v) {
            return "Vec4d(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")";
        });

    // Mat4f (vx, vy, vz, vw are Vec4f rows)
    py::class_<rdl2::Mat4f>(m, "Mat4f")
        .def(py::init<>())
        .def(py::init<rdl2::Vec4f, rdl2::Vec4f, rdl2::Vec4f, rdl2::Vec4f>(),
             py::arg("vx"), py::arg("vy"), py::arg("vz"), py::arg("vw"))
        .def_readwrite("vx", &rdl2::Mat4f::vx)
        .def_readwrite("vy", &rdl2::Mat4f::vy)
        .def_readwrite("vz", &rdl2::Mat4f::vz)
        .def_readwrite("vw", &rdl2::Mat4f::vw)
        .def("__repr__", [](const rdl2::Mat4f& m) {
            return "Mat4f([" + std::to_string(m.vx.x) + ", " + std::to_string(m.vx.y) +
                   ", " + std::to_string(m.vx.z) + ", " + std::to_string(m.vx.w) + "], ...)";
        });

    // Mat4d (vx, vy, vz, vw are Vec4d rows)
    py::class_<rdl2::Mat4d>(m, "Mat4d")
        .def(py::init<>())
        .def(py::init<rdl2::Vec4d, rdl2::Vec4d, rdl2::Vec4d, rdl2::Vec4d>(),
             py::arg("vx"), py::arg("vy"), py::arg("vz"), py::arg("vw"))
        .def_readwrite("vx", &rdl2::Mat4d::vx)
        .def_readwrite("vy", &rdl2::Mat4d::vy)
        .def_readwrite("vz", &rdl2::Mat4d::vz)
        .def_readwrite("vw", &rdl2::Mat4d::vw)
        .def("__repr__", [](const rdl2::Mat4d& m) {
            return "Mat4d([" + std::to_string(m.vx.x) + ", " + std::to_string(m.vx.y) +
                   ", " + std::to_string(m.vx.z) + ", " + std::to_string(m.vx.w) + "], ...)";
        });
}
