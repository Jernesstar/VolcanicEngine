
class TestScreen2 : IScreen
{
    TestScreen2()
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
            // @Controller = player.SetScriptComponent().Instance;
        }

        print("TestScreen2 Load");
    }

    void OnClose()
    {
        print("TestScreen2 Close");
    }

    void OnUpdate(float ts)
    {

    }
}
