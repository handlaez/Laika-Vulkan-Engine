using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Runtime.Loader;

namespace Laika;

public static unsafe class ScriptHost
{
    private sealed class GameLoadContext : AssemblyLoadContext
    {
        private readonly AssemblyDependencyResolver resolver;
        private readonly Assembly managedAssembly;

        public GameLoadContext(
            string assemblyPath,
            Assembly managedAssembly)
            : base(
                $"Laika.Game.{Guid.NewGuid()}",
                isCollectible: true)
        {
            resolver = new AssemblyDependencyResolver(assemblyPath);
            this.managedAssembly = managedAssembly;

            Resolving += OnResolving;
        }

        protected override Assembly? Load(AssemblyName assemblyName)
        {
            Console.WriteLine($"[Managed] Load requested: {assemblyName.FullName}");

            if (string.Equals(
                    assemblyName.Name,
                    managedAssembly.GetName().Name,
                    StringComparison.OrdinalIgnoreCase))
            {
                Console.WriteLine($"[Managed] Sharing: {managedAssembly.FullName}");

                return managedAssembly;
            }

            string? resolvedPath =
                resolver.ResolveAssemblyToPath(assemblyName);

            if (resolvedPath == null)
            {
                Console.WriteLine($"[Managed] Resolver could not find: {assemblyName.FullName}");

                return null;
            }

            Console.WriteLine($"[Managed] Resolver found: {resolvedPath}");

            return LoadFromAssemblyPath(resolvedPath);
        }

        private Assembly? OnResolving(AssemblyLoadContext context, AssemblyName assemblyName)
        {
            Console.WriteLine($"[Managed] Resolving event: {assemblyName.FullName}");

            if (string.Equals(assemblyName.Name, managedAssembly.GetName().Name, StringComparison.OrdinalIgnoreCase))
            {
                Console.WriteLine($"[Managed] Resolving -> shared Laika.Managed");
                return managedAssembly;
            }

            return null;
        }
    }

    private static Assembly? gameAssembly;
    private static GameLoadContext? gameLoadContext;

    private static readonly Dictionary<uint, List<MonoBehaviour>> instances = new();

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
            Console.WriteLine($"Laika.Managed loaded from: {typeof(ScriptHost).Assembly.Location}");

            var managedAssembly = typeof(ScriptHost).Assembly;

            DestroyAllInstances();
            UnloadGameAssembly();

            gameLoadContext = new GameLoadContext(assemblyPath, managedAssembly); 

            gameAssembly = gameLoadContext.LoadFromAssemblyPath(assemblyPath);
            Console.WriteLine($"[Managed] Game assembly: {gameAssembly.FullName}");
            Console.WriteLine($"[Managed] Game context: " + $"{AssemblyLoadContext.GetLoadContext(gameAssembly)?.Name}");

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

            if (!instances.TryGetValue(actorId, out var actorInstances))
            {
                actorInstances = new List<MonoBehaviour>();
                instances[actorId] = actorInstances;
            }

            actorInstances.Add(instance);

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
            if (!instances.TryGetValue(actorId, out var actorInstances))
            {
                return -1;
            }

            var entity = new Entity(actorId);

            entity.Transform.Position = new Vector3(position[0], position[1], position[2]);

            foreach (var instance in actorInstances)
            {
                instance.Entity = entity;
                instance.OnUpdate(deltaTime);
            }

            var result = entity.Transform.Position;

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

    private static void DestroyAllInstances()
    {
        foreach (var actorInstances in instances.Values)
        {
            foreach (var instance in actorInstances)
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
        }

        instances.Clear();
    }

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static void DestroyAll()
    {
        DestroyAllInstances();
        UnloadGameAssembly();
    }

    private static void UnloadGameAssembly()
    {
        gameAssembly = null;

        gameLoadContext?.Unload();
        gameLoadContext = null;

        GC.Collect();
        GC.WaitForPendingFinalizers();
        GC.Collect();
    }
}

