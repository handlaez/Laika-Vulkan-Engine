using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Laika.ScriptTest;

public static class TestEntry
{
    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvStdcall) })]
    public static int Run(nint arg, int argSize)
    {
        Console.WriteLine("Hello from Laika C#!");

        return 42;
    }
}