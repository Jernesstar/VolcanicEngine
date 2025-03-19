#include "../../../Project/App/TestGame.as"

class PlayerController : IEntity
{
    Entity Handle;
    Asset asset = Asset(16624656892553940704, AssetType::Audio);
    bool SomeBool;
    int8 Signed8;
    uint32 Unsigned32;
    float Float;

    PlayerController(Entity entity)
    {
        Handle = entity;
        print("Name: " + entity.Name);
        print("Alive: " + entity.Alive);
        const TagComponent@ tc = Handle.GetTagComponent();
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
        TransformComponent@ tc = Handle.SetTransformComponent();

        if(KeyPressed(Key::Left))
            tc.Translation.x -= 0.05f * ts;
        if(KeyPressed(Key::Right))
            tc.Translation.x += 0.05f * ts;
        if(KeyPressed(Key::Up))
            tc.Translation.z -= 0.05f * ts;
        if(KeyPressed(Key::Down))
            tc.Translation.z += 0.05f * ts;
    }

    void OnKeyEvent(KeyEvent@ event)
    {
        // KeyPressedEvent@ e = cast<KeyPressedEvent>(event);
        // if(e == null)
        //     return;

        // if(e.Key == Key::Space and !e.IsRepeat) {
        //     Sound@ sound = AssetManager.GetSound(asset);
        //     sound.Play();
        // }
    }

    void OnMouseEvent(MouseEvent@ event)
    {
        print("Press");
        if(!Handle.HasRigidBodyComponent())
            return;

        // PhysicsSystem@ world = Scene.GetPhysicsSystem();
        // HitInfo info = world.RayTest(event.x, event.y);
        // if(info.HasHit and info.Body.EntityHandle == Handle)
        //     print("Hit");
    }

    void OnAppEvent(AppEvent@ event)
    {

    }

    // void OnPhysicsEvent(PhysicsEvent@ event)
    // {

    // }

    void OnGameEvent(GameEvent@ event)
    {
        // if(event.ID == "Collision") {
        //     CollisionEvent@ e = 
        // }
    }
}