using System.Runtime.Versioning;
using DiskCore.DiskCoreCommon;

namespace DiskCore.DiskCoreWin
{
    [SupportedOSPlatform("windows")]
    public class DiskCorePlatformWin : IDiskCorePlatform
    {
        public IDiskInfo DiskInfo { get; } = new DiskInfoWin();
        public IPartitionInfo PartitionInfo { get; } = new PartitionInfoWin();

        public IDiskStream GetDiskStream(DiskInfo disk) => new DiskStreamWin(disk);
    }
}