
class TestHome : IScreen
{
    TestHome(Scene &world, UIPage &ui)
    {
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