
class PlayerController : IEntity
{
    Entity handle;

    PlayerController(Entity entity)
    {
        handle = entity;
    }

    void OnUpdate(float ts)
    {
        TransformComponent &component = handle.SetTransformComponent();

        if(Input.KeyPressed(Key::Left))
            component.Translation.x -= 4.0 * ts;
        if(Input.KeyPressed(Key::Right))
            component.Translation.x += 4.0 * ts;
    }
}