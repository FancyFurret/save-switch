namespace DiskCore
{
    public record PartitionInfo
    {
        public string Id { get; init; }
        public string Name { get; init; }
        public string Description { get; init; }
        public uint Index { get; init; }
        public uint DiskIndex { get; init; }
        public ulong Size { get; init; }
        public ulong StartingOffset { get; init; }
        public ulong BlockSize { get; init; }

        public static PartitionInfo[] GetPartitions(DiskInfo disk)
        {
            return DiskCorePlatform.GetPlatform().PartitionInfo.GetPartitions(disk);
        }
    }
}