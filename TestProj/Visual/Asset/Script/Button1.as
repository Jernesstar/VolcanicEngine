
class Button1 : IUIObject
{
    string ID;

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
        App.SetScreen("TestScreen2");
    }

    void OnMouseDown()
    {
        // print("Mouse down");
    }
}
