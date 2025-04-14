
class TestHome : IScreen
{
    TestHome()
    {
// #ifndef EDITOR
//         // App.LoadScene("Scene");
// #endif
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