import numpy
from distutils.core import setup, Extension

setup(name="swig_contour",
      py_modules=["swig_contour"],
      ext_modules=[Extension("_swig_contour",
                     ["swig_contour.i", "contour.cpp", "conrec.c"],
                     include_dirs = [numpy.get_include(), '../'],
                     swig_opts=['-c++', '-I../'],
                  )]

)



