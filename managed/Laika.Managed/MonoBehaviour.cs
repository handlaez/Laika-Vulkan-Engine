namespace Laika;

public abstract class MonoBehaviour
{
    public Entity Entity { get; internal set; } = null!;

    public virtual void OnStart()
    {
    }

    public virtual void OnUpdate(float deltaTime)
    {
    }

    public virtual void OnDestroy()
    {
    }
}