
class PlayerController : IEntity
{
    Entity handle;

    PlayerController(Entity entity)
    {
        handle = entity;
    }

    void OnUpdate(float ts)
    {
        // ref component = handle.GetTransformComponent();

        // if(Input.KeyPressed(Key::Left))
        //     component.Translation.x -= 4.0 * ts;
        // if(Input.KeyPressed(Key::Right))
        //     component.Translation.x += 4.0 * ts;
    }
}