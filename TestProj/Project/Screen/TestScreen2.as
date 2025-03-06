
class TestScreen2 : IScreen
{
    Scene @SceneHandle;
    UIPage @UIHandle;

    TestScreen2(Scene @world, UIPage @ui)
    {
        @SceneHandle = world;
        @UIHandle = ui;
    }

    void OnLoad()
    {

    }

    void OnClose()
    {

    }

    void OnUpdate(float ts)
    {

    }

}
