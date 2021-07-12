using System.Linq;
using System.Management;
using System.Runtime.Versioning;
using DiskCore.DiskCoreCommon;

namespace DiskCore.DiskCoreWin
{
    [SupportedOSPlatform("windows")]
    public class DiskInfoWin : IDiskInfo
    {
        public DiskInfo[] GetDisks()
        {
            return new ManagementClass("Win32_DiskDrive").GetInstances()
                .OfType<ManagementObject>()
                .Select(obj => new DiskInfo
                {
                    Id = (string) obj.GetPropertyValue("DeviceID"),
                    Name = (string) obj.GetPropertyValue("Name"),
                    Caption = (string) obj.GetPropertyValue("Caption"),
                    Model = (string) obj.GetPropertyValue("Model"),
                    Index = (uint) obj.GetPropertyValue("Index"),
                    Partitions = (uint) obj.GetPropertyValue("Partitions"),
                    Size = (ulong) obj.GetPropertyValue("Size"),
                    SectorSize = (uint) obj.GetPropertyValue("BytesPerSector")
                }).ToArray();
        }
    }
}