# Copyright (c) 2026 Alan Blevins
# SPDX-License-Identifier: MIT

from setuptools import setup, Extension
from pybind11.setup_helpers import Pybind11Extension, build_ext
from pybind11 import get_cmake_dir
import pybind11

ext_modules = [
    Pybind11Extension(
        "scene_rdl2",
        [
            "src/module.cpp",
        ],
        include_dirs=[
            # Path to pybind11 headers
            get_cmake_dir(),
            "include",
        ],
        # Example: passing in the version to the preprocessor
        define_macros=[("VERSION_INFO", "0.1.0")],
    ),
]

setup(
    name="scene_rdl2",
    version="0.1.0",
    author="DreamWorks Animation",
    author_email="",
    description="Python bindings for scene_rdl2 library",
    long_description=open("README.md").read(),
    long_description_content_type="text/markdown",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.7",
    install_requires=[
        "pybind11>=2.6.0",
    ],
    setup_requires=[
        "pybind11>=2.6.0",
    ],
)