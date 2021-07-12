namespace DiskCore
{
    public record DiskInfo
    {
        public string Id { get; init; }
        public string Name { get; init; }
        public string Caption { get; init; }
        public string Model { get; init; }
        public uint Index { get; init; }
        public uint Partitions { get; init; }
        public ulong Size { get; init; }
        public uint SectorSize { get; init; }

        public static DiskInfo[] GetDisks()
        {
            return DiskCorePlatform.GetPlatform().DiskInfo.GetDisks();
        }
    }
}