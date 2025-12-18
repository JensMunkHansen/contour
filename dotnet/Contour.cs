using System;
using System.Runtime.InteropServices;

namespace Contour
{
    /// <summary>
    /// P/Invoke wrapper for the contour native library.
    /// Computes contour lines from 2D gridded data using Paul Bourke's CONREC algorithm.
    /// </summary>
    public static class ContourNative
    {
        private const string LibraryName = "contour";

        /// <summary>
        /// Free memory allocated by contour functions.
        /// </summary>
        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void contour_free(IntPtr ptr);

        /// <summary>
        /// Compute contours for a 2D double-precision floating point image.
        /// </summary>
        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int contour_compute(
            [In] double[] pData, nuint nYdata, nuint nXdata,
            [In] double[] pY, nuint nY,
            [In] double[] pX, nuint nX,
            [In] double[] pLevels, nuint nLevels,
            out IntPtr ppOutY, out nuint nOutY,
            out IntPtr ppOutX, out nuint nOutX,
            out IntPtr nOutLengths, out nuint nOutSegments);

        /// <summary>
        /// Compute sorted contours (connected polygons) for a 2D double-precision floating point image.
        /// </summary>
        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int contour_compute_sorted(
            [In] double[] pData, nuint nYdata, nuint nXdata,
            [In] double[] pY, nuint nY,
            [In] double[] pX, nuint nX,
            [In] double[] pLevels, nuint nLevels,
            out IntPtr ppOutY, out nuint nOutY,
            out IntPtr ppOutX, out nuint nOutX,
            out IntPtr nOutLengths, out nuint nOutSegments,
            out IntPtr nLevelSegments, out nuint nLevels2);
    }

    /// <summary>
    /// High-level managed wrapper for contour computation with automatic memory management.
    /// </summary>
    public static class ContourCompute
    {
        /// <summary>
        /// Result of contour computation.
        /// </summary>
        public class ContourResult
        {
            /// <summary>X-coordinates of all contour points.</summary>
            public double[] X { get; set; } = Array.Empty<double>();
            /// <summary>Y-coordinates of all contour points.</summary>
            public double[] Y { get; set; } = Array.Empty<double>();
            /// <summary>Number of points (or segments) per level.</summary>
            public nuint[] SegmentLengths { get; set; } = Array.Empty<nuint>();
        }

        /// <summary>
        /// Result of sorted contour computation.
        /// </summary>
        public class SortedContourResult : ContourResult
        {
            /// <summary>Number of polygons per level.</summary>
            public nuint[] LevelSegments { get; set; } = Array.Empty<nuint>();
        }

        /// <summary>
        /// Compute contours for 2D data.
        /// </summary>
        /// <param name="data">2D data array (row-major, dimensions [nY, nX])</param>
        /// <param name="y">Y-coordinates array</param>
        /// <param name="x">X-coordinates array</param>
        /// <param name="levels">Contour levels (must be increasing)</param>
        /// <returns>Contour result with coordinates and segment information</returns>
        public static ContourResult Compute(double[,] data, double[] y, double[] x, double[] levels)
        {
            int nY = data.GetLength(0);
            int nX = data.GetLength(1);

            // Flatten 2D array to 1D (row-major)
            double[] flatData = new double[nY * nX];
            Buffer.BlockCopy(data, 0, flatData, 0, nY * nX * sizeof(double));

            int result = ContourNative.contour_compute(
                flatData, (nuint)nY, (nuint)nX,
                y, (nuint)y.Length,
                x, (nuint)x.Length,
                levels, (nuint)levels.Length,
                out IntPtr pOutY, out nuint nOutY,
                out IntPtr pOutX, out nuint nOutX,
                out IntPtr pLengths, out nuint nSegments);

            if (result != 0)
                throw new InvalidOperationException("Contour computation failed");

            try
            {
                var contourResult = new ContourResult
                {
                    X = new double[(int)nOutX],
                    Y = new double[(int)nOutY],
                    SegmentLengths = new nuint[(int)nSegments]
                };

                Marshal.Copy(pOutX, contourResult.X, 0, (int)nOutX);
                Marshal.Copy(pOutY, contourResult.Y, 0, (int)nOutY);

                // Copy segment lengths (nuint array)
                for (int i = 0; i < (int)nSegments; i++)
                {
                    contourResult.SegmentLengths[i] = (nuint)Marshal.ReadIntPtr(pLengths, i * IntPtr.Size);
                }

                return contourResult;
            }
            finally
            {
                ContourNative.contour_free(pOutX);
                ContourNative.contour_free(pOutY);
                ContourNative.contour_free(pLengths);
            }
        }

        /// <summary>
        /// Compute sorted contours (connected polygons) for 2D data.
        /// </summary>
        /// <param name="data">2D data array (row-major, dimensions [nY, nX])</param>
        /// <param name="y">Y-coordinates array</param>
        /// <param name="x">X-coordinates array</param>
        /// <param name="levels">Contour levels (must be increasing)</param>
        /// <returns>Sorted contour result with coordinates, segment lengths, and level information</returns>
        public static SortedContourResult ComputeSorted(double[,] data, double[] y, double[] x, double[] levels)
        {
            int nY = data.GetLength(0);
            int nX = data.GetLength(1);

            // Flatten 2D array to 1D (row-major)
            double[] flatData = new double[nY * nX];
            Buffer.BlockCopy(data, 0, flatData, 0, nY * nX * sizeof(double));

            int result = ContourNative.contour_compute_sorted(
                flatData, (nuint)nY, (nuint)nX,
                y, (nuint)y.Length,
                x, (nuint)x.Length,
                levels, (nuint)levels.Length,
                out IntPtr pOutY, out nuint nOutY,
                out IntPtr pOutX, out nuint nOutX,
                out IntPtr pLengths, out nuint nSegments,
                out IntPtr pLevelSegments, out nuint nLevels);

            if (result != 0)
                throw new InvalidOperationException("Contour computation failed");

            try
            {
                var contourResult = new SortedContourResult
                {
                    X = new double[(int)nOutX],
                    Y = new double[(int)nOutY],
                    SegmentLengths = new nuint[(int)nSegments],
                    LevelSegments = new nuint[(int)nLevels]
                };

                Marshal.Copy(pOutX, contourResult.X, 0, (int)nOutX);
                Marshal.Copy(pOutY, contourResult.Y, 0, (int)nOutY);

                // Copy segment lengths (nuint array)
                for (int i = 0; i < (int)nSegments; i++)
                {
                    contourResult.SegmentLengths[i] = (nuint)Marshal.ReadIntPtr(pLengths, i * IntPtr.Size);
                }

                // Copy level segments (nuint array)
                for (int i = 0; i < (int)nLevels; i++)
                {
                    contourResult.LevelSegments[i] = (nuint)Marshal.ReadIntPtr(pLevelSegments, i * IntPtr.Size);
                }

                return contourResult;
            }
            finally
            {
                ContourNative.contour_free(pOutX);
                ContourNative.contour_free(pOutY);
                ContourNative.contour_free(pLengths);
                ContourNative.contour_free(pLevelSegments);
            }
        }
    }
}
