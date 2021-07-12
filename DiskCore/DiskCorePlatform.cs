using System;
using DiskCore.DiskCoreCommon;
using DiskCore.DiskCoreWin;

namespace DiskCore
{
    internal static class DiskCorePlatform
    {
        private static IDiskCorePlatform _platform;

        public static IDiskCorePlatform GetPlatform()
        {
            if (_platform != null)
                return _platform;

            if (OperatingSystem.IsWindows())
                return _platform = new DiskCorePlatformWin();

            throw new NotSupportedException("This platform is currently not supported by DiskCore");
        }
    }
}