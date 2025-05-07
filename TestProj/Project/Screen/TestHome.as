
class TestHome : IScreen
{
    TestHome()
    {
#if EDITOR
        print("This is the Editor");
#endif
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