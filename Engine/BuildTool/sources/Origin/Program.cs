using System;
using System.Threading;

namespace Origin.BuildTool
{
    public class Program
    {
        public static void Main(String[] args)
        {
            while (true)
            {
                Console.WriteLine("Hello from console");
                Thread.Sleep(100);
            }
        }
    }
}
