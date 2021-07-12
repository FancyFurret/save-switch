# SaveSwitch

SaveSwitch automatically syncs your PC and Switch saves, so you can easily play on either system. All you'll need to do
is put your switch into RCM mode, plug in to your computer, and run SaveSwitch. That's it!

Currently SaveSwitch only supports Windows, but support for macOS and Linux can be added fairly easily by implementing
the interfaces in DiskCoreCommon.

## Configuration

Next to SaveSwitch.exe, there is a `config.json` file that needs to be filled out in order to start syncing games.
Here's an example of how it works:

```jsonc
// TODO
{
}
```

## Building

Simply open the .sln file in JetBrains Rider or Visual Studio and build, all dependencies are handled via Nuget.