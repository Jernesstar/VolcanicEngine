#include "../../Project/App/TestGame.as"

class SomeScriptClass
{
    uint32 Int = 0;
}

class PlayerDied : GameEvent
{
    string GetID() const { return "PlayerDied"; }
}

class PlayerController : IEntityController
{
    Entity Handle;
    SomeScriptClass Class;
    [EditorField] Asset soundAsset;
    [EditorField] Asset meshAsset;
 
    [EditorField] bool SomeBool;
    [EditorField] int8 Signed8;
    [EditorField] uint32 Unsigned32;
    [EditorField] float Float;

    [EditorField] array<uint32> List;
    [EditorField] GridSet Grid;
    [EditorField] Vec3 SomeVec3;

    PlayerController(Entity entity)
    {
        Handle = entity;
    }

    void OnStart()
    {
        print("OnStart");

        TestGame@ game = cast<TestGame>(ScriptApp);
        game.NewMethod();
        print("Coins: " + game.State.Coins);

        print("Name: " + Handle.Name);
        print("Alive: " + Handle.IsAlive);
        const TagComponent@ tag = Handle.GetTagComponent();
        print(tag.Tag);

        AssetManager.Load(soundAsset);
        print("Loaded: " + soundAsset.IsLoaded);
        print("Name: " + soundAsset.Name);

        ScriptSystem@ sys = Scene.GetScriptSystem();
        sys.ListenForEvent(Handle, "PlayerDied");

        AssetManager.Load(meshAsset);

        for(uint32 y = 0; y < Grid.Height; y++) {
            for(uint32 x = 0; x < Grid.Width; x++) {
                uint8 data = Grid.At(x, y);
                if(data == 0)
                    continue;

                Entity new = Scene.NewEntity();
                TransformComponent@ tc = new.AddTransformComponent();
                tc.Translation.x = x;
                tc.Translation.y = y;
                MeshComponent@ mc = new.AddMeshComponent();
                mc.MeshAsset = meshAsset;
            }
        }
    }

    void OnUpdate(float ts)
    {
        TransformComponent@ tc = Handle.SetTransformComponent();

        if(Input::KeyPressed(Key::Left))
            tc.Translation.x -= 0.005f * ts;
        if(Input::KeyPressed(Key::Right))
            tc.Translation.x += 0.005f * ts;
        if(Input::KeyPressed(Key::Up))
            tc.Translation.z -= 0.005f * ts;
        if(Input::KeyPressed(Key::Down))
            tc.Translation.z += 0.005f * ts;
        if(Input::KeyPressed(Key::Q))
            tc.Translation.y += 0.005f * ts;
        if(Input::KeyPressed(Key::E))
            tc.Translation.y -= 0.005f * ts;
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

        print("Key: " + e.Key);
        if(e.Key == Key::Space and !e.IsRepeat)
        {
            print("Playing sound");
            Sound@ sound = AssetManager.GetSound(soundAsset);
            sound.Play();
        }

        ScriptSystem@ sys = Scene.GetScriptSystem();
        // PlayerDied@ gameEvent = PlayerDied();
        // sys.BroadcastEvent(Handle, @gameEvent);
    }

    void OnMouseEvent(MouseEvent@ event)
    {
        // print("Mouse");
        // if(event.Type != EventType::MousePressed)
        //     return;
        // print("Press");

        // MousePressedEvent@ e = cast<MousePressedEvent>(event);
        // print("Button is " + e.Button);
    }

    void OnPhysicsEvent(PhysicsEvent@ event)
    {
        // if(event.Type == PhysicsEventType::MousePress) {
        //     print("Pressed");
        //     return;
        // }

        // if(event.EntityHandle.GetTagComponent().Tag == "Lava") {
        //     ScriptSystem@ sys = Scene.GetScriptSystem();
        //     sys.BroadcastEvent(Handle, "PlayerDied");
        // }
    }

    void OnGameEvent(GameEvent@ event)
    {
        PlayerDied@ e = cast<PlayerDied>(event);
        if(@e != null)
        {
            print("Game over");
        }
    }
}
