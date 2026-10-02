using Laika;

namespace Laika.GameTest;

public sealed class TestMover : MonoBehaviour
{
    public override void OnStart()
    {
        Console.WriteLine($"TestMover started on actor {Entity.Id}");
    }

    public override void OnUpdate(float deltaTime)
    {
        var position = Entity.Transform.Position;

        position.X += deltaTime;

        Entity.Transform.Position = position;
    }
}