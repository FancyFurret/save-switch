using System.IO;

namespace DiskCore.DiskCoreCommon
{
    public interface IDiskStream
    {
        public void Flush();
        public int Read(byte[] buffer, int offset, int count);
        public long Seek(long offset, SeekOrigin origin);
        public void Write(byte[] buffer, int offset, int count);

        public bool CanRead { get; }
        public bool CanSeek { get; }
        public bool CanWrite { get; }
        public long Position { get; set; }

        public void Dispose(bool disposing);
    }
}