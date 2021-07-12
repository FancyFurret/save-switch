namespace DiskCore.DiskCoreCommon
{
    public interface IPartitionInfo
    {
        public PartitionInfo[] GetPartitions(DiskInfo disk);
    }
}