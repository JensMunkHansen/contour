#!/usr/bin/env python
import numpy
from distutils.core import setup, Extension

setup(name="swig_contour",
      py_modules=["swig_contour"],
      ext_modules=[Extension("contour/_swig_contour",
                     ["contour/swig_contour.i", "contour/contour.cpp", "contour/conrec.c"],
                     include_dirs = [numpy.get_include(), '.'],
                     swig_opts=['-c++', '-I.'],
                  )]

)



