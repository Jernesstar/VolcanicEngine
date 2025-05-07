
class Button1 : IUIController
{
    string ID;

    [EditorField] uint32 Int;

    Button1(string id)
    {
        ID = id;
    }

    void OnUpdate(float ts)
    {

    }

    void OnClick()
    {
        print("Clicked " + ID);
    }

    void OnHover()
    {
        // print("Hovered");
    }

    void OnMouseUp()
    {
        print("Mouse up");
        App.SwitchScreen("TestScreen2");
    }

    void OnMouseDown()
    {
        // print("Mouse down");
    }
}
