#include "../../../Project/App/TestGame.as"

class PlayerController : IEntity
{
    Entity handle;

    PlayerController(Entity entity)
    {
        handle = entity;
        print("Name: " + entity.Name);
        print("Alive: " + entity.Alive);
        const TagComponent @tc = handle.GetTagComponent();
        print(tc.Tag);

        // TestGame @game = cast<TestGame>(ScriptApp);
        // ScriptApp;
    }

    void OnUpdate(float ts)
    {
        // TransformComponent @tc = handle.SetTransformComponent();
        // print("Component: { " + tc.Translation.x + "," + tc.Translation.y + "," + tc.Translation.z + " }");

        // if(Input.KeyPressed(Key::Left))
            // tc.Translation.x -= 4.0 * ts;
        // if(Input.KeyPressed(Key::Right))
        //     component.Translation.x += 4.0 * ts;
    }
}