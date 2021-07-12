using System;
using System.Linq;
using DiskCore;
using LibHac.Common.Keys;
using LibHac.FsSystem;

namespace SaveSwitchCore
{
    public class SaveSwitch
    {
        public void AutoMount()
        {
            var switchDisk = DiskInfo.GetDisks().FirstOrDefault(IsDiskSwitch);
            if (switchDisk == null)
                throw new SaveSwitchException("Switch disk not found!");

            Console.WriteLine($"Found switch disk: {switchDisk}");

            var keys = new KeySet();
            ExternalKeyReader.ReadKeyFile(keys, "prod.keys", null, null, null);

            using var nand = new SwitchNand(keys, switchDisk);
            foreach (var save in nand.GetSaves())
            {
                Console.WriteLine($"{save.Header.ExtraData.TitleId}");
                foreach (var e in save.EnumerateEntries())
                    Console.WriteLine(e.FullPath);
            }
        }

        private bool IsDiskSwitch(DiskInfo disk)
        {
            return disk.Caption == "Linux UMS disk 0 USB Device" && disk.Partitions == 11;
        }
    }
}