/**
 * @file   contour_capi.h
 * @brief  C API for contour library (for P/Invoke, FFI, etc.)
 *
 * Copyright 2018 Jens Munk Hansen
 */

#ifndef CONTOUR_CAPI_H
#define CONTOUR_CAPI_H

#include <stddef.h>

#ifdef USE_CMAKE
#include <contour/contour_export.h>
#else
#define CONTOUR_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Free memory allocated by contour functions.
 * @param ptr Pointer to free (NULL is safe)
 */
CONTOUR_EXPORT void contour_free(void* ptr);

/**
 * Compute contours for a 2D double-precision floating point image.
 *
 * @param pData       Image data (row-major)
 * @param nYdata      Y dimension (rows)
 * @param nXdata      X dimension (columns)
 * @param pY          Y-coordinates array
 * @param nY          Number of Y-coordinates (must equal nYdata)
 * @param pX          X-coordinates array
 * @param nX          Number of X-coordinates (must equal nXdata)
 * @param pLevels     Contour levels (must be increasing)
 * @param nLevels     Number of levels
 * @param ppOutY      [out] Y-coordinates of contour segments (caller must free with contour_free)
 * @param nOutY       [out] Number of Y-coordinates
 * @param ppOutX      [out] X-coordinates of contour segments (caller must free with contour_free)
 * @param nOutX       [out] Number of X-coordinates
 * @param nOutLengths [out] Number of segments per level (caller must free with contour_free)
 * @param nOutSegments [out] Number of levels
 * @return 0 on success, -1 on error
 */
CONTOUR_EXPORT int contour_compute(
    const double* pData, size_t nYdata, size_t nXdata,
    const double* pY, size_t nY,
    const double* pX, size_t nX,
    const double* pLevels, size_t nLevels,
    double** ppOutY, size_t* nOutY,
    double** ppOutX, size_t* nOutX,
    size_t** nOutLengths, size_t* nOutSegments);

/**
 * Compute sorted contours (connected polygons) for a 2D image.
 *
 * @param pData          Image data (row-major)
 * @param nYdata         Y dimension (rows)
 * @param nXdata         X dimension (columns)
 * @param pY             Y-coordinates array
 * @param nY             Number of Y-coordinates (must equal nYdata)
 * @param pX             X-coordinates array
 * @param nX             Number of X-coordinates (must equal nXdata)
 * @param pLevels        Contour levels (must be increasing)
 * @param nLevels        Number of levels
 * @param ppOutY         [out] Y-coordinates (caller must free with contour_free)
 * @param nOutY          [out] Number of Y-coordinates
 * @param ppOutX         [out] X-coordinates (caller must free with contour_free)
 * @param nOutX          [out] Number of X-coordinates
 * @param nOutLengths    [out] Points per polygon (caller must free with contour_free)
 * @param nOutSegments   [out] Number of polygons
 * @param nLevelSegments [out] Polygons per level (caller must free with contour_free)
 * @param nLevels2       [out] Number of levels
 * @return 0 on success, -1 on error
 */
CONTOUR_EXPORT int contour_compute_sorted(
    const double* pData, size_t nYdata, size_t nXdata,
    const double* pY, size_t nY,
    const double* pX, size_t nX,
    const double* pLevels, size_t nLevels,
    double** ppOutY, size_t* nOutY,
    double** ppOutX, size_t* nOutX,
    size_t** nOutLengths, size_t* nOutSegments,
    size_t** nLevelSegments, size_t* nLevels2);

#ifdef __cplusplus
}
#endif

#endif /* CONTOUR_CAPI_H */
