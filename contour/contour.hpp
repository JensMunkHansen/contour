/**
 * @file   contour.hpp
 * @author Jens Munk Hansen <jens.munk.hansen@gmail.com>
 * @date   Thu Mar 29 22:20:39 2018
 *
 * @brief
 *
 * Copyright 2018 Jens Munk Hansen
 */

#pragma once

#ifdef USE_CMAKE
# include <contour/contour_export.h>
#else
# define CONTOUR_EXPORT
#endif

/**
 * Sorted contours
 *
 * @param[in]  pData
 * @param[in]  nYdata
 * @param[in]  nXdata
 * @param[in]  pY
 * @param[in]  nY
 * @param[in]  pX
 * @param[in]  nX
 * @param[in]  pLevels
 * @param[in]  nLevels
 * @param[out] ppOutX
 * @param[out] nOutX
 * @param[out] ppOutY
 * @param[out] nOutY
 * @param[out] nOutLengths
 * @param[out] nOutSegments
 * @param[out] nLevelSegments
 * @param[out] nLevels2
 *
 * @return
 */
CONTOUR_EXPORT int contours_sorted(
  const double* pData,
  const size_t nYdata,
  const size_t nXdata,
  const double* pY,
  const size_t nY,
  const double* pX,
  const size_t nX,
  const double* pLevels,
  const size_t nLevels,
  double** ppOutX,
  size_t* nOutX,
  double** ppOutY,
  size_t* nOutY,
  size_t** nOutLengths,
  size_t* nOutSegments,
  size_t** nLevelSegments,
  size_t* nLevels2);

/**
 * Compute contours for a 2D double-precision floating point image
 *
 * The image \p pData is row-major with dimensions (\p nYdata, \p
 * nXdata). The coordinates used for locating pixels are given as two
 * arrays, \p pY and \p pX with dimensions, \p nY and \p nX,
 * respectively.
 *
 * The contour levels are given as an array, \p pLevels
 * with \p nLevels increasing values.
 *
 * @param[in] pData  Image data
 * @param[in] nYdata Dimension y (major index)
 * @param[in] nXdata Dimension x (minor index)
 * @param[in] pY     Y-coordinates
 * @param[in] nY     Number of y-coordinates
 * @param[in] pX     X-Coordinates
 * @param[in] nX     Number of x-coordinates
 * @param[in] pLevels Contour levels (increasing)
 * @param[in] nLevels Number of levels
 * @param[out] ppOutY Y-coordinates of contour segments
 * @param[out] nOutY  Number of y-coordinates for segments
 * @param[out] ppOutX X-coordinates of contour segments
 * @param[out] nOutX  Number of y-coordinates for segments
 * @param[out] nOutLengths Number of segments for each levels
 * @param[out] nOutSegments Number of levels (equals nLevels)
 *
 * @return
 */
CONTOUR_EXPORT int contours(
  const double* pData,
  const size_t nYdata,
  const size_t nXdata,
  const double* pY,
  const size_t nY,
  const double* pX,
  const size_t nX,
  const double* pLevels,
  const size_t nLevels,
  double** ppOutY,
  size_t* nOutY,
  double** ppOutX,
  size_t* nOutX,
  size_t** nOutLengths,
  size_t* nOutSegments);
