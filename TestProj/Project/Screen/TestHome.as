
class TestHome : IScreen
{
    IEntityController@ Controller;

    TestHome()
    {
#if EDITOR
        print("This is the Editor");
#endif
    }

    void OnLoad()
    {
        Entity player = Scene.FindEntity("Player");
        if(!player.IsValid) {
            print("Error");
        }
        else {
            print("Found Player Entity");
            @Controller = player.SetScriptComponent().Instance;
        }

        print("TestHome screen loaded");
    }

    void OnClose()
    {
        print("TestHome screen closed");
    }

    void OnUpdate(float ts)
    {

    }
}
