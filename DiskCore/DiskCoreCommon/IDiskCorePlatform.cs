namespace DiskCore.DiskCoreCommon
{
    internal interface IDiskCorePlatform
    {
        public IDiskInfo DiskInfo { get; }
        public IPartitionInfo PartitionInfo { get; }
        
        public IDiskStream GetDiskStream(DiskInfo disk);
    }
}