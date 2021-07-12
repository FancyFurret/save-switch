using System.Linq;
using System.Management;
using System.Runtime.Versioning;
using DiskCore.DiskCoreCommon;

namespace DiskCore.DiskCoreWin
{
    [SupportedOSPlatform("windows")]
    public class PartitionInfoWin : IPartitionInfo
    {
        public PartitionInfo[] GetPartitions(DiskInfo disk)
        {
            return new ManagementClass("Win32_DiskPartition").GetInstances()
                .OfType<ManagementObject>()
                .Select(obj => new PartitionInfo
                {
                    Id = (string) obj.GetPropertyValue("DeviceID"),
                    Name = (string) obj.GetPropertyValue("Name"),
                    Description = (string) obj.GetPropertyValue("Description"),
                    Index = (uint) obj.GetPropertyValue("Index"),
                    DiskIndex = (uint) obj.GetPropertyValue("DiskIndex"),
                    Size = (ulong) obj.GetPropertyValue("Size"),
                    StartingOffset = (ulong) obj.GetPropertyValue("StartingOffset"),
                    BlockSize = (ulong) obj.GetPropertyValue("BlockSize")
                })
                .Where(p => p.DiskIndex == disk.Index)
                .OrderBy(p => p.Index).ToArray();
        }
    }
}