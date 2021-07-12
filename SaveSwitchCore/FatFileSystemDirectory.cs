using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using DiscUtils;
using DiscUtils.Fat;
using LibHac;
using LibHac.Fs;
using LibHac.Fs.Fsa;
using LibHac.FsSystem;
using LibHac.Util;

namespace SaveSwitchCore
{
    public class FatFileSystemDirectory : IDirectory
    {
        private readonly OpenDirectoryMode _mode;
        private readonly DiscDirectoryInfo _directoryInfo;
        private readonly IEnumerator<DiscFileSystemInfo> _enumerator;

        public FatFileSystemDirectory(FatFileSystem fs, string path, OpenDirectoryMode mode)
        {
            _mode = mode;
            _directoryInfo = fs.GetDirectoryInfo(path);
            _enumerator = _directoryInfo.GetFileSystemInfos().AsEnumerable().GetEnumerator();
        }

        protected override Result DoRead(out long entriesRead, Span<DirectoryEntry> entryBuffer)
        {
            entriesRead = 0;
            while (entriesRead < entryBuffer.Length && _enumerator.MoveNext())
            {
                var entry = _enumerator.Current;
                var isDir = (entry.Attributes & FileAttributes.Directory) != 0;
                if (!CanReturnEntry(isDir, _mode))
                    continue;

                var type = isDir ? DirectoryEntryType.Directory : DirectoryEntryType.File;
                var length = isDir ? 0 : entry.FileSystem.GetFileLength(entry.FullName);

                StringUtils.Copy(entryBuffer[(int) entriesRead].Name, StringUtils.StringToUtf8(entry.Name));
                entryBuffer[(int) entriesRead].Name[0x300] = 0;
                entryBuffer[(int) entriesRead].Attributes = entry.Attributes.ToNxAttributes();
                entryBuffer[(int) entriesRead].Size = length;
                entryBuffer[(int) entriesRead].Type = type;

                entriesRead++;
            }

            return Result.Success;
        }

        protected override Result DoGetEntryCount(out long entryCount)
        {
            var count = _directoryInfo.GetFileSystemInfos().Select(entry => (entry.Attributes & FileAttributes.Directory) != 0).Count(isDir => CanReturnEntry(isDir, _mode));

            entryCount = count;
            return Result.Success;
        }

        private static bool CanReturnEntry(bool isDir, OpenDirectoryMode mode)
        {
            return isDir && (mode & OpenDirectoryMode.Directory) != 0 ||
                   !isDir && (mode & OpenDirectoryMode.File) != 0;
        }
    }
}