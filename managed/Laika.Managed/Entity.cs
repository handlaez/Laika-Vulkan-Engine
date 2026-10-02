namespace Laika;

public sealed class Entity
{
    public uint Id { get; }

    public Transform Transform { get; }

    internal Entity(uint id)
    {
        Id = id;
        Transform = new Transform();
    }
}

public sealed class Transform
{
    public Vector3 Position { get; set; }
}