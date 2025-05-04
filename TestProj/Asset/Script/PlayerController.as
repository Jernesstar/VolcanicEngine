#include "../../Project/App/TestGame.as"

class SomeScriptClass
{
    uint32 Int = 0;
}

class PlayerDied
{

}

class PlayerController : IEntityController
{
    Entity Handle;
    SomeScriptClass Class;
    Asset soundAsset = Asset(16624656892553940704, AssetType::Audio);
    Asset meshAsset = Asset(10036008063945961451, AssetType::Mesh);
 
    [EditorField] bool SomeBool;
    [EditorField] int8 Signed8;
    [EditorField] uint32 Unsigned32;
    [EditorField] float Float;

    [Tilemap]
    [EditorField] array<uint32> Tilemap;
    [EditorField] uint32 Width;
    [EditorField] uint32 Height;

    PlayerController(Entity entity)
    {
        Handle = entity;

        TestGame@ game = cast<TestGame>(ScriptApp);
        game.NewMethod();
        print("Coins: " + game.State.Coins);

        print("Name: " + entity.Name);
        print("Alive: " + entity.Alive);
        const TagComponent@ tc = Handle.GetTagComponent();
        print(tc.Tag);

        AssetManager.Load(soundAsset);
        bool loaded = AssetManager.IsLoaded(soundAsset);
        print("Loaded: " + loaded);

        ScriptSystem@ sys = Scene.GetScriptSystem();
        sys.ListenForEvent(Handle, "PlayerDied");
    }

    void OnStart()
    {
        print("OnStart");
        AssetManager.Load(meshAsset);

        for(uint32 i = 0; i < Tilemap.length(); i++) {
            uint32 data = Tilemap[i];
            print("Index: " + data);
            Entity new = Scene.NewEntity();
            TransformComponent@ tc = new.AddTransformComponent();
            tc.Translation.x = float(i);
            tc.Translation.y = data;
            MeshComponent@ mc = new.AddMeshComponent();
            mc.MeshAsset = meshAsset;
        }
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

        print("Key: " + e.Key);
        if(e.Key == Key::Space and !e.IsRepeat)
        {
            print("Playing sound");
            Sound@ sound = AssetManager.GetSound(soundAsset);
            sound.Play();
        }
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
