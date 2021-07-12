using System;
using System.IO;
using System.Runtime.InteropServices;
using DiskCore.DiskCoreCommon;
using Microsoft.Win32.SafeHandles;

namespace DiskCore.DiskCoreWin
{
    public class DiskStreamWin : IDiskStream
    {
        private SafeFileHandle _handle;
        private readonly FileStream _stream;

        public DiskStreamWin(DiskInfo disk)
        {
            var file = NativeWin.CreateFile(
                disk.Name,
                FileAccess.ReadWrite,
                0,
                IntPtr.Zero,
                FileMode.Open,
                0,
                IntPtr.Zero);

            _handle = new SafeFileHandle(file, true);
            if (_handle.IsInvalid)
                Marshal.ThrowExceptionForHR(Marshal.GetHRForLastWin32Error());

            _stream = new FileStream(_handle, FileAccess.ReadWrite);

            if (_handle.IsInvalid)
                Marshal.ThrowExceptionForHR(Marshal.GetHRForLastWin32Error());
        }

        public void Flush() => _stream.Flush();
        public int Read(byte[] buffer, int offset, int count) => _stream.Read(buffer, offset, count);
        public long Seek(long offset, SeekOrigin origin) => _stream.Seek(offset, origin);
        public void Write(byte[] buffer, int offset, int count) => throw new NotSupportedException();

        public bool CanRead => _stream.CanRead;
        public bool CanSeek => _stream.CanSeek;
        public bool CanWrite => false;

        public long Position
        {
            get => _stream.Position;
            set => _stream.Position = value;
        }

        void IDiskStream.Dispose(bool disposing)
        {
            if (disposing)
            {
                if (_handle != null)
                {
                    _stream.Dispose();
                    _handle.Close();
                    _handle.Dispose();
                    _handle = null;
                }
            }
        }
    }
}