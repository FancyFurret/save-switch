using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using DiscUtils;
using DiscUtils.Fat;
using DiscUtils.Partitions;
using DiscUtils.Streams;
using DiskCore;
using LibHac.Common;
using LibHac.Common.Keys;
using LibHac.Fs;
using LibHac.FsSystem;
using LibHac.FsSystem.Save;
using PartitionInfo = DiskCore.PartitionInfo;

namespace SaveSwitchCore
{
    public class SwitchNand : IDisposable
    {
        private KeySet _keys;
        private IStorage _nandStorage;
        private FatFileSystem _nandUser;
        private SubdirectoryFileSystem _saveFs;

        public SwitchNand(KeySet keys, DiskInfo disk)
        {
            // Windows ignores GPT backup, so add that in to get the correct length
            var partitions = PartitionInfo.GetPartitions(disk);
            var lastPartition = partitions.Last();
            var length = (lastPartition.Size + lastPartition.StartingOffset) + 0x200;
            length += (0x747BFFE00 - length) + 0x200;

            Stream switchStream = new DiskStream(disk, (long) length);
            Load(keys, new CachedStorage(
                new StreamStorage(switchStream, false),
                (int) (disk.SectorSize * 100),
                4, false));
        }

        private void Load(KeySet keys, IStorage nandStorage)
        {
            _keys = keys;
            _nandStorage = nandStorage;

            // Get all partitions on switch nand
            var gpt = new GuidPartitionTable(_nandStorage.AsStream(FileAccess.Read), Geometry.Null);

            // Find USER partition
            var user = gpt.Partitions.FirstOrDefault(p => ((GuidPartitionInfo) p).Name == "USER");
            if (user == null)
                throw new SaveSwitchException("Could not find USER partition!");

            // Decrypt USER partition
            var encStorage = user.Open().AsStorage();
            var decStorage = new CachedStorage(
                new Aes128XtsStorage(encStorage, keys.BisKeys[3], 0x4000, false),
                0x4000, 4, false);

            // Create FAT fs around decrypted USER partition
            _nandUser = new FatFileSystem(decStorage.AsStream(FileAccess.Read), Ownership.Dispose);

            // Create Save Directory fs
            var concatFs = new ConcatenationFileSystem(new FatFileSystemProvider(_nandUser));
            if (!concatFs.DirectoryExists("/save"))
                throw new SaveSwitchException("USER partition does not contain a 'save' directory!");
            SubdirectoryFileSystem.CreateNew(out _saveFs, concatFs, "/save".ToU8String()).ThrowIfFailure();
        }

        public IEnumerable<SaveDataFileSystem> GetSaves()
        {
            foreach (var fileEntry in _saveFs.EnumerateEntries().Where(x => x.Type == DirectoryEntryType.File))
            {
                SaveDataFileSystem save = null;

                try
                {
                    _saveFs.OpenFile(out var file, fileEntry.FullPath.ToU8Span(), OpenMode.Read).ThrowIfFailure();
                    save = new SaveDataFileSystem(_keys, file.AsStorage(), IntegrityCheckLevel.None, true);
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"{ex.Message} File: {fileEntry.FullPath}");
                }

                if (save != null)
                    yield return save;
            }
        }

        public void Dispose()
        {
            _nandUser.Dispose();
            _nandStorage.Dispose();
        }
    }
}