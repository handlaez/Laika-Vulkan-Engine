using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Runtime.Loader;

namespace Laika;

public static unsafe class ScriptHost
{
    private static Assembly? gameAssembly;

    private static readonly Dictionary<uint, MonoBehaviour> instances = new();

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static int Initialize(nint assemblyPathUtf8)
    {
        try
        {
            string? assemblyPath = Marshal.PtrToStringUTF8(assemblyPathUtf8);

            if (string.IsNullOrWhiteSpace(assemblyPath))
            {
                return -1;
            }

            assemblyPath = Path.GetFullPath(assemblyPath);

            Console.WriteLine($"Loading game assembly: {assemblyPath}");
            Console.WriteLine($"Laika.Managed loaded from: " + $"{typeof(ScriptHost).Assembly.Location}");

            AssemblyLoadContext.Default.Resolving += (context, assemblyName) =>
                {
                    if (assemblyName.Name == "Laika.Managed")
                    {
                        return typeof(ScriptHost).Assembly;
                    }
                    return null;
                };

            gameAssembly = AssemblyLoadContext.Default.LoadFromAssemblyPath(assemblyPath);
            Console.WriteLine($"Loaded game assembly: {gameAssembly.FullName}");

            return 0;
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex);
            return -1;
        }
    }

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static int Create(uint actorId, nint typeNameUtf8)
    {
        try
        {
            if (gameAssembly == null)
            {
                return -1;
            }

            string? typeName = Marshal.PtrToStringUTF8(typeNameUtf8);

            if (string.IsNullOrWhiteSpace(typeName))
            {
                return -1;
            }

            Type? type = gameAssembly.GetType(typeName);

            if (type == null)
            {
                Console.WriteLine($"Script type not found: {typeName}");
                Console.WriteLine("Available types:");

                foreach (var availableType in gameAssembly.GetTypes())
                {
                    Console.WriteLine($"  {availableType.FullName}");
                }

                return -1;
            }

            if (!typeof(MonoBehaviour).IsAssignableFrom(type))
            {
                Console.WriteLine($"{typeName} does not derive from MonoBehaviour");
                return -1;
            }

            var instance = (MonoBehaviour?)Activator.CreateInstance(type);

            if (instance == null)
            {
                return -1;
            }

            instance.Entity = new Entity(actorId);
            instances[actorId] = instance;
            instance.OnStart();

            return 0;
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex);
            return -1;
        }
    }

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static int Update(uint actorId, float deltaTime, float* position)
    {
        try
        {
            if (!instances.TryGetValue(actorId, out var instance))
            {
                return -1;
            }

            instance.Entity.Transform.Position = new Vector3(position[0], position[1], position[2]);
            instance.OnUpdate(deltaTime);

            var result = instance.Entity.Transform.Position;

            position[0] = result.X;
            position[1] = result.Y;
            position[2] = result.Z;

            return 0;
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex);
            return -1;
        }
    }

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static void DestroyAll()
    {
        foreach (var instance in instances.Values)
        {
            try
            {
                instance.OnDestroy();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }

        instances.Clear();
    }
}