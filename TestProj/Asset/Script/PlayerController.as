#include "../../Project/App/TestGame.as"

class SomeScriptClass
{
    uint32 Int = 0;
}

class PlayerController : IEntityController
{
    SomeScriptClass Class;
    Entity Handle;
    Asset asset = Asset(16624656892553940704, AssetType::Audio);
    bool SomeBool;
    int8 Signed8;
    uint32 Unsigned32;
    float Float;

    PlayerController(Entity entity)
    {
        TestGame@ game = cast<TestGame>(ScriptApp);
        game.NewMethod();
        print("Coins: " + game.State.Coins);

        Handle = entity;
        print("Name: " + entity.Name);
        print("Alive: " + entity.Alive);
        const TagComponent@ tc = Handle.GetTagComponent();
        print(tc.Tag);

        AssetManager.Load(asset);
        bool loaded = AssetManager.IsLoaded(asset);
        print("Loaded: " + loaded);

        ScriptSystem@ sys = Scene.GetScriptSystem();
        // sys.ListenForEvent(Handle, "PlayerDied");
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
        if(KeyPressed(Key::Q))
            tc.Translation.y += 0.05f * ts;
        if(KeyPressed(Key::E))
            tc.Translation.y -= 0.05f * ts;
    }

    void OnKeyEvent(KeyEvent@ event)
    {
        if(event.Type == EventType::KeyCharacter) {
            KeyCharacterEvent@ e = cast<KeyCharacterEvent>(event);
            print("Inputted character: '" + e.Char + "'");
        }

        KeyPressedEvent@ e = cast<KeyPressedEvent>(event);
        if(@e == null)
            return;

        if(e.Key == Key::Space and !e.IsRepeat)
        {
            print("Playing sound");
            Sound@ sound = AssetManager.GetSound(asset);
            sound.Play();
        }
    }

    void OnMouseEvent(MouseEvent@ event)
    {
        // print("Mouse");
        // if(event.Type != MouseEventType::Pressed)
        //     return;
        // print("Press");

        // MousePressedEvent@ e = cast<MousePressedEvent>(event);
    }

    void OnPhysicsEvent(PhysicsEvent@ event)
    {
        // if(event.Type == PhysicsEventType::MousePress) {
        //     print("Pressed");
        //     return;
        // }

        // if(event.EntityHandle.GetTagComponent().Tag == "Lava") {
            ScriptSystem@ sys = Scene.GetScriptSystem();
            sys.BroadcastEvent(Handle, "PlayerDied");
        // }
    }

    void OnGameEvent(GameEvent@ event)
    {
        // if(event.ID == "PlayerDied")
        // {
        //     print("Game over");
        // }
    }
}