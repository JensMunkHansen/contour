/**
 * @file   contour.cpp
 * @author Jens Munk Hansen <jmh@debian9laptop.parknet.dk>
 * @date   Fri Mar 30 01:32:36 2018
 *
 * @brief
 *
 * Copyright 2018 Jens Munk Hansen
 */

// TODO(JEM): Compare whole segments rather than points, i.e.

// Investigate Square Tracing Algorithm
// Moore-Neighbor Tracing
// Mike Alder: Radial Sweep
// Theo Pavlidid' Algorithm

#include <contour/config.h>
#include <contour/conrec.h>
#include <cmath>
#include <cstddef>
#include <memory>

#include <list>
#include <initializer_list>
#include <array>  // must be after initializer list
#include <vector>

#include <algorithm>

#if defined(__GNUC__) && defined(SPS_DEBUG)
# if !defined(__CYGWIN__)
#  include <sps/strace.hpp>
# endif
void ContourInit()     __attribute__((constructor(101)));
#elif defined(_WIN32)
void ContourInit();
#endif

void ContourInit() {
#if defined(__GNUC__) && !defined(__CYGWIN__)
# if !defined(NDEBUG) && defined(SPS_DEBUG)
  sps::STrace::Instance().Enable();
# endif
#endif
}

template <typename T>
using point2_t = std::array<T, 2>;

template<typename T>
using line2_t = std::array<point2_t<T>, 2>;

// Global variables
std::vector<std::list<line2_t<double> > > g_segments;
double g_dx = 0.0;
double g_dy = 0.0;

template <typename T>
inline bool operator==(point2_t<T>, point2_t<T>);

template <typename T>
inline bool operator!=(point2_t<T>, point2_t<T>);

template <typename T>
inline bool operator<(const point2_t<T>& p1, const point2_t<T>& p2);

template <typename T>
inline bool operator<(const line2_t<double>& p1, const line2_t<double>& p2);

#define DIFFERENCE 0.00005
#define EQ(_x_,_y_) (((_x_-_y_<DIFFERENCE)&&(_y_-_x_<DIFFERENCE))?1:0)

template <>
inline bool operator==(point2_t<double> p1, point2_t<double> p2) {
  return ((p1[0] - p2[0]) < g_dy && (p2[0] - p1[0]) < g_dy ) &&
         ((p1[1] - p2[1]) < g_dx && (p2[1] - p1[1]) < g_dx);
}

template <>
inline bool operator!=(point2_t<double> p1, point2_t<double> p2) {
  return (!(EQ(p1[0], p2[0]) && !(EQ(p1[1], p2[1]))));
}

template <>
inline bool operator<(const point2_t<double>& p1, const point2_t<double>& p2) {
  return (((p1[1]*(unsigned int)0xFFFFFFFF)+p1[0]) <
          ((p2[1]*(unsigned int)0xFFFFFFFF)+p2[0]));
}

template <>
inline bool operator <(const line2_t<double>& p1, const line2_t<double>& p2) {
  return (p1[0] < p2[0]);
}


// Used by old Fortran subroutine - x is major index
void segment_add(double x1, double y1,
                 double x2, double y2,
                 int level) {
  g_segments[level].push_back({{ {y1, x1}, {y2, x2} }});
}

void pack_output(const std::list<std::list<point2_t<double> > >& polygons,
                 double** ppOutY, size_t* nOutY,
                 double** ppOutX, size_t* nOutX,
                 size_t** nOutLengths, size_t *nOutSegments);

void sort_segments(std::vector<std::list<line2_t<double> > >* segments,
                   std::list<std::list<point2_t<double> > >* polygons,
                   size_t** nLevelSegments, size_t* pnLevels);

// Another version for sorting
void sort_segments2(std::vector<std::list<line2_t<double> > >* segments,
                    std::list<std::list<point2_t<double> > >* polygons,
                    size_t** nLevelSegments, size_t* pnLevels);


int contours_internal(
  const double* pData, const size_t nYdata, const size_t nXdata,
  const double* pY, const double* pX,
  const double* pLevels, const size_t nLevels,
  double** ppOutY, double** ppOutX, size_t* nCoordinates,
  size_t** nOutLengths) {
  // Clear old segments
  g_segments.clear();
  g_segments.resize(nLevels);

  // Establish row pointers
  double** ppData = new double*[nYdata];
  for (size_t iY = 0 ; iY < nYdata ; iY++) {
    ppData[iY] = const_cast<double*>(&pData[iY*nXdata]);
  }

  int ilb = 0;
  int iub = static_cast<int>(nYdata)-1;
  int jlb = 0;
  int jub = static_cast<int>(nXdata)-1;

  // Data is accessed according to ppData[i][j]
  Contour(ppData, ilb, iub, jlb, jub,
          const_cast<double*>(pY),
          const_cast<double*>(pX),
          static_cast<int>(nLevels),
          const_cast<double*>(pLevels),
          segment_add);

  delete[] ppData;

  // For output - can be omitted for sorted algorithm
  *nOutLengths = static_cast<size_t*>(malloc(nLevels*sizeof(size_t)));

  *nCoordinates = 0;
  size_t iPoint = 0;
  size_t nSegments;

  for (size_t iLevel = 0 ; iLevel < nLevels ; iLevel++) {
    nSegments = g_segments[iLevel].size();
    (*nOutLengths)[iLevel] = nSegments;
    (*nCoordinates) += 2 * nSegments;
  }

  *ppOutX = static_cast<double*>(malloc((*nCoordinates)*sizeof(double)));
  *ppOutY = static_cast<double*>(malloc((*nCoordinates)*sizeof(double)));

  for (size_t iLevel = 0 ; iLevel < nLevels ; iLevel++) {
    nSegments = g_segments[iLevel].size();
    for (auto it : g_segments[iLevel]) {
      (*ppOutX)[iPoint] = it[0][0];
      (*ppOutY)[iPoint] = it[0][1];
      iPoint++;
      (*ppOutX)[iPoint] = it[1][0];
      (*ppOutY)[iPoint] = it[1][1];
      iPoint++;
    }
  }
  return 0;
}

int contours(
  const double* pData, const size_t nYdata, const size_t nXdata,
  const double* pY, const size_t nY,
  const double* pX, const size_t nX,
  const double* pLevels, const size_t nLevels,
  double** ppOutY, size_t* nOutY,
  double** ppOutX, size_t* nOutX,
  size_t** nOutLengths, size_t* nOutSegments) {
  int retval = 0;
  if (nXdata != nX || nYdata != nY || nLevels == 0 || nX == 0 || nY == 0) {
    retval = -1;
    *ppOutX = nullptr;
    *nOutX = 0;
    *ppOutY = nullptr;
    *nOutY = 0;
    *nOutLengths = nullptr;
    *nOutSegments = 0;
  } else {
    size_t nCoordinates;
    retval = contours_internal(pData, nYdata, nXdata,
                               pY, pX,
                               pLevels, nLevels,
                               ppOutY, ppOutX, &nCoordinates,
                               nOutLengths);

    *nOutX = nCoordinates;
    *nOutY = nCoordinates;
    *nOutSegments = nLevels;
  }
  return retval;
}

int contours_sorted(
  const double* pData, const size_t nYdata, const size_t nXdata,
  const double* pY, const size_t nY,
  const double* pX, const size_t nX,
  const double* pLevels, const size_t nLevels,
  double** ppOutY, size_t* nOutY,
  double** ppOutX, size_t* nOutX,
  size_t** nOutLengths, size_t* nOutSegments,  // Length of segments
  size_t** nLevelSegments, size_t* nLevels2) {  // Segments per level
  // Return value
  int retval = 0;
  if (nXdata != nX || nYdata != nY || nLevels == 0 || nX == 0 || nY == 0) {
    retval = -1;
    *ppOutX = nullptr;
    *nOutX = 0;
    *ppOutY = nullptr;
    *nOutY = 0;
    *nOutLengths = nullptr;
    *nOutSegments = 0;
    *nLevelSegments = nullptr;
    *nLevels2 = 0;
  } else {
    // TODO(JEM): Re-use stuff from contours_internal

    // Clear old segments
    g_segments.clear();
    g_segments.resize(nLevels);

    // Establish row pointers
    double** ppData = new double*[nYdata];
    for (size_t iY = 0 ; iY < nYdata ; iY++) {
      ppData[iY] = const_cast<double*>(&pData[iY*nXdata]);
    }

    int ilb = 0;
    int iub = static_cast<int>(nYdata)-1;
    int jlb = 0;
    int jub = static_cast<int>(nXdata)-1;

    // Segment lengths vary - if we knew the kind, we could improve the ordering
    g_dy = 0.0001 * fabs(pY[1] - pY[0]);
    g_dx = 0.0001 * fabs(pX[1] - pX[0]);

    // Data is accessed according to ppData[i][j]
    Contour(ppData, ilb, iub, jlb, jub,
            const_cast<double*>(pY),
            const_cast<double*>(pX),
            static_cast<int>(nLevels),
            const_cast<double*>(pLevels),
            segment_add);

    delete[] ppData;

    // Sort segments:
    std::list<std::list<point2_t<double> > > polygons;

    sort_segments(&g_segments, &polygons, nLevelSegments, nLevels2);

    // Create output
    pack_output(polygons,
                ppOutY, nOutY,
                ppOutX, nOutX,
                nOutLengths, nOutSegments);
  }
  return retval;
}

void pack_output(const std::list<std::list<point2_t<double> > >& polygons,
                 double** ppOutY, size_t* nOutY,
                 double** ppOutX, size_t* nOutX,
                 size_t** nOutLengths, size_t *nOutSegments) {
  // Create output
  size_t nCoordinates = 0;

  size_t nSegments = polygons.size();
  for (auto it2 : polygons) {
    nCoordinates += it2.size();
  }

  *nOutX = nCoordinates;
  *nOutY = nCoordinates;
  *nOutSegments = nSegments;

  *ppOutX = static_cast<double*>(malloc(nCoordinates*sizeof(double)));
  *ppOutY = static_cast<double*>(malloc(nCoordinates*sizeof(double)));
  *nOutLengths = static_cast<size_t*>(malloc(nSegments*sizeof(size_t)));

  size_t iPoint = 0;
  size_t iSegment = 0;
  for (auto it2 : polygons) {
    (*nOutLengths)[iSegment] = it2.size();
    for (auto it3 : it2) {
      (*ppOutX)[iPoint] = it3[0];
      (*ppOutY)[iPoint] = it3[1];
      iPoint++;
    }
    iSegment++;
  }
}

void sort_segments(
  std::vector<std::list<line2_t<double> > >* segments,
  std::list<std::list<point2_t<double> > >* polygons,
  size_t** nLevelSegments, size_t* pnLevels) {
  size_t nLevels = segments->size();
  *nLevelSegments = static_cast<size_t*>(malloc(nLevels*sizeof(size_t)));
  *pnLevels = nLevels;

  for (size_t iLevel = 0 ; iLevel < nLevels ; iLevel++) {
    size_t nPolygons = 0;
    auto it0 = (*segments)[iLevel].begin();

    while (it0 != (*segments)[iLevel].end()) {
      auto seg = *it0;

      it0 = (*segments)[iLevel].erase(it0);

      // Polygon with one edge
      std::list<point2_t<double> > polygon;
      polygon.push_back(seg[0]);
      polygon.push_back(seg[1]);

      // Add polygon to list
      polygons->push_back(polygon);

      // Check for more edges
      auto it1 = it0;

      while (true) {
        size_t nSegmentsLeft = (*segments)[iLevel].size();

        while (it1 != (*segments)[iLevel].end()) {
          auto& poly = polygons->back();
          seg = *it1;
          if (seg[0] == poly.back()) {
            poly.push_back(seg[1]);
            (*segments)[iLevel].erase(it1);
            it1 = (*segments)[iLevel].begin();
            break;  // Start again at the beginning
          } else if (seg[1] == poly.back()) {
            poly.push_back(seg[0]);
            (*segments)[iLevel].erase(it1);
            it1 = (*segments)[iLevel].begin();
            break;  // Start again at the beginning
          } else if (seg[0] == poly.front()) {
            poly.push_front(seg[1]);
            (*segments)[iLevel].erase(it1);
            it1 = (*segments)[iLevel].begin();
            break;  // Start again at the beginning
          } else if (seg[1] == poly.front()) {
            poly.push_front(seg[0]);
            (*segments)[iLevel].erase(it1);
            it1 = (*segments)[iLevel].begin();
            break;  // Start again at the beginning
          } else {
            it1++;
          }
        }
        if (nSegmentsLeft == (*segments)[iLevel].size()) {
          // Done with polygon
          nPolygons++;
          break;
        }
      }
      if (!(*segments)[iLevel].empty()) {
        // Old iterator may point to element, which has been
        // erased, so we cannot simply increment it
        it0 = (*segments)[iLevel].begin();
      } else {
        break;
      }
    }
    (*nLevelSegments)[iLevel] = nPolygons;
  }
}

// Algorithm from Bjorn Harpe
class CContour {
 public:
  CContour() : m_contour(), m_begin(), m_end() {}
  int add_vector(const point2_t<double>& p1, const point2_t<double>& p2) {
    point2_t<double> v;
    v[0] = p2[0] - p1[0];
    v[1] = p2[1] - p1[1];
    if (m_contour.empty()) {
      m_begin = p1;
    }
    m_contour.push_back(v);
    m_end = p2;
    return 0;
  }

  int reverse() {
    std::swap<point2_t<double> >(m_begin, m_end);

    std::vector<point2_t<double> > tmp;
    auto it = m_contour.begin();
    while (it != m_contour.end()) {
      (*it)[0] *= -1.0;
      (*it)[1] *= -1.0;
      tmp.insert(tmp.begin(), (*it));
      it++;
    }
    m_contour = tmp;
    return 0;
  }

  int merge(const CContour& c) {
    m_contour.insert(m_contour.end(), c.m_contour.begin(), c.m_contour.end());
    m_end = c.m_end;
    return 0;
  }
  point2_t<double> end() {
    return m_end;
  }
  point2_t<double> begin() {
    return m_begin;
  }
  int condense(double difference = 0.000000001) {
    /*
      at this time we potentially have multiple SVectors in the contour vector that
      are colinear and could be condensed into one SVector with, to determine if two
      successive vectors are colinear we take each vector and divide the y component
      of the vector by the x component, giving us the slope. we pass in a difference
      if the difference between th two slopes is less than the difference that we
      pass in and since we already know that both segments share a common point they
      can obviously be condensed. in the sample code on this page this is evident it
      the bounding rectangle. another possibility is modifying the code to allow
      point intersections on the plane. In this instance we may have multiple
      identical vectors with no magnitude that can be reduced to a single data point.
    */

    double m1, m2;
    auto it = m_contour.begin();
    auto jt = it+1;
    while (jt != m_contour.end()) {
      if (((*jt)[0]) && ((*it)[0])) {
        m1 = (*jt)[1]/(*jt)[0];
        m2 = (*it)[1]/(*jt)[0];
      } else if (((*jt)[1]) && ((*it)[1])) {
        m1 = (*jt)[0]/(*jt)[1];
        m2 = (*it)[0]/(*jt)[1];
      } else {
        it++;
        jt++;
        continue;
      }
      if ((m1-m2 < difference) && (m2-m1 < difference)) {
        (*it)[1]+=(*jt)[1];
        (*it)[0]+=(*jt)[0];
        jt = m_contour.erase(jt);
      } else {
        it++;
        jt++;
      }
    }
    return 0;
  }

  ~CContour() {
    m_contour.clear();
  }

  std::vector<point2_t<double> > m_contour;
  point2_t<double> m_begin;
  point2_t<double> m_end;
};

int merge(std::vector<CContour>* contours) {
  int c = 0;
  if (contours->size() < 2) {
    return c;
  }

  auto it = contours->begin();
  auto jt = it;
  /*
  using two iterators we walk through the entire vector testing to
  see if some combination of the start and end points match. If we
  find matching points the two vectorsrs are merged. since when we go
  thru the vector once we are garanteed that all vectors that can
  connect to that oone have been merged we only have to merge the
  vector less the processed nodes at the begining. every merge does
  force jt back to the beginning of the search tho since a merge will
  change either the start or the end of the vector
  */
  while (it != contours->end()) {
    jt = it+1;
    while (jt != contours->end()) {
      if ((*it).end() == (*jt).begin()) {
        /*
          if the end of *it matches the start ot *jt we can copy
          *jt to the end of *it and remove jt, the erase funtion
          does us a favour and increments the iterator to the
          next element so we continue to test the next element
        */
        (*it).merge(*jt);
        contours->erase(jt);
        jt = it + 1;
        c++;
      } else if ((*jt).end() == (*it).begin()) {
        /*
          similarily if the end of *jt matches the start ot *it we can copy
          *it to the end of *jt and remove it,replacing it with jt, we then
          neet to update it to point at the just inserted record.
        */
        (*jt).merge(*it);
        (*it) = (*jt);
        contours->erase(jt);
        jt = it + 1;
        c++;
      } else if ((*it).end() == ((*jt).end())) {
        /*
          if both segments end at the same point we reverse one and merge
          it to the other, then remove the one we merged.
        */
        (*jt).reverse();
        (*it).merge(*jt);
        contours->erase(jt);
        jt = it + 1;
        c++;
      } else if ((*it).begin() == ((*jt).begin())) {
        /*
          if both segments start at the same point reverse it, then merge
          it with jt, removing jt and reseting jt to the start of the search
          sequence
        */
        (*it).reverse();
        (*it).merge(*jt);
        jt = contours->erase(jt);
        c++;
      } else {
        jt++;
      }
    }
    it++;
  }
  return c;
}

// Bjorn Harpe (not good either)
void sort_segments2(std::vector<std::list<line2_t<double> > >* segments,
                    std::list<std::list<point2_t<double> > >* polygons,
                    size_t** nLevelSegments, size_t* pnLevels) {
  size_t nLevels = segments->size();
  *nLevelSegments = static_cast<size_t*>(malloc(nLevels*sizeof(size_t)));
  *pnLevels = nLevels;

  size_t nContours = 0;
  int c = 0;

  polygons->clear();

  for (size_t iLevel = 0 ; iLevel < nLevels ; iLevel++) {
    auto& segment = (*segments)[iLevel];
    //  random access iterator is needed
    //  std::sort(segment.begin(), segment.end());

    std::vector<CContour> contours;

    segment.sort();
    while (!segment.empty()) {
      nContours++;
      auto it = segment.begin();
      CContour polygon;
      auto seg = *it;
      polygon.add_vector(seg[0], seg[1]);
      it = segment.erase(it);
      while (it != segment.end()) {
        if ((*it)[0] == polygon.end()) {
          polygon.add_vector((*it)[0], (*it)[1]);
          segment.erase(it);
          it = segment.begin();
        } else {
          it++;
        }
      }
      contours.push_back(polygon);
    }
    c -= merge(&contours);
    auto it0 = contours.begin();
    while (it0 != contours.end()) {
      (*it0).condense();
      it0++;
    }

    size_t nContoursAtLevel = 0;

    // Move contours to output - dump
    for (auto tmp : contours) {
      std::list<point2_t<double> > nextContour;
      auto p0 = tmp.begin();
      nextContour.push_back(p0);
      auto it1 = tmp.m_contour.begin();
      while (it1 != tmp.m_contour.end()) {
        p0[0] += (*it1)[0];
        p0[1] += (*it1)[1];
        nextContour.push_back(p0);
        it1++;
      }
      polygons->push_back(nextContour);
      nContoursAtLevel++;
    }
    (*nLevelSegments)[iLevel] = nContoursAtLevel;
  }
}

/* Local variables: */
/* indent-tabs-mode: nil */
/* tab-width: 2 */
/* c-basic-offset: 2 */
/* End: */
