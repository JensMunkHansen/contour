/**
 * @file   contour_capi.cpp
 * @brief  C API implementation for contour library
 *
 * Copyright 2018 Jens Munk Hansen
 */

#include <contour/contour_capi.h>
#include <contour/contour.hpp>
#include <cstdlib>

extern "C" {

void contour_free(void* ptr)
{
    free(ptr);
}

int contour_compute(
    const double* pData, size_t nYdata, size_t nXdata,
    const double* pY, size_t nY,
    const double* pX, size_t nX,
    const double* pLevels, size_t nLevels,
    double** ppOutY, size_t* nOutY,
    double** ppOutX, size_t* nOutX,
    size_t** nOutLengths, size_t* nOutSegments)
{
    return contours(pData, nYdata, nXdata,
                    pY, nY, pX, nX,
                    pLevels, nLevels,
                    ppOutY, nOutY, ppOutX, nOutX,
                    nOutLengths, nOutSegments);
}

int contour_compute_sorted(
    const double* pData, size_t nYdata, size_t nXdata,
    const double* pY, size_t nY,
    const double* pX, size_t nX,
    const double* pLevels, size_t nLevels,
    double** ppOutY, size_t* nOutY,
    double** ppOutX, size_t* nOutX,
    size_t** nOutLengths, size_t* nOutSegments,
    size_t** nLevelSegments, size_t* nLevels2)
{
    return contours_sorted(pData, nYdata, nXdata,
                           pY, nY, pX, nX,
                           pLevels, nLevels,
                           ppOutY, nOutY, ppOutX, nOutX,
                           nOutLengths, nOutSegments,
                           nLevelSegments, nLevels2);
}

} // extern "C"
