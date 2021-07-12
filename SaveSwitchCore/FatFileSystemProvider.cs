using System.IO;
using DiscUtils.Fat;
using LibHac;
using LibHac.Common;
using LibHac.Fs;
using LibHac.Fs.Fsa;
using LibHac.FsSystem;

namespace SaveSwitchCore
{
    public class FatFileSystemProvider : IAttributeFileSystem
    {
        private readonly FatFileSystem _fs;

        public FatFileSystemProvider(FatFileSystem fs)
        {
            _fs = fs;
        }

        protected override Result DoCreateFile(U8Span path, long size, CreateFileOptions option)
        {
            if (!_fs.DirectoryExists(PathTools.GetParentDirectory(FixPath(path))))
                return ResultFs.PathNotFound.Log();

            if (_fs.Exists(FixPath(path)))
                return ResultFs.PathAlreadyExists.Log();

            if (_fs.AvailableSpace < size)
                return ResultFs.UsableSpaceNotEnough.Log();

            using var stream = _fs.OpenFile(FixPath(path), FileMode.Create, FileAccess.Write);
            stream.SetLength(size);

            return Result.Success;
        }

        protected override Result DoDeleteFile(U8Span path)
        {
            if (!_fs.FileExists(FixPath(path)))
                return ResultFs.PathNotFound.Log();

            _fs.DeleteFile(FixPath(path));
            return Result.Success;
        }

        protected override Result DoCreateDirectory(U8Span path)
        {
            if (!_fs.DirectoryExists(PathTools.GetParentDirectory(FixPath(path))))
                return ResultFs.PathNotFound.Log();

            if (_fs.Exists(FixPath(path)))
                return ResultFs.PathAlreadyExists.Log();

            _fs.CreateDirectory(FixPath(path));
            return Result.Success;
        }

        protected override Result DoDeleteDirectory(U8Span path)
        {
            if (!_fs.DirectoryExists(FixPath(path)))
                return ResultFs.PathNotFound.Log();

            if (_fs.GetFiles(FixPath(path)).Length > 0)
                return ResultFs.DirectoryNotEmpty.Log();

            _fs.DeleteDirectory(FixPath(path), false);
            return Result.Success;
        }

        protected override Result DoDeleteDirectoryRecursively(U8Span path)
        {
            if (!_fs.DirectoryExists(FixPath(path)))
                return ResultFs.PathNotFound.Log();

            _fs.DeleteDirectory(FixPath(path), true);
            return Result.Success;
        }

        protected override Result DoCleanDirectoryRecursively(U8Span path)
        {
            if (!_fs.DirectoryExists(FixPath(path)))
                return ResultFs.PathNotFound.Log();

            foreach (var dir in _fs.GetDirectories(FixPath(path)))
                _fs.DeleteDirectory(dir, true);
            foreach (var file in _fs.GetFiles(FixPath(path)))
                _fs.DeleteFile(file);
            return Result.Success;
        }

        protected override Result DoRenameFile(U8Span oldPath, U8Span newPath)
        {
            if (!_fs.FileExists(FixPath(oldPath)))
                return ResultFs.PathNotFound.Log();

            if (!_fs.DirectoryExists(PathTools.GetParentDirectory(FixPath(newPath))))
                return ResultFs.PathNotFound.Log();

            if (_fs.Exists(FixPath(newPath)))
                return ResultFs.PathAlreadyExists.Log();

            _fs.MoveFile(FixPath(oldPath), FixPath(newPath), false);
            return Result.Success;
        }

        protected override Result DoRenameDirectory(U8Span oldPath, U8Span newPath)
        {
            if (!_fs.DirectoryExists(FixPath(oldPath)))
                return ResultFs.PathNotFound.Log();

            if (!_fs.DirectoryExists(PathTools.GetParentDirectory(FixPath(newPath))))
                return ResultFs.PathNotFound.Log();

            if (_fs.Exists(FixPath(newPath)))
                return ResultFs.PathAlreadyExists.Log();

            if (PathUtility.IsSubPath(FixPath(oldPath).ToU8Span(), FixPath(newPath).ToU8Span()))
                return ResultFs.DirectoryNotRenamable.Log();

            _fs.MoveFile(FixPath(oldPath), FixPath(newPath), false);
            return Result.Success;
        }

        protected override Result DoGetEntryType(out DirectoryEntryType entryType, U8Span path)
        {
            entryType = DirectoryEntryType.File;

            if (_fs.FileExists(FixPath(path)))
                entryType = DirectoryEntryType.File;
            else if (_fs.DirectoryExists(FixPath(path)))
                entryType = DirectoryEntryType.Directory;
            else
                return ResultFs.PathNotFound.Value;

            return Result.Success;
        }

        protected override Result DoOpenFile(out IFile file, U8Span path, OpenMode mode)
        {
            file = null;
            if (!_fs.FileExists(FixPath(path)))
                return ResultFs.PathNotFound.Log();

            Stream stream = _fs.OpenFile(FixPath(path), FileMode.Open, GetFileAccess(mode));
            file = stream.AsIFile(mode);
            return Result.Success;
        }

        protected override Result DoOpenDirectory(out IDirectory directory, U8Span path, OpenDirectoryMode mode)
        {
            directory = null;
            if (!_fs.DirectoryExists(FixPath(path)))
                return ResultFs.PathNotFound.Log();

            directory = new FatFileSystemDirectory(_fs, FixPath(path), mode);
            return Result.Success;
        }

        protected override Result DoCommit()
        {
            return Result.Success;
        }

        protected override Result DoCreateDirectory(U8Span path, NxFileAttributes archiveAttribute)
        {
            if (!_fs.DirectoryExists(PathTools.GetParentDirectory(FixPath(path))))
                return ResultFs.PathNotFound.Log();

            if (_fs.Exists(FixPath(path)))
                return ResultFs.PathAlreadyExists.Log();

            _fs.CreateDirectory(FixPath(path));
            return Result.Success;
        }

        protected override Result DoGetFileAttributes(out NxFileAttributes attributes, U8Span path)
        {
            attributes = NxFileAttributes.None;
            if (!_fs.Exists(FixPath(path)))
                return ResultFs.PathNotFound.Log();

            attributes = _fs.GetAttributes(FixPath(path)).ToNxAttributes();
            return Result.Success;
        }

        protected override Result DoSetFileAttributes(U8Span path, NxFileAttributes attributes)
        {
            if (!_fs.Exists(FixPath(path)))
                return ResultFs.PathNotFound.Log();

            var oldAttributes = _fs.GetAttributes(FixPath(path));
            var newAttributes = oldAttributes.ApplyNxAttributes(attributes);

            _fs.SetAttributes(FixPath(path), newAttributes);
            return Result.Success;
        }

        protected override Result DoGetFileSize(out long fileSize, U8Span path)
        {
            fileSize = 0;
            if (!_fs.FileExists(FixPath(path)))
                return ResultFs.PathNotFound.Log();

            fileSize = _fs.GetFileLength(FixPath(path));
            return Result.Success;
        }

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);
            if (disposing)
                _fs.Dispose();
        }

        private static FileAccess GetFileAccess(OpenMode mode)
        {
            // FileAccess and OpenMode have the same flags
            return (FileAccess) (mode & OpenMode.ReadWrite);
        }

        private static string FixPath(U8Span path)
        {
            return PathTools.Normalize(path.ToString()).Replace("/", @"\");
        }
    }
}