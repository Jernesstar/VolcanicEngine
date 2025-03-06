
class TestHome : IScreen
{
    // Scene @SceneHandle;
    // UIPage @UIHandle;

    TestHome(Scene &world, UIPage &ui)
    {
        // @SceneHandle = world;
        // @UIHandle = ui;
    }

    void OnLoad()
    {
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