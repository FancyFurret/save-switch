using System.IO;
using DiskCore.DiskCoreCommon;

namespace DiskCore
{
    public class DiskStream : Stream
    {
        private readonly IDiskStream _diskStream;
        private long _length;

        public DiskStream(DiskInfo disk, long length = -1)
        {
            _diskStream = DiskCorePlatform.GetPlatform().GetDiskStream(disk);
            _length = length == -1 ? (long) disk.Size : length;
        }

        public override void Flush() => _diskStream.Flush();
        public override int Read(byte[] buffer, int offset, int count) => _diskStream.Read(buffer, offset, count);
        public override long Seek(long offset, SeekOrigin origin) => _diskStream.Seek(offset, origin);
        public override void SetLength(long value) => _length = value;
        public override void Write(byte[] buffer, int offset, int count) => _diskStream.Write(buffer, offset, count);

        public override bool CanRead => _diskStream.CanRead;
        public override bool CanSeek => _diskStream.CanSeek;
        public override bool CanWrite => _diskStream.CanWrite;
        public override long Length => _length;

        public override long Position
        {
            get => _diskStream.Position;
            set => _diskStream.Position = value;
        }

        protected override void Dispose(bool disposing)
        {
            _diskStream.Dispose(disposing);
            base.Dispose(disposing);
        }
    }
}