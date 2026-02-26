// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for scene_rdl2 math types:
//   Rgb, Rgba, Vec2f, Vec2d, Vec3f, Vec3d, Vec4f, Vec4d, Mat4f, Mat4d

#include "bindings.h"

// ---------------------------------------------------------------------------
// Helpers: build a Vec4f / Vec4d from a Python object (Vec4* or sequence)
// ---------------------------------------------------------------------------
static rdl2::Vec4f to_vec4f(py::object h)
{
    if (py::isinstance<rdl2::Vec4f>(h))
        return py::cast<rdl2::Vec4f>(h);
    py::sequence s = py::cast<py::sequence>(h);
    if (py::len(s) != 4)
        throw py::value_error("each row must have 4 elements");
    return rdl2::Vec4f(py::cast<float>(s[0]), py::cast<float>(s[1]),
                       py::cast<float>(s[2]), py::cast<float>(s[3]));
}

static rdl2::Vec4d to_vec4d(py::object h)
{
    if (py::isinstance<rdl2::Vec4d>(h))
        return py::cast<rdl2::Vec4d>(h);
    py::sequence s = py::cast<py::sequence>(h);
    if (py::len(s) != 4)
        throw py::value_error("each row must have 4 elements");
    return rdl2::Vec4d(py::cast<double>(s[0]), py::cast<double>(s[1]),
                       py::cast<double>(s[2]), py::cast<double>(s[3]));
}

void bind_math(py::module_& m)
{
    // -----------------------------------------------------------------------
    // Rgb (Col3f): r, g, b
    // -----------------------------------------------------------------------
    py::class_<rdl2::Rgb>(m, "Rgb")
        .def(py::init<float>(), py::arg("v") = 0.0f)
        .def(py::init<float, float, float>(), py::arg("r"), py::arg("g"), py::arg("b"))
        .def("__init__", [](rdl2::Rgb* self, py::sequence s) {
            if (py::len(s) != 3)
                throw py::value_error(("Rgb requires 3 elements, got " +
                                       std::to_string(py::len(s))).c_str());
            new (self) rdl2::Rgb(py::cast<float>(s[0]), py::cast<float>(s[1]),
                                 py::cast<float>(s[2]));
        }, py::arg("s"))
        .def_rw("r", &rdl2::Rgb::r)
        .def_rw("g", &rdl2::Rgb::g)
        .def_rw("b", &rdl2::Rgb::b)
        .def("__repr__", [](const rdl2::Rgb& c) {
            return "Rgb(" + std::to_string(c.r) + ", " +
                   std::to_string(c.g) + ", " + std::to_string(c.b) + ")";
        })
        .def("__eq__", [](const rdl2::Rgb& a, const rdl2::Rgb& b) {
            return a.r == b.r && a.g == b.g && a.b == b.b;
        })
        .def("__iter__", [](const rdl2::Rgb& c) {
            return py::iter(py::make_tuple(c.r, c.g, c.b));
        });
    py::implicitly_convertible<py::list,  rdl2::Rgb>();
    py::implicitly_convertible<py::tuple, rdl2::Rgb>();

    // -----------------------------------------------------------------------
    // Rgba (Col4f): r, g, b, a
    // -----------------------------------------------------------------------
    py::class_<rdl2::Rgba>(m, "Rgba")
        .def(py::init<float>(), py::arg("v") = 0.0f)
        .def(py::init<float, float, float, float>(),
             py::arg("r"), py::arg("g"), py::arg("b"), py::arg("a"))
        .def("__init__", [](rdl2::Rgba* self, py::sequence s) {
            if (py::len(s) != 4)
                throw py::value_error(("Rgba requires 4 elements, got " +
                                       std::to_string(py::len(s))).c_str());
            new (self) rdl2::Rgba(py::cast<float>(s[0]), py::cast<float>(s[1]),
                                  py::cast<float>(s[2]), py::cast<float>(s[3]));
        }, py::arg("s"))
        .def_rw("r", &rdl2::Rgba::r)
        .def_rw("g", &rdl2::Rgba::g)
        .def_rw("b", &rdl2::Rgba::b)
        .def_rw("a", &rdl2::Rgba::a)
        .def("__repr__", [](const rdl2::Rgba& c) {
            return "Rgba(" + std::to_string(c.r) + ", " + std::to_string(c.g) +
                   ", " + std::to_string(c.b) + ", " + std::to_string(c.a) + ")";
        })
        .def("__eq__", [](const rdl2::Rgba& a, const rdl2::Rgba& b) {
            return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
        });
    py::implicitly_convertible<py::list,  rdl2::Rgba>();
    py::implicitly_convertible<py::tuple, rdl2::Rgba>();

    // -----------------------------------------------------------------------
    // Vec2f
    // -----------------------------------------------------------------------
    py::class_<rdl2::Vec2f>(m, "Vec2f")
        .def(py::init<>())
        .def(py::init<float>(), py::arg("v"))
        .def(py::init<float, float>(), py::arg("x"), py::arg("y"))
        .def("__init__", [](rdl2::Vec2f* self, py::sequence s) {
            if (py::len(s) != 2)
                throw py::value_error(("Vec2f requires 2 elements, got " +
                                       std::to_string(py::len(s))).c_str());
            new (self) rdl2::Vec2f(py::cast<float>(s[0]), py::cast<float>(s[1]));
        }, py::arg("s"))
        .def_rw("x", &rdl2::Vec2f::x)
        .def_rw("y", &rdl2::Vec2f::y)
        .def("__repr__", [](const rdl2::Vec2f& v) {
            return "Vec2f(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        })
        .def("__eq__", [](const rdl2::Vec2f& a, const rdl2::Vec2f& b) {
            return a.x == b.x && a.y == b.y;
        });
    py::implicitly_convertible<py::list,  rdl2::Vec2f>();
    py::implicitly_convertible<py::tuple, rdl2::Vec2f>();

    // -----------------------------------------------------------------------
    // Vec2d
    // -----------------------------------------------------------------------
    py::class_<rdl2::Vec2d>(m, "Vec2d")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("v"))
        .def(py::init<double, double>(), py::arg("x"), py::arg("y"))
        .def("__init__", [](rdl2::Vec2d* self, py::sequence s) {
            if (py::len(s) != 2)
                throw py::value_error(("Vec2d requires 2 elements, got " +
                                       std::to_string(py::len(s))).c_str());
            new (self) rdl2::Vec2d(py::cast<double>(s[0]), py::cast<double>(s[1]));
        }, py::arg("s"))
        .def_rw("x", &rdl2::Vec2d::x)
        .def_rw("y", &rdl2::Vec2d::y)
        .def("__repr__", [](const rdl2::Vec2d& v) {
            return "Vec2d(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        });
    py::implicitly_convertible<py::list,  rdl2::Vec2d>();
    py::implicitly_convertible<py::tuple, rdl2::Vec2d>();

    // -----------------------------------------------------------------------
    // Vec3f
    // -----------------------------------------------------------------------
    py::class_<rdl2::Vec3f>(m, "Vec3f")
        .def(py::init<>())
        .def(py::init<float>(), py::arg("v"))
        .def(py::init<float, float, float>(), py::arg("x"), py::arg("y"), py::arg("z"))
        .def("__init__", [](rdl2::Vec3f* self, py::sequence s) {
            if (py::len(s) != 3)
                throw py::value_error(("Vec3f requires 3 elements, got " +
                                       std::to_string(py::len(s))).c_str());
            new (self) rdl2::Vec3f(py::cast<float>(s[0]), py::cast<float>(s[1]),
                                   py::cast<float>(s[2]));
        }, py::arg("s"))
        .def_rw("x", &rdl2::Vec3f::x)
        .def_rw("y", &rdl2::Vec3f::y)
        .def_rw("z", &rdl2::Vec3f::z)
        .def("__repr__", [](const rdl2::Vec3f& v) {
            return "Vec3f(" + std::to_string(v.x) + ", " +
                   std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
        })
        .def("__eq__", [](const rdl2::Vec3f& a, const rdl2::Vec3f& b) {
            return a.x == b.x && a.y == b.y && a.z == b.z;
        })
        .def("__iter__", [](const rdl2::Vec3f& v) {
            return py::iter(py::make_tuple(v.x, v.y, v.z));
        });
    py::implicitly_convertible<py::list,  rdl2::Vec3f>();
    py::implicitly_convertible<py::tuple, rdl2::Vec3f>();

    // -----------------------------------------------------------------------
    // Vec3d
    // -----------------------------------------------------------------------
    py::class_<rdl2::Vec3d>(m, "Vec3d")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("v"))
        .def(py::init<double, double, double>(), py::arg("x"), py::arg("y"), py::arg("z"))
        .def("__init__", [](rdl2::Vec3d* self, py::sequence s) {
            if (py::len(s) != 3)
                throw py::value_error(("Vec3d requires 3 elements, got " +
                                       std::to_string(py::len(s))).c_str());
            new (self) rdl2::Vec3d(py::cast<double>(s[0]), py::cast<double>(s[1]),
                                   py::cast<double>(s[2]));
        }, py::arg("s"))
        .def_rw("x", &rdl2::Vec3d::x)
        .def_rw("y", &rdl2::Vec3d::y)
        .def_rw("z", &rdl2::Vec3d::z)
        .def("__repr__", [](const rdl2::Vec3d& v) {
            return "Vec3d(" + std::to_string(v.x) + ", " +
                   std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
        });
    py::implicitly_convertible<py::list,  rdl2::Vec3d>();
    py::implicitly_convertible<py::tuple, rdl2::Vec3d>();

    // -----------------------------------------------------------------------
    // Vec4f
    // -----------------------------------------------------------------------
    py::class_<rdl2::Vec4f>(m, "Vec4f")
        .def(py::init<>())
        .def(py::init<float>(), py::arg("v"))
        .def(py::init<float, float, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("z"), py::arg("w"))
        .def("__init__", [](rdl2::Vec4f* self, py::sequence s) {
            if (py::len(s) != 4)
                throw py::value_error(("Vec4f requires 4 elements, got " +
                                       std::to_string(py::len(s))).c_str());
            new (self) rdl2::Vec4f(py::cast<float>(s[0]), py::cast<float>(s[1]),
                                   py::cast<float>(s[2]), py::cast<float>(s[3]));
        }, py::arg("s"))
        .def_rw("x", &rdl2::Vec4f::x)
        .def_rw("y", &rdl2::Vec4f::y)
        .def_rw("z", &rdl2::Vec4f::z)
        .def_rw("w", &rdl2::Vec4f::w)
        .def("__repr__", [](const rdl2::Vec4f& v) {
            return "Vec4f(" + std::to_string(v.x) + ", " + std::to_string(v.y) +
                   ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")";
        });
    py::implicitly_convertible<py::list,  rdl2::Vec4f>();
    py::implicitly_convertible<py::tuple, rdl2::Vec4f>();

    // -----------------------------------------------------------------------
    // Vec4d
    // -----------------------------------------------------------------------
    py::class_<rdl2::Vec4d>(m, "Vec4d")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("v"))
        .def(py::init<double, double, double, double>(),
             py::arg("x"), py::arg("y"), py::arg("z"), py::arg("w"))
        .def("__init__", [](rdl2::Vec4d* self, py::sequence s) {
            if (py::len(s) != 4)
                throw py::value_error(("Vec4d requires 4 elements, got " +
                                       std::to_string(py::len(s))).c_str());
            new (self) rdl2::Vec4d(py::cast<double>(s[0]), py::cast<double>(s[1]),
                                   py::cast<double>(s[2]), py::cast<double>(s[3]));
        }, py::arg("s"))
        .def_rw("x", &rdl2::Vec4d::x)
        .def_rw("y", &rdl2::Vec4d::y)
        .def_rw("z", &rdl2::Vec4d::z)
        .def_rw("w", &rdl2::Vec4d::w)
        .def("__repr__", [](const rdl2::Vec4d& v) {
            return "Vec4d(" + std::to_string(v.x) + ", " + std::to_string(v.y) +
                   ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")";
        });
    py::implicitly_convertible<py::list,  rdl2::Vec4d>();
    py::implicitly_convertible<py::tuple, rdl2::Vec4d>();

    // -----------------------------------------------------------------------
    // Mat4f (vx, vy, vz, vw are Vec4f rows)
    // Accepts: Mat4f(Vec4f, Vec4f, Vec4f, Vec4f)
    //          Mat4f([[r,r,r,r], [r,r,r,r], [r,r,r,r], [r,r,r,r]])
    // -----------------------------------------------------------------------
    py::class_<rdl2::Mat4f>(m, "Mat4f")
        .def(py::init<>())
        .def(py::init<rdl2::Vec4f, rdl2::Vec4f, rdl2::Vec4f, rdl2::Vec4f>(),
             py::arg("vx"), py::arg("vy"), py::arg("vz"), py::arg("vw"))
        .def("__init__", [](rdl2::Mat4f* self, py::sequence rows) {
            if (py::len(rows) != 4)
                throw py::value_error(("Mat4f requires 4 rows, got " +
                                       std::to_string(py::len(rows))).c_str());
            new (self) rdl2::Mat4f(to_vec4f(py::borrow<py::object>(rows[0])),
                                   to_vec4f(py::borrow<py::object>(rows[1])),
                                   to_vec4f(py::borrow<py::object>(rows[2])),
                                   to_vec4f(py::borrow<py::object>(rows[3])));
        }, py::arg("rows"))
        .def_rw("vx", &rdl2::Mat4f::vx)
        .def_rw("vy", &rdl2::Mat4f::vy)
        .def_rw("vz", &rdl2::Mat4f::vz)
        .def_rw("vw", &rdl2::Mat4f::vw)
        .def("__repr__", [](const rdl2::Mat4f& m) {
            return "Mat4f([" + std::to_string(m.vx.x) + ", " + std::to_string(m.vx.y) +
                   ", " + std::to_string(m.vx.z) + ", " + std::to_string(m.vx.w) + "], ...)";
        });
    py::implicitly_convertible<py::list,  rdl2::Mat4f>();
    py::implicitly_convertible<py::tuple, rdl2::Mat4f>();

    // -----------------------------------------------------------------------
    // Mat4d (vx, vy, vz, vw are Vec4d rows)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Mat4d>(m, "Mat4d")
        .def(py::init<>())
        .def(py::init<rdl2::Vec4d, rdl2::Vec4d, rdl2::Vec4d, rdl2::Vec4d>(),
             py::arg("vx"), py::arg("vy"), py::arg("vz"), py::arg("vw"))
        .def("__init__", [](rdl2::Mat4d* self, py::sequence rows) {
            if (py::len(rows) != 4)
                throw py::value_error(("Mat4d requires 4 rows, got " +
                                       std::to_string(py::len(rows))).c_str());
            new (self) rdl2::Mat4d(to_vec4d(py::borrow<py::object>(rows[0])),
                                   to_vec4d(py::borrow<py::object>(rows[1])),
                                   to_vec4d(py::borrow<py::object>(rows[2])),
                                   to_vec4d(py::borrow<py::object>(rows[3])));
        }, py::arg("rows"))
        .def_rw("vx", &rdl2::Mat4d::vx)
        .def_rw("vy", &rdl2::Mat4d::vy)
        .def_rw("vz", &rdl2::Mat4d::vz)
        .def_rw("vw", &rdl2::Mat4d::vw)
        .def("__repr__", [](const rdl2::Mat4d& m) {
            return "Mat4d([" + std::to_string(m.vx.x) + ", " + std::to_string(m.vx.y) +
                   ", " + std::to_string(m.vx.z) + ", " + std::to_string(m.vx.w) + "], ...)";
        });
    py::implicitly_convertible<py::list,  rdl2::Mat4d>();
    py::implicitly_convertible<py::tuple, rdl2::Mat4d>();
}
