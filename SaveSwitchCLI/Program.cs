using System;
using System.IO;
using DiscUtils.Partitions;
using SaveSwitchCore;

namespace SaveSwitchCLI
{
    internal static class Program
    {
        private static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");

            var ss = new SaveSwitch();
            ss.AutoMount();
        }
    }
}