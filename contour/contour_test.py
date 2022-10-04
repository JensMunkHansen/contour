import sys
import numpy as np
from timeit import default_timer as timer
import matplotlib.pyplot as plt

import addpaths
import swig_contour

swap = False
#swap = True

def sort_segments(segments):
    """
    Sorts the segments to make polygons and polylines

    :param segments: list of pairs of coordinates to sort:
       [((0.0),(1,1)),
        ((2,3),(0,0)),
        ((1,1),(2,3)),
        ....
        ]

    :returns: list of polygons. each polygon is a list of coordinate pairs.

    """
    # must be a list (or, mutable sequence anyway...)
    if type(segments) is np.ndarray:
        segments = segments.tolist()
    # just in case it's a tuple or something
    segments = list(segments) if type(segments) is not list else segments

    polygons = []
    # Go backwards for efficieny
    while segments: # are there any left unaccounted for?
        # start a new polygon
        seg = segments.pop() # might as well take the last one
        polygons.append([seg[0], seg[1]])
        # look for segments to add to that polygon:
        while True:
            for seg in (segments):
                # see if it belongs to the last polygon:
                poly = polygons[-1]
                if seg[0] == poly[-1]:
                    poly.append(seg[1])
                    segments.remove(seg)
                    break #start again at the beginning
                elif seg[1] == poly[-1]:
                    poly.append(seg[0])
                    segments.remove(seg)
                    break #start again at the beginning
                # maybe it fits on the beginning:
                elif seg[0] == poly[0]:
                    poly.insert(0,seg[1])
                    segments.remove(seg)
                    break #start again at the beginning
                elif seg[1] == poly[0]:
                    poly.insert(0,seg[0])
                    segments.remove(seg)
                    break #start again at the beginning
                # segment doesn't match anything, moving to next segment
            else: # did not find a matching segment -- done with that polygon
                break # break out of while loop
                      # only if we get through the for loop without hitting a matching segment
    return polygons

plt.ion()

M, N = 1000, 800 #(image size)
# create an array:

x = np.linspace(0, 3*np.pi, 40)
y = np.linspace(0, 3*np.pi, 32) * 0.8
# reshape x:
x.shape = (-1, 1)
z = np.cos(x) + np.sin(y)

# note, x is minor index in z

levels = np.linspace(z.min(),z.max(), 12)
levels = levels[1:-1]

if swap:
  i = y.flatten()
  j = x.flatten()
  z = np.rot90(z)
else:
  i = x.flatten()
  j = y.flatten()

start = timer()
retval, xc, yc, nnSegments = swig_contour.contours(z, i, j, levels.flatten())
elapsed = timer() - start
print(elapsed)

if swap:
  extent = [y.min(), y.max(), x.max(), x.min()]
else:
  extent = [x.min(), x.max(), y.max(), y.min()]

if 0:
  fh = plt.figure()
  axes = [fh.add_subplot(121), fh.add_subplot(122)]
  axes[0].imshow(z.T,extent=extent)
  axes[1].imshow(z.T,extent=extent)
  for i in range(len(xc)/2):
    axes[1].plot([xc[2*i], xc[2*i+1]],
                 [yc[2*i], yc[2*i+1]], 'k')

segs = {}

nLevels = len(levels)

start = timer()

# Convert to (x,y)-pairs again
iCoordinate = 0
for iLevel in range(nLevels):
  nSegments = nnSegments[iLevel]
  # Two point per segment
  for iSegment in range(nSegments):
    segs.setdefault(levels[iLevel], []).append( ((xc[iCoordinate], yc[iCoordinate]),
                                                 (xc[iCoordinate+1], yc[iCoordinate+1])))
    iCoordinate = iCoordinate + 2

# Sort points to form polygons
polygons = {}
for level, segments in segs.items():
  polygons[level] = sort_segments(segments)

xp = []
yp = []

# Create list of x- and y-coordinates
for level, polys in polygons.items():
  for p in polys:
    xp.append(np.array(p)[:,0])
    yp.append(np.array(p)[:,1])

elapsed = timer() - start
print(elapsed)

fh = plt.figure()
axes = [fh.add_subplot(121), fh.add_subplot(122)]

axes[0].imshow(z.T,extent=extent)
axes[1].imshow(z.T,extent=extent)

for i in range(len(xp)):
  axes[1].plot(xp[i],yp[i], 'k')

if swap:
  i = y.flatten()
  j = x.flatten()
else:
  i = x.flatten()
  j = y.flatten()

start = timer()
retval, yc, xc, nnCoordinates, nnContours = swig_contour.contours_sorted(z, i, j, levels.flatten())
elapsed = timer() - start
print(elapsed)

fh = plt.figure()
axes = [fh.add_subplot(121), fh.add_subplot(122)]
axes[0].imshow(z.T,extent=extent)
axes[1].imshow(z.T,extent=extent)

iStart = 0
iContour = 0
for iContour in range(len(nnCoordinates)):
  nPoints = int(nnCoordinates[iContour])
  iLevel = np.where(iContour < np.cumsum(nnContours))[0][0]
  lw = 1.5;
#  lw = iLevel+1
  axes[1].plot(yc[iStart:iStart+nPoints],
               xc[iStart:iStart+nPoints], 'k', linewidth=lw)
  iStart = iStart + nPoints

sys.exit(0)
nx = 100
nz = 100
dx = 6e-5
dz = 4e-4
offset_x = 0
offset_z = 60

x = (np.r_[0:nx] - (nx-1)/2.0 + offset_x)*dx
z = (np.r_[0:nz] - (nz-1)/2.0 + offset_z)*dz

#[z,x] = np.meshgrid(z, x, indexing='ij')
hmm = np.fromfile('lort.npz')
hmm = hmm.reshape((nz,nx)).T
levels = np.r_[-24:3:3].astype(np.float64)
retval, yc, xc, nnCoordinates, nnContours = swig_contour.contours_sorted(hmm, z, x, levels)

plt.figure()

iStart = 0
iContour = 0
for iContour in range(len(nnCoordinates)):
  nPoints = int(nnCoordinates[iContour])
  iLevel = np.where(iContour < np.cumsum(nnContours))[0][0]
  lw = 1.5;
#  lw = iLevel+1
  plt.plot(yc[iStart:iStart+nPoints],
           xc[iStart:iStart+nPoints], 'k', linewidth=lw)
  iStart = iStart + nPoints

retval, xc, yc, nnSegments = swig_contour.contours(hmm, z, x, levels)

if 1:
  extent = [x.min(), x.max(), z.max(), z.min()]
  fh = plt.figure()
  axes = [fh.add_subplot(121), fh.add_subplot(122)]
  axes[0].imshow(hmm, extent=extent,aspect='auto')
  axes[1].imshow(hmm ,extent=extent,aspect='auto')
  for i in range(len(xc)/2):
    axes[1].plot([yc[2*i], yc[2*i+1]],
                 [xc[2*i], xc[2*i+1]], 'k')
