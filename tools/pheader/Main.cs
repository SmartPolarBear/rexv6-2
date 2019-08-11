/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-08-11 13:50:25
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-08-11 14:06:34
 * @ Description: 
    A tool to add protection macro and extern "C"s to headers.
    Usage : dotnet run $(pwd)/../../ $(find ../../include/xv6)
 */


using System;
using System.Linq;
using System.IO;
using System.Collections.Generic;

namespace protect_header
{
    public static class PathHelper
    {
        /// <summary>
        /// Creates a relative path from one file or folder to another.
        /// </summary>
        /// <param name="fromPath">Contains the directory that defines the start of the relative path.</param>
        /// <param name="toPath">Contains the path that defines the endpoint of the relative path.</param>
        /// <returns>The relative path from the start directory to the end path.</returns>
        /// <exception cref="ArgumentNullException"><paramref name="fromPath"/> or <paramref name="toPath"/> is <c>null</c>.</exception>
        /// <exception cref="UriFormatException"></exception>
        /// <exception cref="InvalidOperationException"></exception>
        public static string GetRelativePath(string fromPath, string toPath)
        {
            if (string.IsNullOrEmpty(fromPath))
            {
                throw new ArgumentNullException("fromPath");
            }

            if (string.IsNullOrEmpty(toPath))
            {
                throw new ArgumentNullException("toPath");
            }

            Uri fromUri = new Uri(AppendDirectorySeparatorChar(fromPath));
            Uri toUri = new Uri(AppendDirectorySeparatorChar(toPath));

            if (fromUri.Scheme != toUri.Scheme)
            {
                return toPath;
            }

            Uri relativeUri = fromUri.MakeRelativeUri(toUri);
            string relativePath = Uri.UnescapeDataString(relativeUri.ToString());

            if (string.Equals(toUri.Scheme, Uri.UriSchemeFile, StringComparison.OrdinalIgnoreCase))
            {
                relativePath = relativePath.Replace(Path.AltDirectorySeparatorChar, Path.DirectorySeparatorChar);
            }

            return relativePath;
        }

        private static string AppendDirectorySeparatorChar(string path)
        {
            // Append a slash only if the path is a directory and does not have a slash.
            if (!Path.HasExtension(path) &&
                !path.EndsWith(Path.DirectorySeparatorChar.ToString()))
            {
                return path + Path.DirectorySeparatorChar;
            }

            return path;
        }
    }

    class Program
    {

        static string GetProtectHeaderMacro(string pathname)
        {
            return $"__{pathname.Replace('.', '_').Replace('/', '_').ToUpper()}";
        }

        static void Main(string[] args)
        {
            if(args.Count()==0)
            {
                Console.WriteLine("Usage : dotnet run $(pwd)/../../ $(find ../../include/xv6)");
                return;
            }

            string topd = Path.GetFullPath(args[0]), cwd = Directory.GetCurrentDirectory();
            Console.WriteLine($"Working dir:{topd}");
            for (int i = 1; i < args.Count(); i++)
            {
                var arg = args[i];
                arg = Path.GetFullPath(Path.Combine(cwd, arg));

                Console.WriteLine("Adding protection header for :");
                Console.Write(arg);
                Console.WriteLine(string.Empty);
                if (!File.Exists(Path.GetFullPath(Path.Combine(cwd, arg))))
                {
                    Console.WriteLine($"File {arg} does not exist.");
                    continue;
                }

                var relativePath = PathHelper.GetRelativePath(topd, arg);
                string protectionHeader = GetProtectHeaderMacro(relativePath);
                List<string> allLines = new List<string>();
                allLines.Add($"#if !defined({protectionHeader})");
                allLines.Add($"#define {protectionHeader}");
                allLines.Add($"#if defined(__cplusplus)");
                allLines.Add($"extern \"C\"");
                allLines.Add("{");
                allLines.Add($"#endif");
                allLines.AddRange(File.ReadAllLines(arg));
                allLines.Add($"#if defined(__cplusplus)");
                allLines.Add("}");
                allLines.Add($"#endif");
                allLines.Add($"#endif // {protectionHeader}");

                File.WriteAllLines(arg, allLines.ToArray());
            }
        }
    }
}
