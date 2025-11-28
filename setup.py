#!/usr/bin/env python
"""
Setup script for swig_contour Python extension.

Usage:
    python setup.py build_ext --inplace
"""
import os
import shutil
import numpy
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


class BuildExtWithCopy(build_ext):
    """Custom build_ext that copies SWIG-generated .py to package root."""

    def run(self):
        super().run()
        # Copy SWIG-generated wrapper to root for proper import
        src = os.path.join(os.path.dirname(__file__), 'contour', 'swig_contour.py')
        dst = os.path.join(os.path.dirname(__file__), 'swig_contour.py')
        if os.path.exists(src):
            shutil.copy2(src, dst)
            print(f"Copied {src} -> {dst}")


setup(
    ext_modules=[
        Extension(
            name='_swig_contour',
            sources=[
                'contour/swig_contour.i',
                'contour/contour.cpp',
                'contour/conrec.c',
            ],
            include_dirs=[numpy.get_include(), '.'],
            swig_opts=['-c++', '-I.'],
            language='c++',
        )
    ],
    cmdclass={'build_ext': BuildExtWithCopy},
)
