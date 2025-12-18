using System;
using Contour;

class Program
{
    static int Main()
    {
        // Simple 3x3 test data with a peak in the center
        double[,] data = {
            { 0.0, 0.0, 0.0 },
            { 0.0, 1.0, 0.0 },
            { 0.0, 0.0, 0.0 }
        };

        double[] y = { 0.0, 1.0, 2.0 };
        double[] x = { 0.0, 1.0, 2.0 };
        double[] levels = { 0.5 };

        try
        {
            var result = ContourCompute.Compute(data, y, x, levels);

            Console.WriteLine($"Contour computation succeeded");
            Console.WriteLine($"  Points: {result.X.Length}");
            Console.WriteLine($"  Segments per level: {result.SegmentLengths.Length}");

            if (result.X.Length > 0)
            {
                Console.WriteLine("  First few points:");
                for (int i = 0; i < Math.Min(4, result.X.Length); i++)
                {
                    Console.WriteLine($"    ({result.X[i]:F2}, {result.Y[i]:F2})");
                }
            }

            return 0;
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error: {ex.Message}");
            return 1;
        }
    }
}
