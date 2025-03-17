#include "../../../Project/App/TestGame.as"

class PlayerController : IEntity
{
    Entity handle;
    Asset asset = Asset(16624656892553940704, AssetType::Audio);
    bool SomeBool;
    int8 Signed8;
    uint32 Unsigned32;
    float Float;

    PlayerController(Entity entity)
    {
        handle = entity;
        print("Name: " + entity.Name);
        print("Alive: " + entity.Alive);
        const TagComponent@ tc = handle.GetTagComponent();
        print(tc.Tag);

        TestGame@ game = cast<TestGame>(ScriptApp);
        game.NewMethod();
        print("Coins: " + game.State.Coins);

        AssetManager.Load(asset);
        bool loaded = AssetManager.IsLoaded(asset);
        print("Loaded: " + loaded);
    }

    void OnUpdate(float ts)
    {
        if(KeyPressed(Key::Space)) {
            // Sound@ sound = AssetManager.GetSound(asset);
            // sound.Play();
            print("Space");
        }

        TransformComponent@ tc = handle.SetTransformComponent();

        if(KeyPressed(Key::Left))
            tc.Translation.x -= 0.05f * ts;
        if(KeyPressed(Key::Right))
            tc.Translation.x += 0.05f * ts;
        if(KeyPressed(Key::Up))
            tc.Translation.z -= 0.05f * ts;
        if(KeyPressed(Key::Down))
            tc.Translation.z += 0.05f * ts;
    }
}