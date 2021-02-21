using System;
using System.Collections.Generic;

namespace MDFSplitter
{
    class Program
    {
        static void Main(string[] args)
        {
            byte [] rb = System.IO.File.ReadAllBytes("E:\\SteamLibrary\\steamapps\\common\\Monaco\\Mag\\Worlds_win32.mdf");
            List<int> headers = new List<int>();

            for(int i = 0; i < rb.Length - 4; ++i)
            {
                if(
                    rb[i + 0] == 137 && 
                    rb[i + 1] == 'P' && 
                    rb[i + 2] == 'N' && 
                    rb[i + 3] == 'G')
                {
                    headers.Add(i);
                }
            }
            headers.Add(rb.Length);

            for(int i = 0; i < headers.Count - 1; ++i)
            { 
                string file = $"Extracted_{i.ToString("000")}.png";

                int size = headers[i + 1] - headers[i];
                byte [] contents = new byte[size];
                System.Array.Copy(rb, headers[i], contents, 0, size);

                System.IO.File.WriteAllBytes($"E:\\SteamLibrary\\steamapps\\common\\Monaco\\Mag\\{file}", contents);

                //Console.Write(headers[i]);
                //Console.WriteLine();
            }
        }
    }
}
