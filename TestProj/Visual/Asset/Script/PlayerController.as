
class PlayerController : IEntity
{
    PlayerController(Entity entity)
    {
        m_Handle = entity;
    }

    void OnUpdate(float ts)
    {
        ref component = m_Handle.GetTransformComponent();

        if(Input.KeyPressed(Key::Left))
            component.Translation.x -= 4.0 * ts;
        if(Input.KeyPressed(Key::Right))
            component.Translation.x += 4.0 * ts;
    }

    Entity m_Handle;
}