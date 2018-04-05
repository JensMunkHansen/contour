import sys
import os
import glob

# Improved version
filedir = os.path.dirname(os.path.realpath(__file__))
cwd     = os.getcwd()

# Go one levels up (if possible)
os.chdir(filedir)
iDir = 1
while (iDir > 0):
    os.chdir('..')
    iDir = iDir - 1;

matches = []

for i in range(3):
  if os.name == 'nt':
    paths = [i*'*/' + 'contour/Release',i*'*/' + 'contour/Debug', i*'*/' + 'contour']
  else:
    paths = [i*'*/' + 'contour', i*'*/' + 'release/contour', i*'*/' + 'debug/contour']
  for path in paths:
    entries = glob.glob(path)
    if len(entries) > 0:
      path = os.path.join(os.getcwd(),entries[0])
      matches.append(path)

for path in matches:
  sys.path.append( path )

os.chdir(cwd)
