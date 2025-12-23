%module(docstring="This is a wrapper for contour") swig_contour
#pragma SWIG nowarn=320
%{
  #define SWIG_FILE_WITH_INIT
  #include <contour/contour.hpp>
%}

// SWIG 4.1+ compatibility - SWIG_Python_AppendOutput now requires 3 args
%insert("header") %{
#if SWIG_VERSION >= 0x040100
  #define SWIG_AppendOutput(result, obj) SWIG_Python_AppendOutput(result, obj, 0)
#else
  #define SWIG_AppendOutput(result, obj) SWIG_Python_AppendOutput(result, obj)
#endif
%}

%include "windows.i"

#ifdef SWIGPYTHON
  %include "numpy.i"

  %init {
    import_array();
  }
#endif

#define CONTOUR_EXPORT

%apply (double* IN_ARRAY2, int DIM1, int DIM2) \
{(const double* pData, const size_t nYdata, const size_t nXdata)};

%apply (double* IN_ARRAY1, int DIM1) \
{(const double* pY, const size_t nY)};

%apply (double* IN_ARRAY1, int DIM1) \
{(const double* pX, const size_t nX)};

%apply (double* IN_ARRAY1, int DIM1) \
{(const double* pLevels, const size_t nLevels)};

%apply (double** ARGOUTVIEWM_ARRAY1, size_t* DIM1) \
{(double** ppOutY, size_t* nOutY)};

%apply (double** ARGOUTVIEWM_ARRAY1, size_t* DIM1)  \
{(double** ppOutX, size_t* nOutX)};

%apply (size_t** ARGOUTVIEWM_ARRAY1, size_t* DIM1) \
{(size_t** nOutLengths, size_t* nOutSegments)};

%apply (size_t** ARGOUTVIEWM_ARRAY1, size_t* DIM1) \
{(size_t** nLevelSegments, size_t* nLevels2)};


%include <contour/contour.hpp>
